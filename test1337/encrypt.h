#pragma once

inline void encrypt(unsigned char* data, size_t size)
{
    for(size_t i = 0; i < size; i++)
        data[i] = ~data[i];
}

inline void decrypt(unsigned char* data, size_t size)
{
    encrypt(data, size);
}