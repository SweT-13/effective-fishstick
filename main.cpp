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

    // std::string mesInput = "Hello world";
    std::string mesInput = "main.cpp";

    std::fstream Conteiner("test.bmp", std::ios_base::in | std::ios_base::out | std::ios_base::binary);
    if (Conteiner.is_open())
    {
        char *conteinerBuf = new char[4]{0};
        Conteiner.seekg(10, Conteiner.beg);
        Conteiner.read(conteinerBuf, 4);
        int startOffset = arrToOne<int>(conteinerBuf);

        Conteiner.seekg(22, Conteiner.beg);
        Conteiner.read(conteinerBuf, 4);
        int wight = arrToOne<int>(conteinerBuf);

        Conteiner.seekg(18, Conteiner.beg);
        Conteiner.read(conteinerBuf, 4);
        int hight = arrToOne<int>(conteinerBuf);

        // BGR
        int sizeConteiner = hight * wight * 3;

        std::fstream mesBox;
        mesBox.open(mesInput, std::ios_base::in | std::ios_base::binary);
        size_t sizeMesseg = 0;
        if (mesBox.is_open())
        {
            mesBox.seekg(0, mesBox.end);
            sizeMesseg = mesBox.tellg();
            mesBox.seekg(0, mesBox.beg);
        }
        else
        {
            sizeMesseg = mesInput.size();
            std::ofstream MyFile("tmp");
            if (!MyFile.is_open())
            {
                std::cerr << "need create tmp file" << std::endl;
                Conteiner.close();
                return 1;
            }
            MyFile << mesInput;
            MyFile.close();
            mesBox.open("tmp", std::ios_base::in | std::ios_base::binary);
            if (!mesBox.is_open())
            {
                std::cerr << "need FILE" << std::endl;
                Conteiner.close();
                return 1;
            }
        }

        if (sizeConteiner < (sizeMesseg + 1) * 8)
        {
            std::cerr << sizeConteiner << " malo dla " << sizeMesseg << std::endl;
            std::cout << "Fill in as much as possible? " << std::endl;
            std::string ans = "n";
            std::cin >> ans;
            if (ans != "y")
            {
                Conteiner.close();
                return 1;
            }
        }
        char mesBuf = 0;
        // std::cout << size << std::endl;
        for (int i = 0; i < sizeConteiner; i++)
        {

            int pos = startOffset + i + (i / (wight * 3) * (8 - ((wight * 3) % 8)));
            Conteiner.seekg(pos, Conteiner.beg);
            Conteiner.read(conteinerBuf, 1);
            char tmp = 0;
            if (i <= (sizeMesseg + 1) * 8)
            {
                if (i % 8 == 0)
                {
                    mesBox.read(&mesBuf, 1);
                }
                tmp = (conteinerBuf[0] & 0xFE) | (mesBuf >> (i % 8) & 0x01);
            }
            else
            {
                break;
                // tmp = (conteinerBuf[0] & 0xFE) | (0xAA >> (i % 8) & 0x01);
            }
            std::cout << (int)tmp << " ";
            Conteiner.seekp(pos, Conteiner.beg);
            Conteiner.write(&tmp, 1);
            // std::cout << (int)conteinerBuf[0] << " ";
        }
        Conteiner.close();
    }
    else
    {
        std::cerr << " Conteiner don't open" << std::endl;
    }
    return 0;
}