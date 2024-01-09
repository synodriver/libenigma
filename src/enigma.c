#include "enigma.h"


uint8_t
_enigma_findindex(const uint8_t *map, uint8_t c)
{
    for (int i = 0; i < 256; i++)
    {
        if (map[i] == c)
        {
            return (uint8_t) i;
        }
    }
    return UINT8_MAX;
}


enigma_machine_t *
enigma_machine_new(const uint8_t *maps, size_t mapsize,
                   uint8_t (*reflect_func)(void *, uint8_t),
                   void *reflect_ud,
                   uint8_t (*replace_func)(void *, uint8_t),
                   void *replace_ud)
{
    if (maps == NULL || reflect_func == NULL || replace_func == NULL)
    {
        return NULL;
    }
    if (mapsize % 256 != 0)
    {
        return NULL;
    }
    enigma_machine_t *self = ENIGMA_MALLOC(sizeof(enigma_machine_t));
    if (self == NULL)
    {
        return NULL;
    }
    self->forward_maps = ENIGMA_MALLOC(mapsize);
    if (self->forward_maps == NULL)
    {
        ENIGMA_FREE(self);
        return NULL;
    }
    self->reverse_maps = ENIGMA_MALLOC(mapsize);
    if (self->reverse_maps == NULL)
    {
        ENIGMA_FREE(self->forward_maps);
        ENIGMA_FREE(self);
        return NULL;
    }
    self->reflect_func = reflect_func;
    self->reflect_ud = reflect_ud;
    self->replace_func = replace_func;
    self->replace_ud = replace_ud;
    self->encode_count = 0;
    self->rollers = mapsize / 256;
    for (int i = 0; i < self->rollers; i++)
    {
        for (int j = 0; j < 256; j++)
        {
            *(self->forward_maps + i * 256 + j) = *(maps + i * 256 + j) - (uint8_t) j;
            *(self->reverse_maps + i * 256 + j) = _enigma_findindex(maps + i * 256, (uint8_t) j) - (uint8_t) j;
        }
    }
    return self;
}

void
enigma_machine_del(enigma_machine_t *self)
{
    if (self->forward_maps)
    {
        ENIGMA_FREE(self->forward_maps);
    }
    if (self->reverse_maps)
    {
        ENIGMA_FREE(self->reverse_maps);
    }
    ENIGMA_FREE(self);
}

// roller idx start from 0
void
_enigma_machine_roll_once(enigma_machine_t *self, size_t idx)
{
    uint8_t *ptr_forward = self->forward_maps + idx * 256;
    uint8_t *ptr_reverse = self->reverse_maps + idx * 256;
    uint8_t temp_forward = ptr_forward[255];
    uint8_t temp_reverse = ptr_reverse[255];
    for (int i = 255; i > 0; i--)
    {
        ptr_forward[i] = ptr_forward[i - 1];
        ptr_reverse[i] = ptr_reverse[i - 1];
    }
    ptr_forward[0] = temp_forward;
    ptr_reverse[0] = temp_reverse;
}

void
enigma_machine_roll(enigma_machine_t *self, size_t idx, int count)
{
    for (int i = 0; i < count; i++)
    {
        _enigma_machine_roll_once(self, idx);
    }
}

void
_enigma_machine_inc(enigma_machine_t *self)
{
    self->encode_count++;
    size_t current = 1;
    for (size_t i = 0; i < self->rollers; i++)
    {
        if ((self->encode_count % current) == 0)
        {
            _enigma_machine_roll_once(self, i);
        }
        current *= 256;
    }
}


uint8_t
_enigma_machine_encode_one(enigma_machine_t *self, uint8_t code)
{
    uint8_t replaced = self->replace_func(self->replace_ud, code);
    for (size_t i = 0; i < self->rollers; i++)
    {
        replaced += *(self->forward_maps + i * 256 + replaced);
    }
    replaced = self->reflect_func(self->reflect_ud, replaced);
    for (size_t i = self->rollers; i > 0; i--)
    {
        replaced += *(self->reverse_maps + (i - 1) * 256 + replaced);
    }
    _enigma_machine_inc(self);
    return replaced;
}

void
enigma_machine_encode_into(enigma_machine_t *self, const uint8_t *data, size_t len, uint8_t *out)
{
    for (size_t i = 0; i < len; i++)
    {
        out[i] = _enigma_machine_encode_one(self, data[i]);
    }
}

void
enigma_machine_encode_inplace(enigma_machine_t *self, uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        data[i] = _enigma_machine_encode_one(self, data[i]);
    }
}

bool
enigma_machine_test_replace(enigma_machine_t *self)
{
    uint8_t y;
    for (int i = 0; i < 256; i++)
    {
        y = self->replace_func(self->replace_ud, (uint8_t) i);
        if ((uint8_t) i != self->replace_func(self->replace_ud, y))
        {
            return false;
        }
    }
    return true;
}

bool
enigma_machine_test_reflect(enigma_machine_t *self)
{
    uint8_t y;
    for (int i = 0; i < 256; i++)
    {
        y = self->reflect_func(self->reflect_ud, (uint8_t) i);
        if (y == (uint8_t) i)
        {
            return false;
        }
        if ((uint8_t) i != self->reflect_func(self->reflect_ud, y))
        {
            return false;
        }
    }
    return true;
}