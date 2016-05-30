/*********************************
*   Valentin Latunov 2013        *
*   TGA Reader                   *
*   Adapted to work with OpenGL. *
*********************************/
#ifndef GL
#define GL
#include <gl/gl.h>
#endif

#ifndef TGA_H
#define TGA_H
#include <fstream>
using namespace std;
unsigned short get_short(fstream& file)
{
    unsigned char a, b;
    a = file.get();
    b = file.get();
    return ((unsigned int)a + ((unsigned int)b << 8));
}
unsigned int get_int(fstream& file)
{
    unsigned char a, b, c, d;
    a = file.get();
    b = file.get();
    c = file.get();
    d = file.get();
    return ((unsigned int)a + ((unsigned int)b << 8) + ((unsigned int)c << 16) + ((unsigned int)d << 24));
}
struct TGA
{
    unsigned int id;
    unsigned char* data;
    unsigned short width;
    unsigned short height;
    unsigned int size;
    bool alpha;
    TGA();
    ~TGA();
    bool Load(char*);
    bool Free();
};
TGA::TGA()
{
    data = NULL;
    id = 0;
}
TGA::~TGA(){}
bool TGA::Load(char* file_path)
{
    fstream file;
    file.open(file_path, ios::in | ios::binary);
    if(!file.is_open())
    {
        return false;
    }
    file.seekg(12); // width
    width = get_short(file);
    file.seekg(14); // heigth
    height = get_short(file);
    file.seekg(16);
    if((unsigned int)file.get() == 32) // bits per pixel
    {
        alpha = true;
    }
    else
    {
        alpha = false;
    }
    size = alpha ? (width * height * 4) : (width * height * 3);
    data = new unsigned char [size];
    file.seekg(18);
    file.read((char*)data, size);
    if(!file.good() || !width || !height)
    {
        file.clear();
        file.close();
        return false;
    }
    file.clear();
    file.close();
    unsigned char temp;
    int step = alpha ? 4 : 3;
    for(int i = 0; i < size; i += step)
    {
        temp = data[i];
        data[i] = data[i + 2];
        data[i + 2] = temp;
    }
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    //glPixelStorei(GL_PACK_ROW_LENGTH, 4);
	//glPixelStorei(GL_PACK_ALIGNMENT, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_FALSE);
    //glPixelStorei(GL_UNPACK_LSB_FIRST,  GL_FALSE);
    if(alpha)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    delete data;
	if(!id)
    {
        return false;
    }
    return true;
}
bool TGA::Free()
{
    glDeleteTextures(1, &id);
    return true;
}
#endif
#ifndef GL
#define GL
#include <gl/gl.h>
#endif
#ifndef FONT_H
#define FONT_H
TGA font;
bool CAPS = false;
bool ENG = true;
struct symbol
{
    float top, bottom, left, right;
    symbol();
    symbol(float, float, float, float);
};
symbol::symbol(){}
symbol::symbol(float a, float b, float c, float d)
{
    left = a;
    right = b;
    top = c;
    bottom = d;
}
symbol SYM[] = // 155
{
    symbol(15.0/1024.0, 48.0/1024.0, 0.0/1024.0, 64.0/1024.0), // a
    symbol(81.0/1024.0, 115.0/1024.0, 0.0/1024.0, 64.0/1024.0), // b
    symbol(146.0/1024.0, 177.0/1024.0, 0.0/1024.0, 64.0/1024.0), // c
    symbol(206.0/1024.0, 240.0/1024.0, 0.0/1024.0, 64.0/1024.0), // d
    symbol(271.0/1024.0, 306.0/1024.0, 0.0/1024.0, 64.0/1024.0), // e
    symbol(342.0/1024.0, 367.0/1024.0, 0.0/1024.0, 64.0/1024.0), // f
    symbol(398.0/1024.0, 432.0/1024.0, 0.0/1024.0, 64.0/1024.0), // g
    symbol(465.0/1024.0, 497.0/1024.0, 0.0/1024.0, 64.0/1024.0), // h
    symbol(540.0/1024.0, 550.0/1024.0, 0.0/1024.0, 64.0/1024.0), // i
    symbol(594.0/1024.0, 616.0/1024.0, 0.0/1024.0, 64.0/1024.0), // j
    symbol(658.0/1024.0, 693.0/1024.0, 0.0/1024.0, 64.0/1024.0), // k
    symbol(732.0/1024.0, 741.0/1024.0, 0.0/1024.0, 64.0/1024.0), // l
    symbol(774.0/1024.0, 828.0/1024.0, 0.0/1024.0, 64.0/1024.0), // m
    symbol(848.0/1024.0, 881.0/1024.0, 0.0/1024.0, 64.0/1024.0), // n
    symbol(911.0/1024.0, 947.0/1024.0, 0.0/1024.0, 64.0/1024.0), // o
    symbol(977.0/1024.0, 1011.0/1024.0, 0.0/1024.0, 64.0/1024.0), // p
    symbol(15.0/1024.0, 48.0/1024.0, 64.0/1024.0, 128.0/1024.0), // q
    symbol(87.0/1024.0, 112.0/1024.0, 64.0/1024.0, 128.0/1024.0), // r
    symbol(146.0/1024.0, 176.0/1024.0, 64.0/1024.0, 128.0/1024.0), // s
    symbol(212.0/1024.0, 237.0/1024.0, 64.0/1024.0, 128.0/1024.0), // t
    symbol(272.0/1024.0, 305.0/1024.0, 64.0/1024.0, 128.0/1024.0), // u
    symbol(334.0/1024.0, 371.0/1024.0, 64.0/1024.0, 128.0/1024.0), // v
    symbol(392.0/1024.0, 441.0/1024.0, 64.0/1024.0, 128.0/1024.0), // w
    symbol(462.0/1024.0, 500.0/1024.0, 64.0/1024.0, 128.0/1024.0), // x
    symbol(526.0/1024.0, 564.0/1024.0, 64.0/1024.0, 128.0/1024.0), // y
    symbol(593.0/1024.0, 625.0/1024.0, 64.0/1024.0, 128.0/1024.0), // z
    symbol(650.0/1024.0, 695.0/1024.0, 64.0/1024.0, 128.0/1024.0), // A
    symbol(719.0/1024.0, 758.0/1024.0, 64.0/1024.0, 128.0/1024.0), // B
    symbol(780.0/1024.0, 823.0/1024.0, 64.0/1024.0, 128.0/1024.0), // C
    symbol(845.0/1024.0, 887.0/1024.0, 64.0/1024.0, 128.0/1024.0), // D
    symbol(913.0/1024.0, 947.0/1024.0, 64.0/1024.0, 128.0/1024.0), // E
    symbol(979.0/1024.0, 1011.0/1024.0, 64.0/1024.0, 128.0/1024.0), // F
    symbol(10.0/1024.0, 54.0/1024.0, 128.0/1024.0, 192.0/1024.0), // G
    symbol(77.0/1024.0, 116.0/1024.0, 128.0/1024.0, 192.0/1024.0), // H
    symbol(150.0/1024.0, 171.0/1024.0, 128.0/1024.0, 192.0/1024.0), // I
    symbol(210.0/1024.0, 235.0/1024.0, 128.0/1024.0, 192.0/1024.0), // J
    symbol(271.0/1024.0, 311.0/1024.0, 128.0/1024.0, 192.0/1024.0), // K
    symbol(340.0/1024.0, 372.0/1024.0, 128.0/1024.0, 192.0/1024.0), // L
    symbol(394.0/1024.0, 439.0/1024.0, 128.0/1024.0, 192.0/1024.0), // M
    symbol(461.0/1024.0, 500.0/1024.0, 128.0/1024.0, 192.0/1024.0), // N
    symbol(521.0/1024.0, 569.0/1024.0, 128.0/1024.0, 192.0/1024.0), // O
    symbol(594.0/1024.0, 627.0/1024.0, 128.0/1024.0, 192.0/1024.0), // P
    symbol(649.0/1024.0, 697.0/1024.0, 128.0/1024.0, 192.0/1024.0), // Q
    symbol(719.0/1024.0, 760.0/1024.0, 128.0/1024.0, 192.0/1024.0), // R
    symbol(782.0/1024.0, 821.0/1024.0, 128.0/1024.0, 192.0/1024.0), // S
    symbol(843.0/1024.0, 886.0/1024.0, 128.0/1024.0, 192.0/1024.0), // T
    symbol(909.0/1024.0, 948.0/1024.0, 128.0/1024.0, 188.0/1024.0), // U
    symbol(970.0/1024.0, 1016.0/1024.0, 128.0/1024.0, 192.0/1024.0), // V
    symbol(3.0/1024.0, 62.0/1024.0, 192.0/1024.0, 256.0/1024.0), // W
    symbol(76.0/1024.0, 118.0/1024.0, 192.0/1024.0, 256.0/1024.0), // X
    symbol(140.0/1024.0, 182.0/1024.0, 192.0/1024.0, 254.0/1024.0), // Y
    symbol(205.0/1024.0, 244.0/1024.0, 192.0/1024.0, 256.0/1024.0), // Z
    symbol(271.0/1024.0, 306.0/1024.0, 192.0/1024.0, 256.0/1024.0), // 0
    symbol(340.0/1024.0, 368.0/1024.0, 192.0/1024.0, 256.0/1024.0), // 1
    symbol(400.0/1024.0, 434.0/1024.0, 192.0/1024.0, 254.0/1024.0), // 2
    symbol(464.0/1024.0, 498.0/1024.0, 192.0/1024.0, 254.0/1024.0), // 3
    symbol(525.0/1024.0, 564.0/1024.0, 192.0/1024.0, 254.0/1024.0), // 4
    symbol(593.0/1024.0, 627.0/1024.0, 192.0/1024.0, 256.0/1024.0), // 5
    symbol(655.0/1024.0, 692.0/1024.0, 192.0/1024.0, 256.0/1024.0), // 6
    symbol(720.0/1024.0, 754.0/1024.0, 192.0/1024.0, 256.0/1024.0), // 7
    symbol(783.0/1024.0, 819.0/1024.0, 192.0/1024.0, 256.0/1024.0), // 8
    symbol(846.0/1024.0, 882.0/1024.0, 192.0/1024.0, 254.0/1024.0), // 9
    symbol(919.0/1024.0, 935.0/1024.0, 188.0/1024.0, 254.0/1024.0), // `
    symbol(988.0/1024.0, 999.0/1024.0, 192.0/1024.0, 254.0/1024.0), // !
    symbol(5.0/1024.0, 61.0/1024.0, 256.0/1024.0, 318.0/1024.0), // @
    symbol(75.0/1024.0, 119.0/1024.0, 256.0/1024.0, 318.0/1024.0), // #
    symbol(144.0/1024.0, 178.0/1024.0, 256.0/1024.0, 318.0/1024.0), // $
    symbol(193.0/1024.0, 256.0/1024.0, 256.0/1024.0, 318.0/1024.0), // %
    symbol(267.0/1024.0, 311.0/1024.0, 256.0/1024.0, 318.0/1024.0), // ^
    symbol(331.0/1024.0, 379.0/1024.0, 256.0/1024.0, 320.0/1024.0), // &
    symbol(400.0/1024.0, 434.0/1024.0, 256.0/1024.0, 320.0/1024.0), // *
    symbol(470.0/1024.0, 494.0/1024.0, 256.0/1024.0, 318.0/1024.0), // (
    symbol(532.0/1024.0, 556.0/1024.0, 256.0/1024.0, 320.0/1024.0), // )
    symbol(586.0/1024.0, 631.0/1024.0, 256.0/1024.0, 320.0/1024.0), // _
    symbol(651.0/1024.0, 694.0/1024.0, 256.0/1024.0, 320.0/1024.0), // +
    symbol(724.0/1024.0, 749.0/1024.0, 256.0/1024.0, 320.0/1024.0), // -
    symbol(780.0/1024.0, 821.0/1024.0, 256.0/1024.0, 320.0/1024.0), // =
    symbol(856.0/1024.0, 875.0/1024.0, 256.0/1024.0, 320.0/1024.0), // [
    symbol(918.0/1024.0, 938.0/1024.0, 256.0/1024.0, 318.0/1024.0), // ]
    symbol(977.0/1024.0, 1010.0/1024.0, 256.0/1024.0, 320.0/1024.0), // {
    symbol(17.0/1024.0, 50.0/1024.0, 320.0/1024.0, 384.0/1024.0), // }
    symbol(83.0/1024.0, 114.0/1024.0, 320.0/1024.0, 384.0/1024.0), /* \ */
    symbol(156.0/1024.0, 166.0/1024.0, 320.0/1024.0, 384.0/1024.0), // |
    symbol(219.0/1024.0, 236.0/1024.0, 320.0/1024.0, 384.0/1024.0), // '
    symbol(277.0/1024.0, 305.0/1024.0, 320.0/1024.0, 384.0/1024.0), // "
    symbol(343.0/1024.0, 361.0/1024.0, 320.0/1024.0, 384.0/1024.0), // ;
    symbol(411.0/1024.0, 423.0/1024.0, 320.0/1024.0, 382.0/1024.0), // :
    symbol(464.0/1024.0, 494.0/1024.0, 320.0/1024.0, 384.0/1024.0), // /
    symbol(531.0/1024.0, 560.0/1024.0, 320.0/1024.0, 384.0/1024.0), // ?
    symbol(603.0/1024.0, 615.0/1024.0, 320.0/1024.0, 384.0/1024.0), // .
    symbol(664.0/1024.0, 681.0/1024.0, 320.0/1024.0, 384.0/1024.0), // ,
    symbol(717.0/1024.0, 756.0/1024.0, 320.0/1024.0, 384.0/1024.0), // <
    symbol(781.0/1024.0, 820.0/1024.0, 320.0/1024.0, 384.0/1024.0), // >
    symbol(847.0/1024.0, 880.0/1024.0, 320.0/1024.0, 384.0/1024.0), // а
    symbol(911.0/1024.0, 946.0/1024.0, 320.0/1024.0, 384.0/1024.0), // б
    symbol(978.0/1024.0, 1010.0/1024.0, 320.0/1024.0, 384.0/1024.0), // в
    symbol(22.0/1024.0, 49.0/1024.0, 384.0/1024.0, 448.0/1024.0), // г
    symbol(75.0/1024.0, 116.0/1024.0, 384.0/1024.0, 448.0/1024.0), // д
    symbol(143.0/1024.0, 178.0/1024.0, 384.0/1024.0, 448.0/1024.0), // е
    symbol(199.0/1024.0, 250.0/1024.0, 384.0/1024.0, 448.0/1024.0), // ж
    symbol(272.0/1024.0, 304.0/1024.0, 384.0/1024.0, 448.0/1024.0), // з
    symbol(336.0/1024.0, 369.0/1024.0, 384.0/1024.0, 448.0/1024.0), // и
    symbol(400.0/1024.0, 433.0/1024.0, 384.0/1024.0, 448.0/1024.0), // й
    symbol(466.0/1024.0, 500.0/1024.0, 384.0/1024.0, 448.0/1024.0), // к
    symbol(524.0/1024.0, 561.0/1024.0, 384.0/1024.0, 448.0/1024.0), // л
    symbol(591.0/1024.0, 627.0/1024.0, 384.0/1024.0, 448.0/1024.0), // м
    symbol(656.0/1024.0, 689.0/1024.0, 384.0/1024.0, 448.0/1024.0), // н
    symbol(719.0/1024.0, 754.0/1024.0, 384.0/1024.0, 448.0/1024.0), // о
    symbol(784.0/1024.0, 816.0/1024.0, 384.0/1024.0, 448.0/1024.0), // п
    symbol(849.0/1024.0, 883.0/1024.0, 384.0/1024.0, 448.0/1024.0), // р
    symbol(913.0/1024.0, 946.0/1024.0, 384.0/1024.0, 448.0/1024.0), // с
    symbol(976.0/1024.0, 1010.0/1024.0, 384.0/1024.0, 448.0/1024.0), // т
    symbol(14.0/1024.0, 51.0/1024.0, 448.0/1024.0, 512.0/1024.0), // у
    symbol(71.0/1024.0, 122.0/1024.0, 448.0/1024.0, 512.0/1024.0), // ф
    symbol(142.0/1024.0, 179.0/1024.0, 448.0/1024.0, 512.0/1024.0), // х
    symbol(208.0/1024.0, 240.0/1024.0, 448.0/1024.0, 498.0/1024.0), // ч
    symbol(272.0/1024.0, 309.0/1024.0, 448.0/1024.0, 512.0/1024.0), // ц
    symbol(329.0/1024.0, 376.0/1024.0, 448.0/1024.0, 512.0/1024.0), // ш
    symbol(392.0/1024.0, 444.0/1024.0, 448.0/1024.0, 512.0/1024.0), // щ
    symbol(459.0/1024.0, 500.0/1024.0, 448.0/1024.0, 512.0/1024.0), // ъ
    symbol(531.0/1024.0, 561.0/1024.0, 448.0/1024.0, 512.0/1024.0), // ь
    symbol(586.0/1024.0, 634.0/1024.0, 448.0/1024.0, 512.0/1024.0), // ю
    symbol(653.0/1024.0, 688.0/1024.0, 448.0/1024.0, 512.0/1024.0), // я
    symbol(713.0/1024.0, 760.0/1024.0, 448.0/1024.0, 512.0/1024.0), // А
    symbol(783.0/1024.0, 822.0/1024.0, 448.0/1024.0, 512.0/1024.0), // Б
    symbol(847.0/1024.0, 886.0/1024.0, 448.0/1024.0, 512.0/1024.0), // В
    symbol(915.0/1024.0, 948.0/1024.0, 448.0/1024.0, 512.0/1024.0), // Г
    symbol(968.0/1024.0, 1015.0/1024.0, 448.0/1024.0, 512.0/1024.0), // Д
    symbol(17.0/1024.0, 51.0/1024.0, 512.0/1024.0, 576.0/1024.0), // Е
    symbol(67.0/1024.0, 128.0/1024.0, 512.0/1024.0, 576.0/1024.0), // Ж
    symbol(142.0/1024.0, 178.0/1024.0, 512.0/1024.0, 576.0/1024.0), // З
    symbol(205.0/1024.0, 244.0/1024.0, 500.0/1024.0, 576.0/1024.0), // Й
    symbol(269.0/1024.0, 308.0/1024.0, 512.0/1024.0, 576.0/1024.0), // И
    symbol(335.0/1024.0, 375.0/1024.0, 512.0/1024.0, 576.0/1024.0), // К
    symbol(392.0/1024.0, 435.0/1024.0, 512.0/1024.0, 576.0/1024.0), // Л
    symbol(458.0/1024.0, 503.0/1024.0, 512.0/1024.0, 576.0/1024.0), // М
    symbol(525.0/1024.0, 564.0/1024.0, 512.0/1024.0, 576.0/1024.0), // Н
    symbol(585.0/1024.0, 632.0/1024.0, 512.0/1024.0, 572.0/1024.0), // О
    symbol(653.0/1024.0, 692.0/1024.0, 512.0/1024.0, 576.0/1024.0), // П
    symbol(722.0/1024.0, 755.0/1024.0, 512.0/1024.0, 576.0/1024.0), // Р
    symbol(780.0/1024.0, 823.0/1024.0, 512.0/1024.0, 576.0/1024.0), // С
    symbol(843.0/1024.0, 886.0/1024.0, 512.0/1024.0, 576.0/1024.0), // Т
    symbol(907.0/1024.0, 950.0/1024.0, 512.0/1024.0, 576.0/1024.0), // У
    symbol(968.0/1024.0, 1017.0/1024.0, 512.0/1024.0, 576.0/1024.0), // Ф
    symbol(12.0/1024.0, 54.0/1024.0, 576.0/1024.0, 640.0/1024.0), // Х
    symbol(76.0/1024.0, 114.0/1024.0, 576.0/1024.0, 640.0/1024.0), // Ч
    symbol(141.0/1024.0, 184.0/1024.0, 576.0/1024.0, 640.0/1024.0), // Ц
    symbol(195.0/1024.0, 252.0/1024.0, 576.0/1024.0, 640.0/1024.0), // Ш
    symbol(259.0/1024.0, 320.0/1024.0, 576.0/1024.0, 640.0/1024.0), // Щ
    symbol(326.0/1024.0, 377.0/1024.0, 576.0/1024.0, 640.0/1024.0), // Ъ
    symbol(400.0/1024.0, 437.0/1024.0, 576.0/1024.0, 640.0/1024.0), // Ь
    symbol(451.0/1024.0, 511.0/1024.0, 576.0/1024.0, 640.0/1024.0), // Ю
    symbol(521.0/1024.0, 563.0/1024.0, 576.0/1024.0, 640.0/1024.0), // Я
    symbol(592.0/1024.0, 626.0/1024.0, 572.0/1024.0, 640.0/1024.0), // ударено и
    symbol(651.0/1024.0, 694.0/1024.0, 576.0/1024.0, 640.0/1024.0) // ~
};
// [sym][eng][caps][shift]
unsigned int strlen(unsigned char* str)
{
    unsigned int c = 0;
    while(str[c] != 255)
    {
        c++;
    }
    return c;
}
unsigned char keyboard_table[256][2][2][2]; // [sym][eng][caps][shift]
unsigned char ascii_table[256];
void Initialize_Font()
{
    font.Load("font.tga");
    // A
    keyboard_table[65][0][0][0] = 0;
    keyboard_table[65][0][1][0] = 26;
    keyboard_table[65][0][0][1] = 26;
    keyboard_table[65][0][1][1] = 0;
    keyboard_table[65][1][0][0] = 93;
    keyboard_table[65][1][1][0] = 123;
    keyboard_table[65][1][0][1] = 123;
    keyboard_table[65][1][1][1] = 93;
    
    // B
    keyboard_table[66][0][0][0] = 1;
    keyboard_table[66][0][1][0] = 27;
    keyboard_table[66][0][0][1] = 27;
    keyboard_table[66][0][1][1] = 1;
    keyboard_table[66][1][0][0] = 94;
    keyboard_table[66][1][1][0] = 124;
    keyboard_table[66][1][0][1] = 124;
    keyboard_table[66][1][1][1] = 94;
    
    // C
    keyboard_table[67][0][0][0] = 2;
    keyboard_table[67][0][1][0] = 28;
    keyboard_table[67][0][0][1] = 28;
    keyboard_table[67][0][1][1] = 2;
    keyboard_table[67][1][0][0] = 116;
    keyboard_table[67][1][1][0] = 146;
    keyboard_table[67][1][0][1] = 146;
    keyboard_table[67][1][1][1] = 116;
    
    // D
    keyboard_table[68][0][0][0] = 3;
    keyboard_table[68][0][1][0] = 29;
    keyboard_table[68][0][0][1] = 29;
    keyboard_table[68][0][1][1] = 3;
    keyboard_table[68][1][0][0] = 97;
    keyboard_table[68][1][1][0] = 127;
    keyboard_table[68][1][0][1] = 127;
    keyboard_table[68][1][1][1] = 97;
    
    // E
    keyboard_table[69][0][0][0] = 4;
    keyboard_table[69][0][1][0] = 30;
    keyboard_table[69][0][0][1] = 30;
    keyboard_table[69][0][1][1] = 4;
    keyboard_table[69][1][0][0] = 98;
    keyboard_table[69][1][1][0] = 128;
    keyboard_table[69][1][0][1] = 128;
    keyboard_table[69][1][1][1] = 98;
    
    // F
    keyboard_table[70][0][0][0] = 5;
    keyboard_table[70][0][1][0] = 31;
    keyboard_table[70][0][0][1] = 31;
    keyboard_table[70][0][1][1] = 5;
    keyboard_table[70][1][0][0] = 113;
    keyboard_table[70][1][1][0] = 143;
    keyboard_table[70][1][0][1] = 143;
    keyboard_table[70][1][1][1] = 113;
    
    // G
    keyboard_table[71][0][0][0] = 6;
    keyboard_table[71][0][1][0] = 32;
    keyboard_table[71][0][0][1] = 32;
    keyboard_table[71][0][1][1] = 6;
    keyboard_table[71][1][0][0] = 96;
    keyboard_table[71][1][1][0] = 126;
    keyboard_table[71][1][0][1] = 126;
    keyboard_table[71][1][1][1] = 96;
    
    // H
    keyboard_table[72][0][0][0] = 7;
    keyboard_table[72][0][1][0] = 33;
    keyboard_table[72][0][0][1] = 33;
    keyboard_table[72][0][1][1] = 7;
    keyboard_table[72][1][0][0] = 114;
    keyboard_table[72][1][1][0] = 144;
    keyboard_table[72][1][0][1] = 144;
    keyboard_table[72][1][1][1] = 114;
    
    // I
    keyboard_table[73][0][0][0] = 8;
    keyboard_table[73][0][1][0] = 34;
    keyboard_table[73][0][0][1] = 34;
    keyboard_table[73][0][1][1] = 8;
    keyboard_table[73][1][0][0] = 101;
    keyboard_table[73][1][1][0] = 132;
    keyboard_table[73][1][0][1] = 132;
    keyboard_table[73][1][1][1] = 101;
    
    // J
    keyboard_table[74][0][0][0] = 9;
    keyboard_table[74][0][1][0] = 35;
    keyboard_table[74][0][0][1] = 35;
    keyboard_table[74][0][1][1] = 9;
    keyboard_table[74][1][0][0] = 102;
    keyboard_table[74][1][1][0] = 131;
    keyboard_table[74][1][0][1] = 131;
    keyboard_table[74][1][1][1] = 102;
    
    // K
    keyboard_table[75][0][0][0] = 10;
    keyboard_table[75][0][1][0] = 36;
    keyboard_table[75][0][0][1] = 36;
    keyboard_table[75][0][1][1] = 10;
    keyboard_table[75][1][0][0] = 103;
    keyboard_table[75][1][1][0] = 133;
    keyboard_table[75][1][0][1] = 133;
    keyboard_table[75][1][1][1] = 103;
    
    // L
    keyboard_table[76][0][0][0] = 11;
    keyboard_table[76][0][1][0] = 37;
    keyboard_table[76][0][0][1] = 37;
    keyboard_table[76][0][1][1] = 11;
    keyboard_table[76][1][0][0] = 104;
    keyboard_table[76][1][1][0] = 134;
    keyboard_table[76][1][0][1] = 134;
    keyboard_table[76][1][1][1] = 104;
    
    // M
    keyboard_table[77][0][0][0] = 12;
    keyboard_table[77][0][1][0] = 38;
    keyboard_table[77][0][0][1] = 38;
    keyboard_table[77][0][1][1] = 12;
    keyboard_table[77][1][0][0] = 105;
    keyboard_table[77][1][1][0] = 135;
    keyboard_table[77][1][0][1] = 135;
    keyboard_table[77][1][1][1] = 105;
    
    // N
    keyboard_table[78][0][0][0] = 13;
    keyboard_table[78][0][1][0] = 39;
    keyboard_table[78][0][0][1] = 39;
    keyboard_table[78][0][1][1] = 13;
    keyboard_table[78][1][0][0] = 106;
    keyboard_table[78][1][1][0] = 136;
    keyboard_table[78][1][0][1] = 136;
    keyboard_table[78][1][1][1] = 106;
    
    // O
    keyboard_table[79][0][0][0] = 14;
    keyboard_table[79][0][1][0] = 40;
    keyboard_table[79][0][0][1] = 40;
    keyboard_table[79][0][1][1] = 14;
    keyboard_table[79][1][0][0] = 107;
    keyboard_table[79][1][1][0] = 137;
    keyboard_table[79][1][0][1] = 137;
    keyboard_table[79][1][1][1] = 107;
    
    // P
    keyboard_table[80][0][0][0] = 15;
    keyboard_table[80][0][1][0] = 41;
    keyboard_table[80][0][0][1] = 41;
    keyboard_table[80][0][1][1] = 15;
    keyboard_table[80][1][0][0] = 108;
    keyboard_table[80][1][1][0] = 138;
    keyboard_table[80][1][0][1] = 138;
    keyboard_table[80][1][1][1] = 108;
    
    // Q
    keyboard_table[81][0][0][0] = 16;
    keyboard_table[81][0][1][0] = 42;
    keyboard_table[81][0][0][1] = 42;
    keyboard_table[81][0][1][1] = 16;
    keyboard_table[81][1][0][0] = 122;
    keyboard_table[81][1][1][0] = 152;
    keyboard_table[81][1][0][1] = 152;
    keyboard_table[81][1][1][1] = 122;
    
    // R
    keyboard_table[82][0][0][0] = 17;
    keyboard_table[82][0][1][0] = 43;
    keyboard_table[82][0][0][1] = 43;
    keyboard_table[82][0][1][1] = 17;
    keyboard_table[82][1][0][0] = 109;
    keyboard_table[82][1][1][0] = 139;
    keyboard_table[82][1][0][1] = 139;
    keyboard_table[82][1][1][1] = 109;
    
    // S
    keyboard_table[83][0][0][0] = 18;
    keyboard_table[83][0][1][0] = 44;
    keyboard_table[83][0][0][1] = 44;
    keyboard_table[83][0][1][1] = 18;
    keyboard_table[83][1][0][0] = 110;
    keyboard_table[83][1][1][0] = 140;
    keyboard_table[83][1][0][1] = 140;
    keyboard_table[83][1][1][1] = 110;
    
    // T
    keyboard_table[84][0][0][0] = 19;
    keyboard_table[84][0][1][0] = 45;
    keyboard_table[84][0][0][1] = 45;
    keyboard_table[84][0][1][1] = 19;
    keyboard_table[84][1][0][0] = 111;
    keyboard_table[84][1][1][0] = 141;
    keyboard_table[84][1][0][1] = 141;
    keyboard_table[84][1][1][1] = 111;
    
    // U
    keyboard_table[85][0][0][0] = 20;
    keyboard_table[85][0][1][0] = 46;
    keyboard_table[85][0][0][1] = 46;
    keyboard_table[85][0][1][1] = 20;
    keyboard_table[85][1][0][0] = 112;
    keyboard_table[85][1][1][0] = 142;
    keyboard_table[85][1][0][1] = 142;
    keyboard_table[85][1][1][1] = 112;
    
    // V
    keyboard_table[86][0][0][0] = 21;
    keyboard_table[86][0][1][0] = 47;
    keyboard_table[86][0][0][1] = 47;
    keyboard_table[86][0][1][1] = 21;
    keyboard_table[86][1][0][0] = 99;
    keyboard_table[86][1][1][0] = 129;
    keyboard_table[86][1][0][1] = 129;
    keyboard_table[86][1][1][1] = 99;
    
    // W
    keyboard_table[87][0][0][0] = 22;
    keyboard_table[87][0][1][0] = 48;
    keyboard_table[87][0][0][1] = 48;
    keyboard_table[87][0][1][1] = 22;
    keyboard_table[87][1][0][0] = 95;
    keyboard_table[87][1][1][0] = 125;
    keyboard_table[87][1][0][1] = 125;
    keyboard_table[87][1][1][1] = 95;
    
    // X
    keyboard_table[88][0][0][0] = 23;
    keyboard_table[88][0][1][0] = 49;
    keyboard_table[88][0][0][1] = 49;
    keyboard_table[88][0][1][1] = 23;
    keyboard_table[88][1][0][0] = 120;
    keyboard_table[88][1][1][0] = 150;
    keyboard_table[88][1][0][1] = 153;
    keyboard_table[88][1][1][1] = 153;
    
    // Y
    keyboard_table[89][0][0][0] = 24;
    keyboard_table[89][0][1][0] = 50;
    keyboard_table[89][0][0][1] = 50;
    keyboard_table[89][0][1][1] = 24;
    keyboard_table[89][1][0][0] = 119;
    keyboard_table[89][1][1][0] = 149;
    keyboard_table[89][1][0][1] = 149;
    keyboard_table[89][1][1][1] = 119;
    
    // Z
    keyboard_table[90][0][0][0] = 25;
    keyboard_table[90][0][1][0] = 51;
    keyboard_table[90][0][0][1] = 51;
    keyboard_table[90][0][1][1] = 25;
    keyboard_table[90][1][0][0] = 100;
    keyboard_table[90][1][1][0] = 130;
    keyboard_table[90][1][0][1] = 130;
    keyboard_table[90][1][1][1] = 100;
    
    // 0
    keyboard_table[48][0][0][0] = 52;
    keyboard_table[48][0][1][0] = 52;
    keyboard_table[48][0][0][1] = 72;
    keyboard_table[48][0][1][1] = 72;
    keyboard_table[48][1][0][0] = 52;
    keyboard_table[48][1][1][0] = 52;
    keyboard_table[48][1][0][1] = 72;
    keyboard_table[48][1][1][1] = 72;
    
    // 1
    keyboard_table[49][0][0][0] = 53;
    keyboard_table[49][0][1][0] = 53;
    keyboard_table[49][0][0][1] = 63;
    keyboard_table[49][0][1][1] = 63;
    keyboard_table[49][1][0][0] = 53;
    keyboard_table[49][1][1][0] = 53;
    keyboard_table[49][1][0][1] = 63;
    keyboard_table[49][1][1][1] = 63;
    
    // 2
    keyboard_table[50][0][0][0] = 54;
    keyboard_table[50][0][1][0] = 54;
    keyboard_table[50][0][0][1] = 64;
    keyboard_table[50][0][1][1] = 64;
    keyboard_table[50][1][0][0] = 54;
    keyboard_table[50][1][1][0] = 54;
    keyboard_table[50][1][0][1] = 64;
    keyboard_table[50][1][1][1] = 64;
    
    // 3
    keyboard_table[51][0][0][0] = 55;
    keyboard_table[51][0][1][0] = 55;
    keyboard_table[51][0][0][1] = 65;
    keyboard_table[51][0][1][1] = 65;
    keyboard_table[51][1][0][0] = 55;
    keyboard_table[51][1][1][0] = 55;
    keyboard_table[51][1][0][1] = 65;
    keyboard_table[51][1][1][1] = 65;
    
    // 4
    keyboard_table[52][0][0][0] = 56;
    keyboard_table[52][0][1][0] = 56;
    keyboard_table[52][0][0][1] = 66;
    keyboard_table[52][0][1][1] = 66;
    keyboard_table[52][1][0][0] = 56;
    keyboard_table[52][1][1][0] = 56;
    keyboard_table[52][1][0][1] = 66;
    keyboard_table[52][1][1][1] = 66;
    
    // 5
    keyboard_table[53][0][0][0] = 57;
    keyboard_table[53][0][1][0] = 57;
    keyboard_table[53][0][0][1] = 67;
    keyboard_table[53][0][1][1] = 67;
    keyboard_table[53][1][0][0] = 57;
    keyboard_table[53][1][1][0] = 57;
    keyboard_table[53][1][0][1] = 67;
    keyboard_table[53][1][1][1] = 67;
    
    // 6
    keyboard_table[54][0][0][0] = 58;
    keyboard_table[54][0][1][0] = 58;
    keyboard_table[54][0][0][1] = 68;
    keyboard_table[54][0][1][1] = 68;
    keyboard_table[54][1][0][0] = 58;
    keyboard_table[54][1][1][0] = 58;
    keyboard_table[54][1][0][1] = 68;
    keyboard_table[54][1][1][1] = 68;
    
    // 7
    keyboard_table[55][0][0][0] = 59;
    keyboard_table[55][0][1][0] = 59;
    keyboard_table[55][0][0][1] = 69;
    keyboard_table[55][0][1][1] = 69;
    keyboard_table[55][1][0][0] = 59;
    keyboard_table[55][1][1][0] = 59;
    keyboard_table[55][1][0][1] = 69;
    keyboard_table[55][1][1][1] = 69;
    
    // 8
    keyboard_table[56][0][0][0] = 60;
    keyboard_table[56][0][1][0] = 60;
    keyboard_table[56][0][0][1] = 70;
    keyboard_table[56][0][1][1] = 70;
    keyboard_table[56][1][0][0] = 60;
    keyboard_table[56][1][1][0] = 60;
    keyboard_table[56][1][0][1] = 70;
    keyboard_table[56][1][1][1] = 70;
    
    // 9
    keyboard_table[57][0][0][0] = 61;
    keyboard_table[57][0][1][0] = 61;
    keyboard_table[57][0][0][1] = 71;
    keyboard_table[57][0][1][1] = 71;
    keyboard_table[57][1][0][0] = 61;
    keyboard_table[57][1][1][0] = 61;
    keyboard_table[57][1][0][1] = 71;
    keyboard_table[57][1][1][1] = 71;
    
    // ~
    keyboard_table[192][0][0][0] = 62;
    keyboard_table[192][0][1][0] = 62;
    keyboard_table[192][0][0][1] = 154;
    keyboard_table[192][0][1][1] = 154;
    keyboard_table[192][1][0][0] = 115;
    keyboard_table[192][1][1][0] = 145;
    keyboard_table[192][1][0][1] = 145;
    keyboard_table[192][1][1][1] = 115;
    
    // _
    keyboard_table[189][0][0][0] = 75;
    keyboard_table[189][0][1][0] = 75;
    keyboard_table[189][0][0][1] = 73;
    keyboard_table[189][0][1][1] = 73;
    keyboard_table[189][1][0][0] = 75;
    keyboard_table[189][1][1][0] = 75;
    keyboard_table[189][1][0][1] = 73;
    keyboard_table[189][1][1][1] = 73;
    
    // +
    keyboard_table[187][0][0][0] = 76;
    keyboard_table[187][0][1][0] = 76;
    keyboard_table[187][0][0][1] = 74;
    keyboard_table[187][0][1][1] = 74;
    keyboard_table[187][1][0][0] = 76;
    keyboard_table[187][1][1][0] = 76;
    keyboard_table[187][1][0][1] = 74;
    keyboard_table[187][1][1][1] = 74;
    
    // num0
    keyboard_table[96][0][0][0] = 52;
    keyboard_table[96][0][1][0] = 52;
    keyboard_table[96][0][0][1] = 52;
    keyboard_table[96][0][1][1] = 52;
    keyboard_table[96][1][0][0] = 52;
    keyboard_table[96][1][1][0] = 52;
    keyboard_table[96][1][0][1] = 52;
    keyboard_table[96][1][1][1] = 52;
    
    // num1
    keyboard_table[97][0][0][0] = 53;
    keyboard_table[97][0][1][0] = 53;
    keyboard_table[97][0][0][1] = 53;
    keyboard_table[97][0][1][1] = 53;
    keyboard_table[97][1][0][0] = 53;
    keyboard_table[97][1][1][0] = 53;
    keyboard_table[97][1][0][1] = 53;
    keyboard_table[97][1][1][1] = 53;
    
    // num2
    keyboard_table[98][0][0][0] = 54;
    keyboard_table[98][0][1][0] = 54;
    keyboard_table[98][0][0][1] = 54;
    keyboard_table[98][0][1][1] = 54;
    keyboard_table[98][1][0][0] = 54;
    keyboard_table[98][1][1][0] = 54;
    keyboard_table[98][1][0][1] = 54;
    keyboard_table[98][1][1][1] = 54;
    
    // num3
    keyboard_table[99][0][0][0] = 55;
    keyboard_table[99][0][1][0] = 55;
    keyboard_table[99][0][0][1] = 55;
    keyboard_table[99][0][1][1] = 55;
    keyboard_table[99][1][0][0] = 55;
    keyboard_table[99][1][1][0] = 55;
    keyboard_table[99][1][0][1] = 55;
    keyboard_table[99][1][1][1] = 55;
    
    // num4
    keyboard_table[100][0][0][0] = 56;
    keyboard_table[100][0][1][0] = 56;
    keyboard_table[100][0][0][1] = 56;
    keyboard_table[100][0][1][1] = 56;
    keyboard_table[100][1][0][0] = 56;
    keyboard_table[100][1][1][0] = 56;
    keyboard_table[100][1][0][1] = 56;
    keyboard_table[100][1][1][1] = 56;
    
    // num5
    keyboard_table[101][0][0][0] = 57;
    keyboard_table[101][0][1][0] = 57;
    keyboard_table[101][0][0][1] = 57;
    keyboard_table[101][0][1][1] = 57;
    keyboard_table[101][1][0][0] = 57;
    keyboard_table[101][1][1][0] = 57;
    keyboard_table[101][1][0][1] = 57;
    keyboard_table[101][1][1][1] = 57;
    
    // num6
    keyboard_table[102][0][0][0] = 58;
    keyboard_table[102][0][1][0] = 58;
    keyboard_table[102][0][0][1] = 58;
    keyboard_table[102][0][1][1] = 58;
    keyboard_table[102][1][0][0] = 58;
    keyboard_table[102][1][1][0] = 58;
    keyboard_table[102][1][0][1] = 58;
    keyboard_table[102][1][1][1] = 58;
    
    // num7
    keyboard_table[103][0][0][0] = 59;
    keyboard_table[103][0][1][0] = 59;
    keyboard_table[103][0][0][1] = 59;
    keyboard_table[103][0][1][1] = 59;
    keyboard_table[103][1][0][0] = 59;
    keyboard_table[103][1][1][0] = 59;
    keyboard_table[103][1][0][1] = 59;
    keyboard_table[103][1][1][1] = 59;
    
    // num8
    keyboard_table[104][0][0][0] = 60;
    keyboard_table[104][0][1][0] = 60;
    keyboard_table[104][0][0][1] = 60;
    keyboard_table[104][0][1][1] = 60;
    keyboard_table[104][1][0][0] = 60;
    keyboard_table[104][1][1][0] = 60;
    keyboard_table[104][1][0][1] = 60;
    keyboard_table[104][1][1][1] = 60;
    
    // num9
    keyboard_table[105][0][0][0] = 61;
    keyboard_table[105][0][1][0] = 61;
    keyboard_table[105][0][0][1] = 61;
    keyboard_table[105][0][1][1] = 61;
    keyboard_table[105][1][0][0] = 61;
    keyboard_table[105][1][1][0] = 61;
    keyboard_table[105][1][0][1] = 61;
    keyboard_table[105][1][1][1] = 61;
    
    // num.
    keyboard_table[110][0][0][0] = 89;
    keyboard_table[110][0][1][0] = 89;
    keyboard_table[110][0][0][1] = 89;
    keyboard_table[110][0][1][1] = 89;
    keyboard_table[110][1][0][0] = 89;
    keyboard_table[110][1][1][0] = 89;
    keyboard_table[110][1][0][1] = 89;
    keyboard_table[110][1][1][1] = 89;
    
    // num+
    keyboard_table[107][0][0][0] = 74;
    keyboard_table[107][0][1][0] = 74;
    keyboard_table[107][0][0][1] = 74;
    keyboard_table[107][0][1][1] = 74;
    keyboard_table[107][1][0][0] = 74;
    keyboard_table[107][1][1][0] = 74;
    keyboard_table[107][1][0][1] = 74;
    keyboard_table[107][1][1][1] = 74;
    
    // num-
    keyboard_table[109][0][0][0] = 75;
    keyboard_table[109][0][1][0] = 75;
    keyboard_table[109][0][0][1] = 75;
    keyboard_table[109][0][1][1] = 75;
    keyboard_table[109][1][0][0] = 75;
    keyboard_table[109][1][1][0] = 75;
    keyboard_table[109][1][0][1] = 75;
    keyboard_table[109][1][1][1] = 75;
    
    // num*
    keyboard_table[106][0][0][0] = 70;
    keyboard_table[106][0][1][0] = 70;
    keyboard_table[106][0][0][1] = 70;
    keyboard_table[106][0][1][1] = 70;
    keyboard_table[106][1][0][0] = 70;
    keyboard_table[106][1][1][0] = 70;
    keyboard_table[106][1][0][1] = 70;
    keyboard_table[106][1][1][1] = 70;
    
    // num/
    keyboard_table[111][0][0][0] = 87;
    keyboard_table[111][0][1][0] = 87;
    keyboard_table[111][0][0][1] = 87;
    keyboard_table[111][0][1][1] = 87;
    keyboard_table[111][1][0][0] = 87;
    keyboard_table[111][1][1][0] = 87;
    keyboard_table[111][1][0][1] = 87;
    keyboard_table[111][1][1][1] = 87;
    
    // ,
    keyboard_table[188][0][0][0] = 90;
    keyboard_table[188][0][1][0] = 90;
    keyboard_table[188][0][0][1] = 91;
    keyboard_table[188][0][1][1] = 91;
    keyboard_table[188][1][0][0] = 90;
    keyboard_table[188][1][1][0] = 90;
    keyboard_table[188][1][0][1] = 91;
    keyboard_table[188][1][1][1] = 91;
    
    // .
    keyboard_table[190][0][0][0] = 89;
    keyboard_table[190][0][1][0] = 89;
    keyboard_table[190][0][0][1] = 92;
    keyboard_table[190][0][1][1] = 92;
    keyboard_table[190][1][0][0] = 89;
    keyboard_table[190][1][1][0] = 89;
    keyboard_table[190][1][0][1] = 92;
    keyboard_table[190][1][1][1] = 92;
    
    // /
    keyboard_table[191][0][0][0] = 87;
    keyboard_table[191][0][1][0] = 87;
    keyboard_table[191][0][0][1] = 88;
    keyboard_table[191][0][1][1] = 88;
    keyboard_table[191][1][0][0] = 87;
    keyboard_table[191][1][1][0] = 87;
    keyboard_table[191][1][0][1] = 88;
    keyboard_table[191][1][1][1] = 88;
    
    // ;
    keyboard_table[186][0][0][0] = 85;
    keyboard_table[186][0][1][0] = 85;
    keyboard_table[186][0][0][1] = 86;
    keyboard_table[186][0][1][1] = 86;
    keyboard_table[186][1][0][0] = 85;
    keyboard_table[186][1][1][0] = 85;
    keyboard_table[186][1][0][1] = 86;
    keyboard_table[186][1][1][1] = 86;
    
    // '
    keyboard_table[222][0][0][0] = 83;
    keyboard_table[222][0][1][0] = 83;
    keyboard_table[222][0][0][1] = 84;
    keyboard_table[222][0][1][1] = 84;
    keyboard_table[222][1][0][0] = 83;
    keyboard_table[222][1][1][0] = 83;
    keyboard_table[222][1][0][1] = 84;
    keyboard_table[222][1][1][1] = 84;
    
    /* \ */
    keyboard_table[220][0][0][0] = 81;
    keyboard_table[220][0][1][0] = 81;
    keyboard_table[220][0][0][1] = 82;
    keyboard_table[220][0][1][1] = 82;
    keyboard_table[220][1][0][0] = 121;
    keyboard_table[220][1][1][0] = 151;
    keyboard_table[220][1][0][1] = 151;
    keyboard_table[220][1][1][1] = 121;
    
    // [
    keyboard_table[219][0][0][0] = 77;
    keyboard_table[219][0][1][0] = 77;
    keyboard_table[219][0][0][1] = 79;
    keyboard_table[219][0][1][1] = 79;
    keyboard_table[219][1][0][0] = 117;
    keyboard_table[219][1][1][0] = 147;
    keyboard_table[219][1][0][1] = 147;
    keyboard_table[219][1][1][1] = 117;
    
    // ]
    keyboard_table[221][0][0][0] = 78;
    keyboard_table[221][0][1][0] = 78;
    keyboard_table[221][0][0][1] = 80;
    keyboard_table[221][0][1][1] = 80;
    keyboard_table[221][1][0][0] = 118;
    keyboard_table[221][1][1][0] = 148;
    keyboard_table[221][1][0][1] = 148;
    keyboard_table[221][1][1][1] = 118;
    
    // space
    keyboard_table[32][0][0][0] = 254;
    keyboard_table[32][0][1][0] = 254;
    keyboard_table[32][0][0][1] = 254;
    keyboard_table[32][0][1][1] = 254;
    keyboard_table[32][1][0][0] = 254;
    keyboard_table[32][1][1][0] = 254;
    keyboard_table[32][1][0][1] = 254;
    keyboard_table[32][1][1][1] = 254;
    
    // space
    ascii_table[32] = 254;
    
    // !
    ascii_table[33] = 63;
    
    // "
    ascii_table[34] = 84;
    
    // #
    ascii_table[35] = 65;
    
    // $
    ascii_table[36] = 66;
    
    // %
    ascii_table[37] = 67;
    
    // &
    ascii_table[38] = 69;
    
    // '
    ascii_table[39] = 83;
    
    // (
    ascii_table[40] = 71;
    
    // )
    ascii_table[41] = 72;
    
    // *
    ascii_table[42] = 70;
    
    // +
    ascii_table[43] = 74;
    
    // ,
    ascii_table[44] = 90;
    
    // -
    ascii_table[45] = 75;
    
    // .
    ascii_table[46] = 89;
    
    // /
    ascii_table[47] = 87;
    
    // 0
    ascii_table[48] = 52;
    
    // 1
    ascii_table[49] = 53;
    
    // 2
    ascii_table[50] = 54;
    
    // 3
    ascii_table[51] = 55;
    
    // 4
    ascii_table[52] = 56;
    
    // 5
    ascii_table[53] = 57;
    
    // 6
    ascii_table[54] = 58;
    
    // 7
    ascii_table[55] = 59;
    
    // 8
    ascii_table[56] = 60;
    
    // 9
    ascii_table[57] = 61;
    
    // :
    ascii_table[58] = 86;
    
    // ;
    ascii_table[59] = 85;
    
    // <
    ascii_table[60] = 91;
    
    // =
    ascii_table[61] = 76;
    
    // >
    ascii_table[62] = 92;
    
    // ?
    ascii_table[63] = 88;
    
    // @
    ascii_table[64] = 64;
    
    // A
    ascii_table[65] = 26;
    
    // B
    ascii_table[66] = 27;
    
    // C
    ascii_table[67] = 28;
    
    // D
    ascii_table[68] = 29;
    
    // E
    ascii_table[69] = 30;
    
    // F
    ascii_table[70] = 31;
    
    // G
    ascii_table[71] = 32;
    
    // H
    ascii_table[72] = 33;
    
    // I
    ascii_table[73] = 34;
    
    // J
    ascii_table[74] = 35;
    
    // K
    ascii_table[75] = 36;
    
    // L
    ascii_table[76] = 37;
    
    // M
    ascii_table[77] = 38;
    
    // N
    ascii_table[78] = 39;
    
    // O
    ascii_table[79] = 40;
    
    // P
    ascii_table[80] = 41;
    
    // Q
    ascii_table[81] = 42;
    
    // R
    ascii_table[82] = 43;
    
    // S
    ascii_table[83] = 44;
    
    // T
    ascii_table[84] = 45;
    
    // U
    ascii_table[85] = 46;
    
    // V
    ascii_table[86] = 47;
    
    // W
    ascii_table[87] = 48;
    
    // X
    ascii_table[88] = 49;
    
    // Y
    ascii_table[89] = 50;
    
    // Z
    ascii_table[90] = 51;
    
    // [
    ascii_table[91] = 77;
    
    /* \ */
    ascii_table[92] = 81;
    
    // ]
    ascii_table[93] = 78;
    
    // ^
    ascii_table[94] = 68;
    
    // _
    ascii_table[95] = 73;
    
    // `
    ascii_table[96] = 62;
    
    // a
    ascii_table[97] = 0;
    
    // b
    ascii_table[98] = 1;
    
    // c
    ascii_table[99] = 2;
    
    // d
    ascii_table[100] = 3;
    
    // e
    ascii_table[101] = 4;
    
    // f
    ascii_table[102] = 5;
    
    // g
    ascii_table[103] = 6;
    
    // h
    ascii_table[104] = 7;
    
    // i
    ascii_table[105] = 8;
    
    // j
    ascii_table[106] = 9;
    
    // k
    ascii_table[107] = 10;
    
    // l
    ascii_table[108] = 11;
    
    // m
    ascii_table[109] = 12;
    
    // n
    ascii_table[110] = 13;
    
    // o
    ascii_table[111] = 14;
    
    // p
    ascii_table[112] = 15;
    
    // q
    ascii_table[113] = 16;
    
    // r
    ascii_table[114] = 17;
    
    // s
    ascii_table[115] = 18;
    
    // t
    ascii_table[116] = 19;
    
    // u
    ascii_table[117] = 20;
    
    // v
    ascii_table[118] = 21;
    
    // w
    ascii_table[119] = 22;
    
    // x
    ascii_table[120] = 23;
    
    // y
    ascii_table[121] = 24;
    
    // z
    ascii_table[122] = 25;
    
    // {
    ascii_table[123] = 79;
    
    // |
    ascii_table[124] = 82;
    
    // }
    ascii_table[125] = 80;
    
    // ~
    ascii_table[126] = 154;
    
    // А
    ascii_table[192] = 123;
    
    // Б
    ascii_table[193] = 124;
    
    // В
    ascii_table[194] = 125;
    
    // Г
    ascii_table[195] = 126;
    
    // Д
    ascii_table[196] = 127;
    
    // Е
    ascii_table[197] = 128;
    
    // Ж
    ascii_table[198] = 129;
    
    // З
    ascii_table[199] = 130;
    
    // И
    ascii_table[200] = 132;
    
    // Й
    ascii_table[201] = 131;
    
    // К
    ascii_table[202] = 133;
    
    // Л
    ascii_table[203] = 134;
    
    // М
    ascii_table[204] = 135;
    
    // Н
    ascii_table[205] = 136;
    
    // О
    ascii_table[206] = 137;
    
    // П
    ascii_table[207] = 138;
    
    // Р
    ascii_table[208] = 139;
    
    // С
    ascii_table[209] = 140;
    
    // Т
    ascii_table[210] = 141;
    
    // У
    ascii_table[211] = 142;
    
    // Ф
    ascii_table[212] = 143;
    
    // Х
    ascii_table[213] = 144;
    
    // Ц
    ascii_table[214] = 146;
    
    // Ч
    ascii_table[215] = 145;
    
    // Ш
    ascii_table[216] = 147;
    
    // Щ
    ascii_table[217] = 148;
    
    // Ъ
    ascii_table[218] = 149;
    
    // Ь
    ascii_table[220] = 150;
    
    // Ю
    ascii_table[222] = 151;
    
    // Я
    ascii_table[223] = 152;
    
    // а
    ascii_table[224] = 93;
    
    // б
    ascii_table[225] = 94;
    
    // в
    ascii_table[226] = 95;
    
    // г
    ascii_table[227] = 96;
    
    // д
    ascii_table[228] = 97;
    
    // е
    ascii_table[229] = 98;
    
    // ж
    ascii_table[230] = 99;
    
    // з
    ascii_table[231] = 100;
    
    // и
    ascii_table[232] = 101;
    
    // й
    ascii_table[233] = 102;
    
    // к
    ascii_table[234] = 103;
    
    // л
    ascii_table[235] = 104;
    
    // м
    ascii_table[236] = 105;
    
    // н
    ascii_table[237] = 106;
    
    // о
    ascii_table[238] = 107;
    
    // п
    ascii_table[239] = 108;
    
    // р
    ascii_table[240] = 109;
    
    // с
    ascii_table[241] = 110;
    
    // т
    ascii_table[242] = 111;
    
    // у
    ascii_table[243] = 112;
    
    // ф
    ascii_table[244] = 113;
    
    // х
    ascii_table[245] = 114;
    
    // ц
    ascii_table[246] = 116;
    
    // ч
    ascii_table[247] = 115;
    
    // ш
    ascii_table[248] = 117;
    
    // щ
    ascii_table[249] = 118;
    
    // ъ
    ascii_table[250] = 119;
    
    // ь
    ascii_table[252] = 120;
    
    // ю
    ascii_table[254] = 121;
    
    // я
    ascii_table[255] = 122;
}
void Uninitialize_Font()
{
    font.Free();
}
unsigned char get_keyboard_symbol(unsigned char in, bool eng, bool caps, bool shift)
{
    return keyboard_table[(int)in][!eng][caps][shift];
}
unsigned char get_ascii_symbol(unsigned char in)
{
    return ascii_table[(int)in];
}
void draw_text(unsigned char* text, float x, float y, float size)
{
    float X_OFF = 0;
    int len = strlen(text);
    glBindTexture(GL_TEXTURE_2D, font.id);
    glBegin(GL_QUADS);
    for(int i=0;i<len;++i)
    {
        if((int)text[i] != 254)
        {
            if(((int)text[i] == 62) || ((int)text[i] == 131) || ((int)text[i] == 153))
            {
                float Y_OFF = ((SYM[(int)text[i]].bottom - SYM[(int)text[i]].top) * 1024 - 64) * size;
                glTexCoord2f(SYM[(int)text[i]].left, 1 - SYM[(int)text[i]].top);
                glVertex2f(x + X_OFF * size, y - Y_OFF);
                glTexCoord2f(SYM[(int)text[i]].right, 1 - SYM[(int)text[i]].top);
                glVertex2f(x + (X_OFF + (SYM[(int)text[i]].right - SYM[(int)text[i]].left) * 1024) * size, y - Y_OFF);
                glTexCoord2f(SYM[(int)text[i]].right, 1 - SYM[(int)text[i]].bottom);
                glVertex2f(x + (X_OFF + (SYM[(int)text[i]].right - SYM[(int)text[i]].left) * 1024) * size, y + ((SYM[(int)text[i]].bottom - SYM[(int)text[i]].top) * 1024) * size - Y_OFF);
                glTexCoord2f(SYM[(int)text[i]].left, 1 - SYM[(int)text[i]].bottom);
                glVertex2f(x + X_OFF * size, y + ((SYM[(int)text[i]].bottom - SYM[(int)text[i]].top) * 1024) * size - Y_OFF);
                X_OFF += (SYM[(int)text[i]].right - SYM[(int)text[i]].left) * 1024;
            }
            else
            {
                glTexCoord2f(SYM[(int)text[i]].left, 1 - SYM[(int)text[i]].top);
                glVertex2f(x + X_OFF * size, y);
                glTexCoord2f(SYM[(int)text[i]].right, 1 - SYM[(int)text[i]].top);
                glVertex2f(x + (X_OFF + (SYM[(int)text[i]].right - SYM[(int)text[i]].left) * 1024) * size, y);
                glTexCoord2f(SYM[(int)text[i]].right, 1 - SYM[(int)text[i]].bottom);
                glVertex2f(x + (X_OFF + (SYM[(int)text[i]].right - SYM[(int)text[i]].left) * 1024) * size, y + ((SYM[(int)text[i]].bottom - SYM[(int)text[i]].top) * 1024) * size);
                glTexCoord2f(SYM[(int)text[i]].left, 1 - SYM[(int)text[i]].bottom);
                glVertex2f(x + X_OFF * size, y + ((SYM[(int)text[i]].bottom - SYM[(int)text[i]].top) * 1024) * size);
                X_OFF += (SYM[(int)text[i]].right - SYM[(int)text[i]].left) * 1024;
            }
        }
        else
        {
            X_OFF += 16;
        }
    }
    glEnd();
}
unsigned char* make_string_to_text(char* in)
{
    unsigned char* result = new unsigned char [strlen(in) + 1];
    int len = strlen(in);
    for(int i = 0; i < len; ++i)
    {
        result[i] = get_ascii_symbol((unsigned char)in[i]);
    }
    result[strlen(in)] = 255;
    return result;
}
unsigned char* make_number_to_text(int in)
{
    unsigned char* result;
    unsigned short length = 0;
    bool negative = false;
    if(in < 0)
    {
        negative = true;
        in = -in;
    }
    int copy = in;
    do
    {
        ++length;
        copy /= 10;
    }
    while(copy > 0);
    if(negative)
    {
        ++length;
    }
    result = new unsigned char [length + 1];
    for(int i = length - 1; i >= 0; --i)
    {
        result[i] = get_ascii_symbol((unsigned char)((in % 10) + '0'));
        in /= 10;
    }
    if(negative)
    {
        result[0] = get_ascii_symbol((unsigned char)'-');
    }
    result[length] = 255;
    return result;
}
unsigned char* make_number_to_text(float in)
{
    unsigned char* result;
    unsigned short length = 0, prec_len = 4;
    bool negative = false;
    if(in < 0)
    {
        negative = true;
        in = -in;
    }
    int copy = (int)in;
    do
    {
        ++length;
        copy /= 10;
    }
    while(copy > 0);
    length += 4;
    copy = (int)(in * 10000) - ((int)in) * 100000;
    while((copy % 10) == 0 && (prec_len))
    {
        --length;
        --prec_len;
        copy /= 10;
    }
    if(prec_len)
    {
        ++length;
    }
    if(negative)
    {
        ++length;
    }
    copy = (int)(in * 10000);
    for(int i = 0; i < 4 - prec_len; ++i)
    {
        copy /= 10;
    }
    result = new unsigned char [length + 1];
    for(int i = length - 1; i >= 0; --i)
    {
        if((i + 1 == length - prec_len) && prec_len)
        {
            result[i] = get_ascii_symbol((unsigned char)'.');
        }
        else
        {
            result[i] = get_ascii_symbol((unsigned char)(copy % 10 + '0'));
            copy /= 10;
        }
    }
    if(negative)
    {
        result[0] = get_ascii_symbol((unsigned char)'-');
    }
    result[length] = 255;
    return result;
}

#endif
