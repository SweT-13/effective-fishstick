#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

// #define PRINT_BIT_INSERT
template <typename T, typename I>
T arrToOne(I *arr)
{
    T rez = 0;
    for (int i = 0; i < sizeof(T) / sizeof(I); i++)
    {
        rez |= ((arr[i] & 0xFF) << 8 * i);
    }
    return rez;
}

class img
{
private:
    std::string nameConteiner;
    std::fstream Conteiner;
    unsigned int startOffset = 0;
    unsigned int hight = 0;
    unsigned int wight = 0;
    unsigned long long sizeConteiner = 0;

public:
    enum RGB
    {
        R = 0,
        G = 1,
        B = 2,
        y = 1,
        cb = 2,
        cr = 3,
    };
    img(std::string name);
    std::vector<std::vector<std::vector<int>>> data;
    std::vector<std::vector<std::vector<double>>> double_data;
    int getSize();
    int getHight();
    int getWight();
    int readData();
    int writeData();
    int printData(unsigned int num = -1u);
    ~img();
    double DCT(const int a, const img::RGB plane, const double k1, const double k2, const unsigned int N1 = 8, const unsigned int N2 = 8);    // N1_def = 8 | N2_def = 8
    double DCT(const double a, const img::RGB plane, const double k1, const double k2, const unsigned int N1 = 8, const unsigned int N2 = 8); // N1_def = 8 | N2_def = 8
    template <typename Q>
    int img2dct(Q chose = 1); // (int)data | (double)double_data
    int RGB2ycbcr();
    long double mean();
    long double dispersion();
};

