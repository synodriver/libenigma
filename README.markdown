# Enigma 
### C写的恩尼格玛 不怎么高效 为了模拟机械结构只能串行加密，一次1字节


### 公开函数
```c
typedef struct enigma_machine_s
{
    size_t encode_count;
    size_t rollers;
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


enigma_machine_t *
enigma_machine_new(const uint8_t *maps, size_t mapsize,
uint8_t (*reflect_func)(void *, uint8_t),
void *reflect_ud,
uint8_t (*replace_func)(void *, uint8_t),
void *replace_ud);

void
enigma_machine_del(enigma_machine_t *self);

void
enigma_machine_roll(enigma_machine_t *self, size_t idx, int count);

void
enigma_machine_encode_into(enigma_machine_t *self, const uint8_t *data, size_t len, uint8_t *out);

void
enigma_machine_encode_inplace(enigma_machine_t *self, uint8_t *data, size_t len);

bool
enigma_machine_test_replace(enigma_machine_t *self);

bool
enigma_machine_test_reflect(enigma_machine_t *self);
```