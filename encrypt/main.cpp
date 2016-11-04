#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "../test1337/encrypt.h"

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        puts("usage: encrypt SCRIPT.bin");
        return 0;
    }

    FILE* fileptr;
    fopen_s(&fileptr, argv[1], "rb");
    fseek(fileptr, 0, SEEK_END);
    auto filelen = ftell(fileptr);
    rewind(fileptr);

    auto buffer = (unsigned char*)malloc((filelen + 1) * sizeof(char));
    fread(buffer, filelen, 1, fileptr);
    fclose(fileptr);

    encrypt(buffer, filelen);

    auto encname = std::string(argv[1]) + ".enc";
    fopen_s(&fileptr, encname.c_str(), "wb");
    fwrite(buffer, 1, filelen, fileptr);
    fclose(fileptr);

    free(buffer);

    return 0;
}