img::img(std::string name)
{
    nameConteiner = name;
    Conteiner.open(nameConteiner, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
    if (Conteiner.is_open())
    {
        char *conteinerBuf = new char[4]{0};
        Conteiner.seekg(10, Conteiner.beg);
        Conteiner.read(conteinerBuf, 4);
        startOffset = arrToOne<unsigned int>(conteinerBuf);

        Conteiner.seekg(22, Conteiner.beg);
        Conteiner.read(conteinerBuf, 4);
        hight = arrToOne<unsigned int>(conteinerBuf);

        Conteiner.seekg(18, Conteiner.beg);
        Conteiner.read(conteinerBuf, 4);
        wight = arrToOne<unsigned int>(conteinerBuf);

        sizeConteiner = hight * wight * 3;

        data.resize(hight);
        for (int i = 0; i < hight; ++i)
        {
            data[i].resize(wight);
            for (int j = 0; j < wight; ++j)
            {
                data[i][j].resize(3);
            }
        }
        double_data.resize(hight);
        for (int i = 0; i < hight; ++i)
        {
            double_data[i].resize(wight);
            for (int j = 0; j < wight; ++j)
            {
                double_data[i][j].resize(3);
            }
        }

        this->readData();
    }
}

int img::getSize()
{
    return sizeConteiner;
}
int img::getHight()
{
    return hight;
}
int img::getWight()
{
    return wight;
}

int img::readData()
{
    if (Conteiner.is_open())
    {
        char conteinerBuf[3] = {0};
        for (int i = 0; i < hight; i++)
        {
            int endOffset = (((wight * 3) % 8)) * i;
            for (int j = 0; j < wight; j++)
            {
                int pos = startOffset + (i * wight * 3) + (j * 3) + endOffset;
                Conteiner.seekg(pos, Conteiner.beg);
                Conteiner.read(conteinerBuf, 3);
                data[i][j] = {conteinerBuf[0], conteinerBuf[1], conteinerBuf[2]};
            }
        }
        return 1;
    }
    return 0;
}
int img::writeData()
{
    if (Conteiner.is_open())
    {
        char conteinerBuf[3] = {0};
        for (int i = 0; i < hight; i++)
        {
            int endOffset = (((wight * 3) % 8)) * i;
            for (int j = 0; j < wight; j++)
            {
                int pos = startOffset + (i * wight * 3) + (j * 3) + endOffset;
                conteinerBuf[0] = data[i][j][img::RGB::R];
                conteinerBuf[1] = data[i][j][img::RGB::G];
                conteinerBuf[2] = data[i][j][img::RGB::B];
                Conteiner.seekp(pos, Conteiner.beg);
                Conteiner.write(conteinerBuf, 3);
            }
        }
        return 1;
    }
    return 0;
}
int img::printData(unsigned int num) // num def = -1
{
    if (Conteiner.is_open())
    {
        for (int i = 0; i < hight && i < num; i++)
        {
            for (int j = 0; j < wight; j++)
            {
                printf("%02x %02x %02x ", 0xFF & data[i][j][img::RGB::R], 0xFF & data[i][j][img::RGB::G], 0xFF & data[i][j][img::RGB::B]);
            }
            printf("%d\n", i);
        }
    }
    return 0;
}

img::~img()
{
    Conteiner.close();
}

template <typename Q>     // (int)data | (double)double_data
int img::img2dct(Q chose) // def int
{
    for (int i = 0; i < hight; ++i)
    {
        for (int j = 0; j < wight; ++j)
        {
            double_data[i][j][RGB::R] = DCT(chose, RGB::R, i, j);
            double_data[i][j][RGB::G] = DCT(chose, RGB::G, i, j);
            double_data[i][j][RGB::B] = DCT(chose, RGB::B, i, j);
        }
    }
    return 0;
}

double img::DCT(const int a, const img::RGB plane, const double k1, const double k2, const unsigned int N1, const unsigned int N2) // N1_def = 8 | N2_def = 8
{
    double sum1 = 0;
    for (int n1 = 0; n1 < N1; n1++)
    {
        double sum2 = 0;
        for (int n2 = 0; n2 < N2; n2++)
        {
            sum2 += (n1 + k1 < hight && n2 + k2 < wight ? data[n1 + k1][n2 + k2][plane] : 0) * cos((M_PI / N2) * (n2 + 0.5) * k2);
        }
        sum1 += sum2 * cos((M_PI / N1) * (n1 + 0.5) * k1);
    }
    return sum1;
}

double img::DCT(const double a, const img::RGB plane, const double k1, const double k2, const unsigned int N1, const unsigned int N2) // N1_def = 8 | N2_def = 8
{
    double sum1 = 0;
    for (int n1 = 0; n1 < N1; n1++)
    {
        double sum2 = 0;
        for (int n2 = 0; n2 < N2; n2++)
        {
            sum2 += (n1 + k1 < hight && n2 + k2 < wight ? double_data[n1 + k1][n2 + k2][plane] : 0) * cos((M_PI / N2) * (n2 + 0.5) * k2);
        }
        sum1 += sum2 * cos((M_PI / N1) * (n1 + 0.5) * k1);
    }
    return sum1;
}

int img::RGB2ycbcr()
{
    for (int i = 0; i < hight; ++i)
    {
        for (int j = 0; j < wight; ++j)
        {
            double_data[i][j][RGB::y] = (int)(0.299 * data[i][j][RGB::R] + 0.587 * data[i][j][RGB::G] + 0.114 * data[i][j][RGB::B]);
            double_data[i][j][RGB::cb] = (int)(-0.16874 * data[i][j][RGB::R] - 0.33126 * data[i][j][RGB::G] + 0.50000 * data[i][j][RGB::B]);
            double_data[i][j][RGB::cr] = (int)(0.50000 * data[i][j][RGB::R] - 0.41869 * data[i][j][RGB::G] - 0.08131 * data[i][j][RGB::B]);
        }
    }

    return 0;
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

int swapBMP(img &a, img &b, int needPlace = 1)
{
    if (a.getHight() != b.getHight() || a.getWight() != b.getWight() || needPlace > 8)
    {
        return 1;
    }
    int h = a.getHight();
    int w = a.getWight();
    int tmp = 0;
    for (int i = 0; i < h; ++i)
    {
        for (int j = 0; j < w; ++j)
        {
            for (int place = 0; place < 3; place++)
            {
                tmp = a.data[i][j][place];
                a.data[i][j][place] = (a.data[i][j][place] & ~getBitMask(needPlace)) | (b.data[i][j][place] & getBitMask(needPlace));
                b.data[i][j][place] = (b.data[i][j][place] & ~getBitMask(needPlace)) | (tmp & getBitMask(needPlace));
            }
        }
    }
    return 0;
}

long double img::mean()
{
    long double sum = 0;
    for (int i = 0; i < hight; ++i)
    {
        for (int j = 0; j < wight; ++j)
        {
            for (int place = 0; place < 3; place++)
            {
                sum += data[i][j][place];
            }
        }
    }
    return sum / sizeConteiner;
}

long double img::dispersion()
{
    long double sum = 0;
    long double mean = this->mean();
    for (int i = 0; i < hight; ++i)
    {
        for (int j = 0; j < wight; ++j)
        {
            for (int place = 0; place < 3; place++)
            {
                sum += (data[i][j][place] - mean) * (data[i][j][place] - mean);
            }
        }
    }
    return sum / sizeConteiner;
}

double ssimBMP(img &a, img &b)
{
    if (a.getHight() != b.getHight() || a.getWight() != b.getWight())
    {
        return -2;
    }
    // оптимизация по памяти плачет
    const double k1 = 0.01;
    const double k2 = 0.03;
    double m_x = a.mean();
    double m_y = b.mean();
    double c1 = k1 * getBitMask(8 * 3) * k1 * getBitMask(8 * 3);
    double c2 = k2 * getBitMask(8 * 3) * k2 * getBitMask(8 * 3);
    double o2_x = a.dispersion();
    double o2_y = b.dispersion();
    double o_xy = m_x * m_y;

    double ssim = ((2 * m_x * m_y + c1) * (2 * o_xy + c2)) / ((m_x * m_x + m_y * m_y + c1) * (o2_x + o2_y + c2));

    return ssim;
}

long double psnrBMP(img &a, img &b)
{
    if (a.getHight() != b.getHight() || a.getWight() != b.getWight())
    {
        return 1;
    }
    int h = a.getHight();
    int w = a.getWight();

    int MAX = getBitMask(8 * 3);
    long double MSE = 0;
    long double sum = 0.0;
    for (int i = 0; i < h; ++i)
    {
        for (int j = 0; j < w; ++j)
        {
            for (int place = 0; place < 3; place++)
            {
                sum += (a.data[i][j][place] - b.data[i][j][place]) * (a.data[i][j][place] - b.data[i][j][place]);
            }
        }
    }
    MSE = sum / (h * w * 3);
    // if (MSE == 0)
    // {
    //     return -1;
    // }
    return 20 * log10l(MAX / sqrtl(MSE));
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
        unsigned long long sizeConteiner = hight * wight * 3;

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
                return 2;
            }
            MyFile << mesInput;
            MyFile.close();
            mesBox.open(nameTmpFile, std::ios_base::in | std::ios_base::binary);
            if (!mesBox.is_open())
            {
                std::cerr << "need FILE" << std::endl;
                Conteiner.close();
                return 3;
            }
        }

        if (sizeConteiner * countBit < (sizeMesseg + 1) * 8)
        {
            std::cerr << sizeConteiner << " malo dla " << sizeMesseg << std::endl;
            std::cout << "Fill in as much as possible? " << std::endl;
            std::string ans = "n";
            std::cin >> ans;
            if (ans != "y")
            {
                std::remove(nameTmpFile.data());
                Conteiner.close();
                return 4;
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
            if (i <= (sizeMesseg + 1) * 8 / countBit)
            {
                if (i % (8 / countBit + (8 % countBit ? 1 : 0)) == 0)
                {
                    mesBox.read(mesBuf, 2);
                    mesBox.seekg(-1, mesBox.cur);
                }
                // i 0 1 2        | 3 4 5      | 6 7 | 8 9
                //   0 3 6{2 + 1} | 1 4 7{1+2} | 2 5 | 0 3
                // i 0 1        | 2 3      | 4      | 5 6      | 7 | 8 9
                //   0 5{3 + 2} | 2 7{1+4} | 4{4+1} | 1 6{2+3} | 3 | 0
                tmp = (char)(conteinerBuf[0] & ~maskBit) | ((arrToOne<short>(mesBuf) >> ((i * countBit) % 8)) & maskBit);
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
    img first("MyFace.bmp");
    img second("MyFace8.bmp");

    // first.printData(2);
    printf("%f\n", ssimBMP(first, first));
    printf("%f\n", ssimBMP(first, second));

    // std::cout << psnrBMP(first, first) << std::endl;
    // std::cout << psnrBMP(first, second) << std::endl;

    // first.img2dct<int>();

    // first.printData(1);
    // swapBMP(first, second, 8);
    // first.printData(1);

    return 0;

    InsertBMP("MyFace1.bmp", "test.txt", 1);
    InsertBMP("MyFace2.bmp", "test.txt", 2);
    InsertBMP("MyFace3.bmp", "test.txt", 3);
    InsertBMP("MyFace4.bmp", "test.txt", 4);
    InsertBMP("MyFace5.bmp", "test.txt", 5);
    InsertBMP("MyFace6.bmp", "test.txt", 6);
    InsertBMP("MyFace7.bmp", "test.txt", 7);
    InsertBMP("MyFace8.bmp", "test.txt", 8);
    return 0;
}

// int test()
// {
//     std::fstream Conteiner("test.bmp", std::ios_base::in | std::ios_base::out | std::ios_base::binary);
//     if (Conteiner.is_open())
//     {
//         char *conteinerBuf = new char[4]{0};
//         Conteiner.seekg(10, Conteiner.beg);
//         Conteiner.read(conteinerBuf, 4);
//         int startOffset = arrToOne<int>(conteinerBuf);

//         Conteiner.seekg(22, Conteiner.beg);
//         Conteiner.read(conteinerBuf, 4);
//         int wight = arrToOne<int>(conteinerBuf);

//         Conteiner.seekg(18, Conteiner.beg);
//         Conteiner.read(conteinerBuf, 4);
//         int hight = arrToOne<int>(conteinerBuf);

//         // BGR
//         int sizeConteiner = hight * wight * 3;

//         std::vector<pix> arr;

//         for (int i = 0; i < sizeConteiner; i++)
//         {

//             int pos = startOffset + i + (i / (wight * 3) * (8 - ((wight * 3) % 8)));
//             Conteiner.seekg(pos, Conteiner.beg);
//             Conteiner.read(conteinerBuf, 3);
//             char tmp = 0;
//             arr.push_back({((conteinerBuf[2] * 0.257) + (conteinerBuf[1] * 0.504) + (conteinerBuf[0] * 0.098)) / 6,
//                            ((conteinerBuf[2] * -0.148) + (conteinerBuf[1] * 0.291) + (conteinerBuf[0] * 0.439)) / 128,
//                            ((conteinerBuf[2] * 0.439) + (conteinerBuf[1] * 0.368) + (conteinerBuf[0] * 0.071)) / 128});
//         }
//         Conteiner.close();
//     }
//     return 0;
// }