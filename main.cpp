#include <iostream>
#include <fstream>

#define LSB_C 1

template <typename T, typename I>
T arrToOne(I *arr)
{
    T rez = 0;
    for (int i = 0; i < sizeof(T) / sizeof(I); i++)
    {
        rez |= (arr[i] << 8 * i);
    }
    return rez;
}

int main(int argc, char *argv[])
{

    std::string mes = "Hello world";

    std::fstream mesBox("test.bmp", std::ios_base::in | std::ios_base::binary);
    size_t size_file = 0;
    if (mesBox.is_open())
    {
        mesBox.seekg(0, mesBox.end);
        size_file = mesBox.tellg();
    }
    std::fstream Conteiner("test.bmp", std::ios_base::in | std::ios_base::out | std::ios_base::binary);
    if (Conteiner.is_open())
    {
        char *startPointer = new char[4]{0};
        Conteiner.seekg(10, Conteiner.beg);
        Conteiner.read(startPointer, 4);
        int startOffset = arrToOne<int>(startPointer);

        Conteiner.seekg(22, Conteiner.beg);
        Conteiner.read(startPointer, 4);
        int wight = arrToOne<int>(startPointer);

        Conteiner.seekg(18, Conteiner.beg);
        Conteiner.read(startPointer, 4);
        int hight = arrToOne<int>(startPointer);

        // BGR
        int size = hight * wight * 3;

        if (size < (mes.size() + 1) * 8 || size < size_file)
        {
            std::cerr << size << " malo dla " << mes.size() << std::endl;
            return 1;
        }
        // std::cout << size << std::endl;
        for (int i = 0; i < size; i++)
        {
            int pos = startOffset + i + (i / (wight * 3) * (8 - ((wight * 3) % 8)));
            Conteiner.seekg(pos, Conteiner.beg);
            Conteiner.read(startPointer, 1);
            char tmp = 0;
            if ((mes.size() + 1) * 8 >= i)
            {
                tmp = (startPointer[0] & 0xFE) | ((mes[i / 8] >> (i % 8)) & 0x01);
            }

            std::cout << (int)tmp << " ";
            Conteiner.seekp(pos, Conteiner.beg);
            Conteiner.write(&tmp, 1);
            // std::cout << (int)startPointer[0] << " ";
        }
    }
    Conteiner.close();
    return 0;
}