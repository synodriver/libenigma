#include <assert.h>

#include "enigma.h"

// 代表滚轮内部连线 每个滚轮256个数字，正好替换字节 这里有2个这样的滚轮，还可以更多
static uint8_t map[] = {
        99, 131, 166, 42, 162, 68, 243, 212, 152, 71, 24, 172, 151, 247, 113, 78, 18, 208, 111, 90, 199, 137, 121, 150,
        213, 124, 129, 7, 45, 128, 57, 67, 160, 56, 132, 8, 202, 175, 70, 93, 106, 31, 122, 33, 140, 20, 135, 97, 10,
        41, 27, 167, 144, 232, 102, 214, 184, 211, 220, 29, 168, 39, 98, 37, 117, 234, 86, 222, 66, 158, 237, 74, 245,
        224, 141, 164, 1, 9, 145, 210, 218, 206, 80, 50, 204, 182, 139, 250, 201, 198, 236, 88, 134, 83, 227, 51, 239,
        163, 186, 197, 35, 174, 94, 69, 207, 17, 77, 43, 194, 13, 114, 104, 115, 181, 53, 105, 40, 44, 25, 235, 133,
        217, 28, 165, 170, 48, 229, 16, 54, 116, 32, 142, 107, 251, 23, 209, 147, 82, 100, 95, 85, 5, 179, 19, 156, 188,
        60, 255, 240, 12, 47, 120, 123, 183, 76, 252, 92, 248, 244, 249, 203, 34, 185, 161, 241, 110, 242, 72, 109, 154,
        65, 14, 195, 81, 101, 159, 153, 96, 58, 230, 55, 87, 215, 146, 6, 216, 61, 205, 62, 189, 112, 149, 187, 143, 22,
        89, 191, 63, 192, 3, 49, 157, 155, 253, 91, 173, 177, 228, 103, 178, 125, 108, 171, 75, 119, 38, 26, 21, 138,
        180, 30, 4, 136, 226, 200, 196, 223, 11, 59, 238, 246, 254, 73, 193, 84, 64, 15, 79, 130, 225, 148, 118, 190, 0,
        2, 127, 176, 233, 219, 221, 126, 52, 231, 36, 46, 169, 43, 136, 8, 4, 93, 175, 84, 202, 239, 203, 149, 31, 246,
        126, 133, 82, 46, 216, 156, 190, 70, 74, 21, 15, 2, 153, 130, 200, 51, 14, 65, 22, 35, 104, 233, 117, 116, 134,
        177, 162, 131, 68, 163, 33, 9, 145, 90, 237, 232, 38, 132, 56, 83, 140, 124, 154, 221, 164, 36, 105, 85, 209,
        218, 248, 144, 123, 27, 201, 167, 44, 26, 250, 30, 155, 108, 54, 252, 191, 179, 135, 28, 205, 215, 120, 109, 52,
        72, 96, 128, 247, 100, 158, 80, 41, 249, 122, 139, 198, 199, 92, 143, 13, 244, 17, 176, 18, 178, 210, 147, 251,
        255, 91, 161, 127, 219, 7, 89, 118, 188, 168, 234, 119, 64, 114, 236, 112, 152, 174, 24, 23, 25, 62, 229, 102,
        45, 160, 146, 11, 228, 86, 60, 217, 59, 55, 61, 206, 103, 180, 181, 113, 183, 150, 245, 185, 99, 193, 94, 110,
        1, 71, 16, 63, 187, 230, 107, 224, 223, 40, 6, 235, 225, 49, 231, 69, 148, 3, 53, 57, 98, 77, 254, 207, 78, 189,
        166, 66, 5, 111, 20, 220, 37, 50, 125, 195, 115, 79, 226, 42, 142, 97, 242, 243, 182, 211, 81, 169, 165, 222,
        19, 241, 186, 213, 171, 48, 159, 138, 208, 32, 29, 47, 214, 170, 157, 58, 212, 197, 238, 34, 141, 87, 184, 227,
        137, 95, 129, 121, 151, 173, 73, 75, 39, 67, 101, 88, 253, 0, 192, 106, 76, 240, 12, 196, 10, 172, 204, 194
};

// 模拟反射板
uint8_t reflect(void *ud, uint8_t c)
{
    if (c % 2 == 0) // 0, 2, 254
    {
        return c + 1;
    }
    else
    {
        return c - 1;
    }
}

// 模拟外置接线桩 替换输入用
uint8_t replace(void *ud, uint8_t c)
{
    return c;
}

int main(int argc, char **argv)
{
    uint8_t buf[500];
    char *a = "hello enigma!hello enigma!hello enigma!hello enigma!hello enigma!hello enigma!hello enigma!hello enigma!hello enigma!hello enigma!hello enigma!hello enigma!hello enigma!hello enigma!hello enigma!hello enigma!hello enigma!hello enigma!hello enigma!hello enigma!hello enigma!";
    memcpy(buf, a, 274);
    enigma_machine_t *machine = enigma_machine_new(map, 512, reflect, NULL, replace, NULL);
    assert(machine->rollers == 2);
    assert(enigma_machine_test_replace(machine));
    assert(enigma_machine_test_reflect(machine));
    enigma_machine_roll(machine, 0, 10); // 把第0个滚轮转到10
    enigma_machine_roll(machine, 1, 20); // 把第1个滚轮转到20
    enigma_machine_encode_inplace(machine, buf, 10); // 加密  274
    enigma_machine_t *newmachine = enigma_machine_dup(machine);
    enigma_machine_del(machine);
    machine = NULL;
    assert(newmachine->rollers == 2);
    assert(enigma_machine_test_replace(newmachine));
    assert(enigma_machine_test_reflect(newmachine));
    enigma_machine_encode_inplace(newmachine, buf+10, 264);
    enigma_machine_del(newmachine);
    newmachine=NULL;
    FILE* out = fopen("encrypted.bin","wb");
    if (out==NULL)
    {
        fprintf(stderr, "can not open output file\n");
        return 1;
    }
    fwrite(buf, 1, 274, out);
    fwrite(buf, 1, 274, stderr);
    fclose(out);

    fprintf(stderr, "\n");
    /* decode */
    enigma_machine_t *decoder = enigma_machine_new(map, 512, reflect, NULL, replace, NULL);
    enigma_machine_roll(decoder, 0, 10);
    enigma_machine_roll(decoder, 1, 20);
    enigma_machine_encode_inplace(decoder, buf, 274);
    fwrite(buf, 1, 274, stderr);
    assert(strncmp(buf, a, 274) == 0);

    uint8_t reflect_table[256];
    enigma_machine_dump_reflect_table(decoder, reflect_table);
    fprintf(stderr, "\nreflect_table is \n");
    fwrite(reflect_table, 1, 256, stderr);
    fprintf(stderr, "\n");
    enigma_machine_dump_replace_table(decoder, reflect_table);
    fprintf(stderr, "replace_table is \n");
    fwrite(reflect_table, 1, 256, stderr);
    fprintf(stderr, "\n");
    enigma_machine_del(decoder);
    return 0;
}