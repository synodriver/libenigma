#include "enigma.h"

void _get_inverse_map(const replace_map map, replace_map target) {
    for (int i = 0; i < 256; i++)
        target[map[i]] = i;
}

void swap(uint8_t* a, uint8_t* b) {
    uint8_t t = *a;
    *a = *b;
    *b = t;
}

void _random_shuffle(replace_map map) {
    for (int i = 0; i < 256; i++)
        swap(map + i, map + rand() % 256);
}

enigma_machine_t*
enigma_machine_new(size_t rollers_count) {
    enigma_machine_t* self = ENIGMA_MALLOC(sizeof(enigma_machine_t));
    self->forward_maps = malloc(sizeof(replace_map) * rollers_count);
    self->reverse_maps = malloc(sizeof(replace_map) * rollers_count);
    self->roll_count = malloc(sizeof(uint8_t) * rollers_count);
    self->reflect_key = rand() % 256;
    memset(self->roll_count, rollers_count, 0);
    for (int i = 0; i < rollers_count; i++) {
        for (int j = 0; j < 256; j++)
            self->forward_maps[i][j] = j;
        _random_shuffle(self->forward_maps[i]);
        for (int j = 0; j < 256; j++)
            printf("%d ", self->forward_maps[i][j]);
        puts("\n\n");
        _get_inverse_map(self->forward_maps[i], self->reverse_maps[i]);
        for (int j = 0; j < 256; j++)
            printf("%d ", self->reverse_maps[i][j]);
        puts("\n\n\n");
    }
    return self;
}

void enigma_machine_del(enigma_machine_t* self) {
    if (self->forward_maps) {
        ENIGMA_FREE(self->forward_maps);
    }
    if (self->reverse_maps) {
        ENIGMA_FREE(self->reverse_maps);
    }
    if (self->roll_count) {
        ENIGMA_FREE(self->roll_count);
    }
    ENIGMA_FREE(self);
}

void _enigma_machine_roll_one(enigma_machine_t* self, size_t idx) {
    self->roll_count[idx]++;
    if (self->roll_count[idx] == 0 && idx + 1 < self->rollers_count)
        _enigma_machine_roll_one(self, idx + 1);
}

uint8_t _enigma_machine_get_replaced_byte(enigma_machine_t* self, uint8_t byte) {
    for (int i = 0; i < self->rollers_count; i++)
        byte = self->forward_maps[i][byte - self->roll_count[i]];
    byte ^= self->reflect_key;
    for (int i = self->rollers_count - 1; i >= 0; i--)
        byte = self->reverse_maps[i][byte] + self->roll_count[i];
    return byte;
}

uint8_t _enigma_machine_encode_one(enigma_machine_t* self, uint8_t byte) {
    uint8_t res = _enigma_machine_get_replaced_byte(self, byte);
    _enigma_machine_roll_one(self, 0);
    return res;
}

void enigma_machine_roll(enigma_machine_t* self, size_t idx, int count) {
    self->roll_count[idx] += count;
}

void enigma_machine_encode_into(enigma_machine_t* self, const uint8_t* data, size_t len, uint8_t* out) {
    for (size_t i = 0; i < len; i++) {
        out[i] = _enigma_machine_encode_one(self, data[i]);
    }
}

void enigma_machine_encode_inplace(enigma_machine_t* self, uint8_t* data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        data[i] = _enigma_machine_encode_one(self, data[i]);
    }
}

// bool enigma_machine_test_replace(enigma_machine_t* self) {
//     uint8_t y;
//     for (int i = 0; i < 256; i++) {
//         y = self->replace_func(self->replace_ud, (uint8_t)i);
//         if ((uint8_t)i != self->replace_func(self->replace_ud, y)) {
//             return false;
//         }
//     }
//     return true;
// }

// bool enigma_machine_test_reflect(enigma_machine_t* self) {
//     uint8_t y;
//     for (int i = 0; i < 256; i++) {
//         y = self->reflect_func(self->reflect_ud, (uint8_t)i);
//         if (y == (uint8_t)i) {
//             return false;
//         }
//         if ((uint8_t)i != self->reflect_func(self->reflect_ud, y)) {
//             return false;
//         }
//     }
//     return true;
// }
