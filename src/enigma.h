#ifndef ENIGMA_ENIGMA_H
#define ENIGMA_ENIGMA_H

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef ENIGMA_EXTRA_HEADER
#include "ENIGMA_EXTRA_HEADER"
#endif

#ifndef ENIGMA_MALLOC
#define ENIGMA_MALLOC malloc
#endif

#ifndef ENIGMA_FREE
#define ENIGMA_FREE free
#endif

#if defined(_WIN32) || defined(_WIN64)
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif /* _WIN32 */

typedef uint8_t replace_map[256];

typedef struct enigma_machine_s {
    size_t rollers_count;
    uint8_t* roll_count;
    replace_map *forward_maps, *reverse_maps;
    uint8_t reflect_key;
    // 256
} enigma_machine_t;

DLLEXPORT enigma_machine_t*
enigma_machine_new(size_t rollers_count);

DLLEXPORT void
enigma_machine_del(enigma_machine_t* self);

DLLEXPORT void
enigma_machine_roll(enigma_machine_t* self, size_t idx, int count);

DLLEXPORT void
enigma_machine_encode_into(enigma_machine_t* self, const uint8_t* data, size_t len, uint8_t* out);

DLLEXPORT void
enigma_machine_encode_inplace(enigma_machine_t* self, uint8_t* data, size_t len);

// DLLEXPORT bool
// enigma_machine_test_replace(enigma_machine_t* self);

// DLLEXPORT bool
// enigma_machine_test_reflect(enigma_machine_t* self);

#endif  // ENIGMA_ENIGMA_H
