#include <iostream>
#include <fstream>

// #define PRINT_BIT_INSERT

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

template <typename I>
void printBitMap(I val, int countBit = sizeof(I) * 8, int endl = 0)
{
    for (int i = countBit - 1; i >= 0; --i)
    {
        std::cout << ((val >> i) & 1);
        if (i % 8 == 0 && i > 0)
        {
            std::cout << "|";
        }
    }
    std::cout << " ";
    if (endl == 1)
    {
        std::cout << std::endl;
    }
    if (endl == 2)
    {
        std::cout << "\t";
    }
    return;
}

int getBitMask(int bit)
{
    if (bit < 1)
    {
        return 0;
    }
    return getBitMask(bit - 1) | (1 << bit - 1);
}

int InsertBMP(std::string conteinerInput, std::string mesInput, short countBit = 1, std::string nameTmpFile = "tmp")
{
    // unsigned int const countBit = 5;
    unsigned int const maskBit = getBitMask(countBit);

    std::fstream Conteiner(conteinerInput, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
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
            std::ofstream MyFile(nameTmpFile);
            if (!MyFile.is_open())
            {
                std::cerr << "need create tmp file" << std::endl;
                Conteiner.close();
                return 1;
            }
            MyFile << mesInput;
            MyFile.close();
            mesBox.open(nameTmpFile, std::ios_base::in | std::ios_base::binary);
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
                std::remove(nameTmpFile.data());
                Conteiner.close();
                return 1;
            }
        }
        char mesBuf[2] = {0};
        // std::cout << size << std::endl;
        for (int i = 0; i < sizeConteiner; i++)
        {

            int pos = startOffset + i + (i / (wight * 3) * (8 - ((wight * 3) % 8)));
            Conteiner.seekg(pos, Conteiner.beg);
            Conteiner.read(conteinerBuf, 1);
            char tmp = 0;
            if (i <= (sizeMesseg + 1) * 8)
            {
                if (i % (8 / countBit + 1) == 0)
                {
                    mesBox.read(mesBuf, 2);
                    mesBox.seekg(-1, mesBox.cur);
                }
                // i 0 1 2        | 3 4 5      | 6 7 | 8 9
                //   0 3 6{2 + 1} | 1 4 7{1+2} | 2 5 | 0 3
                // i 0 1        | 2 3      | 4      | 5 6      | 7 | 8 9
                //   0 5{3 + 2} | 2 7{1+4} | 4{4+1} | 1 6{2+3} | 3 | 0
                tmp = (conteinerBuf[0] & ~maskBit) | ((arrToOne<short>(mesBuf) >> ((i * countBit) % 8)) & maskBit);
#ifdef PRINT_BIT_INSERT
                printBitMap((arrToOne<short>(mesBuf) >> ((i * countBit) % 8)) & maskBit, countBit, 2);
#endif
            }
            else
            {
                break; // Mojno dobivat ostavsheesa prostranstvo
                // tmp = (conteinerBuf[0] & 0xFE) | (0xAA >> (i % 8) & 0x01);
            }
            // printf("%02x ", static_cast<unsigned char>(tmp));
            // std::cout << std::hex << static_cast<unsigned short>(static_cast<unsigned char>(tmp)) << "\t";
            Conteiner.seekp(pos, Conteiner.beg);
            Conteiner.write(&tmp, 1);
            // std::cout << (int)conteinerBuf[0] << " ";
        }
        Conteiner.close();
        mesBox.close();
        std::remove(nameTmpFile.data()); // СНАЧАЛО ЗАКРЫТЬ ПОТОМ УДАЛИТЬ АУТ
    }
    else
    {
        std::cerr << " Conteiner don't open" << std::endl;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    InsertBMP("test.bmp", "Hello world", 3);
    return 0;
}