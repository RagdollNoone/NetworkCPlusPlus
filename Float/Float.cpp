//
// Created by dendy on 19-5-24.
//

#include <cstdio>
#include "Float.h"


uint32_t
htonf(float f) {
    uint32_t p;
    uint32_t sign;

    if (f < 0) {
        sign = 1;
        f = -f;
    } else {
        sign = 0;
    }


    // debug
//    uint32_t val1 = (uint32_t)f;
//    uint32_t val2 = val1 & 0x7fff;
//    uint32_t val3 = val2 << 16;
//    uint32_t val4 = sign << 31;
//    p = val3 | val4;
//    p |=  (uint32_t)(((f - (int)f) * 65536.0f))&0xffff;

    p = ((((uint32_t)f)&0x7fff)<<16) | (sign<<31); // whole part and sign
    p |= (uint32_t)(((f - (int)f) * 65536.0f))&0xffff; // fraction


    return p;
}

float
ntohf(uint32_t p) {
    float f =  ((p>>16)&0x7fff);
    f += (p&0xffff) / 65536.0f;

    if (((p>>31)&0x1) == 0x1) {
        f = -f;
    }

    return f;
}

void
floatTest() {
    float f1 = 3.1415926, f2;
    uint32_t  netf;

    netf = htonf(f1);
    f2 = ntohf(netf);

    printf("Original: %f\n", f1);
    printf("Network: 0x%08X\n", netf);
    printf("Unpack: %f\n", f2);
}

