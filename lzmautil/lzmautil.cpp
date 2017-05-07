#include <fstream>
#include <iostream>
#include <cstdlib>
#include "LzmaEnc.h"
#include "LzmaLib.h"


unsigned char* compress(unsigned char* src, size_t datalen, size_t* complen)
{
    unsigned char* dest = new unsigned char[datalen*2];
    size_t destlen = datalen*2;

    unsigned char* odest = dest;
    *((size_t*)odest) = datalen;
    size_t propssize = LZMA_PROPS_SIZE;

    dest += 4;
    CLzmaEncProps props;


    LzmaCompress(dest+LZMA_PROPS_SIZE, &destlen, src, datalen,
    dest, &propssize, 9,1<<12,3,0,2,32,1);

    *complen = destlen+4+5;
    return odest;
}

unsigned char* decompress(unsigned char* src, size_t datalen, size_t* complen)
{

    src += 4;

    size_t destlen = *((size_t*)src);
    size_t propssize = LZMA_PROPS_SIZE;

    unsigned char* dest = new unsigned char[destlen];


    size_t inlen = datalen-8-propssize;

    src += 4;

    LzmaUncompress(dest, &destlen,src+LZMA_PROPS_SIZE,&inlen,src, propssize);
    *complen = destlen;
    return dest;
}

int main(int argc, char** argv)
{
    auto usage = "usage: lzma [-d] infile outfile";

    if (argc < 3)
        std::cout << usage << std::endl;

    bool decomp = false;
    int paramOffset = 1;
    std::string a = argv[paramOffset];
    if (a == "-d")
    {
        decomp =true;
        paramOffset ++;
        if ((argc - paramOffset) < 2)
            std::cout << usage;
    }

    std::ifstream file;
    file.open(argv[paramOffset], std::ifstream::in | std::ifstream::binary);
    paramOffset ++;

    size_t len = 0;
    if (file.is_open())
    {
        file.seekg(0, file.end);
        len = file.tellg();
        file.seekg(0, file.beg);
    }
    else
        return 1;

    char* data = new char[len];
    if (data)
        file.read(data,len);
    else
        len = 0;

    file.close();

    size_t tk = 0;

    void* comp_data;

    std::cout << "Copmressing " << len << " bytes" << std::endl;
    if (decomp==false)
        comp_data = compress((unsigned char*)data,len,&tk);
    else
        comp_data = decompress((unsigned char*)data,len,&tk);
    std::cout << "Compressed size " << tk << " bytes" << std::endl;
    delete[] data;


    std::ofstream ofile;
    ofile.open(argv[paramOffset], std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
    std::cout << "Writing " << tk << " bytes to file " << argv[paramOffset] << std::endl;
    if (decomp==false)
        ofile.write((char*)(&tk),4);
    ofile.write((char*)comp_data,tk);


    return 0;
}
