#include "enigma.h"


static uint8_t
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
        goto fail;
    }
    if (mapsize % 256 != 0)
    {
        goto fail;
    }
    enigma_machine_t *self = ENIGMA_MALLOC(sizeof(enigma_machine_t));
    if (self == NULL)
    {
        goto fail;
    }
    self->forward_maps = ENIGMA_MALLOC(mapsize);
    if (self->forward_maps == NULL)
    {
        goto fail1;
    }
    self->reverse_maps = ENIGMA_MALLOC(mapsize);
    if (self->reverse_maps == NULL)
    {
        goto fail2;
    }
    self->rollers = mapsize / 256;
    self->offset = ENIGMA_MALLOC(sizeof(uint8_t)*self->rollers);
    if(self->offset==NULL)
    {
        goto fail3;
    }
    memset(self->offset, 0, self->rollers);
    self->reflect_func = reflect_func;
    self->reflect_ud = reflect_ud;
    self->replace_func = replace_func;
    self->replace_ud = replace_ud;
    self->encode_count = 0;

    for (int i = 0; i < self->rollers; i++)
    {
        for (int j = 0; j < 256; j++)
        {
            *(self->forward_maps + i * 256 + j) = *(maps + i * 256 + j) - (uint8_t) j;
            *(self->reverse_maps + i * 256 + j) = _enigma_findindex(maps + i * 256, (uint8_t) j) - (uint8_t) j;
        }
    }
    return self;

fail3:
    ENIGMA_FREE(self->reverse_maps);
fail2:
    ENIGMA_FREE(self->forward_maps);
fail1:
    ENIGMA_FREE(self);
fail:
    return NULL;

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
    if(self->offset)
    {
        ENIGMA_FREE(self->offset);
    }
    ENIGMA_FREE(self);
}

// roller idx start from 0
static void
_enigma_machine_roll_once(enigma_machine_t *self, size_t idx)
{
    self->offset[idx]+=1;
}

void
enigma_machine_roll(enigma_machine_t *self, size_t idx, int count)
{
    self->offset[idx]+=count;
}

static void
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


static uint8_t
_enigma_machine_encode_one(enigma_machine_t *self, uint8_t code)
{
    uint8_t replaced = self->replace_func(self->replace_ud, code);
    for (size_t i = 0; i < self->rollers; i++)
    {
        replaced += *(self->forward_maps + i * 256 + (uint8_t)(replaced-self->offset[i]));
    }
    replaced = self->reflect_func(self->reflect_ud, replaced);
    for (size_t i = self->rollers; i > 0; i--)
    {
        replaced += *(self->reverse_maps + (i - 1) * 256 +(uint8_t)( replaced-self->offset[i-1]));
    }
    replaced = self->replace_func(self->replace_ud, replaced);
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

enigma_machine_t *
enigma_machine_dup(enigma_machine_t *self)
{
    enigma_machine_t *new = ENIGMA_MALLOC(sizeof(enigma_machine_t));
    if (new == NULL)
    {
        goto fail;
    }
    size_t mapsize = self->rollers * 256;
    new->forward_maps = ENIGMA_MALLOC(mapsize);
    if (new->forward_maps == NULL)
    {
        goto fail1;
    }
    new->reverse_maps = ENIGMA_MALLOC(mapsize);
    if (new->reverse_maps == NULL)
    {
        goto fail2;
    }
    new->rollers =self->rollers;
    new->offset = ENIGMA_MALLOC(sizeof(uint8_t)*new->rollers);
    if(new->offset==NULL)
    {
        goto fail3;
    }
    memcpy(new->forward_maps, self->forward_maps, mapsize);
    memcpy(new->reverse_maps, self->reverse_maps, mapsize);
    memcpy(new->offset, self->offset,sizeof(uint8_t)*new->rollers );
    new->encode_count = self->encode_count;

    new->replace_func = self->replace_func;
    new->replace_ud = self->replace_ud;
    new->reflect_func = self->reflect_func;
    new->reflect_ud  = self->reflect_ud;

    return new;

fail3:
    ENIGMA_FREE(new->reverse_maps);
fail2:
    ENIGMA_FREE(new->forward_maps);
fail1:
    ENIGMA_FREE(new);
fail:
    return NULL;
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

void
enigma_machine_dump_replace_table(enigma_machine_t *self, uint8_t *out)
{
    for (int i = 0; i < 256; i++)
    {
        out[i] = self->replace_func(self->replace_ud, (uint8_t)i);
    }

}

void
enigma_machine_dump_reflect_table(enigma_machine_t *self, uint8_t *out)
{
    for (int i = 0; i < 256; i++)
    {
        out[i] = self->reflect_func(self->reflect_ud, (uint8_t)i);
    }

}
