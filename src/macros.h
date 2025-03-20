#pragma once

#define NKO_STR_HELPER(x) #x

#define NKO_STR(x) NKO_STR_HELPER(x)

#define NKO_ASSERT_DO(a, m, t) \
    if(!(a)) { Serial.println(m); t; }

#define NKO_ASSERT(a, m) NKO_ASSERT_DO(a, m, while(1) {})

#define NKO_ASSERT_RET(a, m) NKO_ASSERT_DO(a, m, return)

typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;