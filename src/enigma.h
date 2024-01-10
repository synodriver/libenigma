#ifndef ENIGMA_ENIGMA_H
#define ENIGMA_ENIGMA_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

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
#elif
#define DLLEXPORT
#endif /* _WIN32 */

typedef struct enigma_machine_s
{
    size_t encode_count;
    size_t rollers;
    uint8_t *offset; // roller offset len=rollers
    uint8_t *forward_maps; // rollers*256
    uint8_t *reverse_maps; // rollers*256
    uint8_t (*reflect_func)(void *, uint8_t);

    // f(f(x))==x f(x)!=x
    void *reflect_ud;

    uint8_t (*replace_func)(void *, uint8_t);

    // f(f(x))==x
    void *replace_ud;
    // 256
} enigma_machine_t;


DLLEXPORT enigma_machine_t *
enigma_machine_new(const uint8_t *maps, size_t mapsize,
                   uint8_t (*reflect_func)(void *, uint8_t),
                   void *reflect_ud,
                   uint8_t (*replace_func)(void *, uint8_t),
                   void *replace_ud);

DLLEXPORT void
enigma_machine_del(enigma_machine_t *self);

DLLEXPORT void
enigma_machine_roll(enigma_machine_t *self, size_t idx, int count);

DLLEXPORT void
enigma_machine_encode_into(enigma_machine_t *self, const uint8_t *data, size_t len, uint8_t *out);

DLLEXPORT void
enigma_machine_encode_inplace(enigma_machine_t *self, uint8_t *data, size_t len);

DLLEXPORT enigma_machine_t *
enigma_machine_dup(enigma_machine_t *self);

DLLEXPORT bool
enigma_machine_test_replace(enigma_machine_t *self);

DLLEXPORT bool
enigma_machine_test_reflect(enigma_machine_t *self);

#endif //ENIGMA_ENIGMA_H
