#include "../engine.h"

U32 RANDOM_STATE = 1804289383;

U32 get_random_U32_number()
{
    U32 number = RANDOM_STATE;
    
    number ^= number << 13;
    number ^= number >> 17;
    number ^= number << 5;
    
    RANDOM_STATE = number;
    
    return number;
}

U64 get_random_U64_number()
{
    U64 n1, n2, n3, n4;
    
    n1 = (U64) get_random_U32_number() & 0xFFFF;
    n2 = (U64) get_random_U32_number() & 0xFFFF;
    n3 = (U64) get_random_U32_number() & 0xFFFF;
    n4 = (U64) get_random_U32_number() & 0xFFFF;
    
    return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}

U64 generate_magic_number()
{
    return get_random_U64_number() & get_random_U64_number() & get_random_U64_number();
}
