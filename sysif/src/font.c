#include <stdint.h>
#include "font.h"
#include "font_spec.h"
#include "kheap.h"
#include "fb.h"
#include "math.h"
#include "vmem.h"

#define FONT_TABLE_SIZE 128
#define FONT_TABLE_START 33
#define FONT_TABLE_END 126
#define SIZE_OF_BLOCK 8
#define CURSOR_BUFFER_SIZE 10

FontTable* initFont()
{
    FontTable* font = (FontTable *)sys_mmap(sizeof (FontTable));
    font->values = (char **)sys_mmap(FONT_TABLE_SIZE * sizeof (char *)); // ASCII Size Table
    font->widths = (uint32_t *)sys_mmap(FONT_TABLE_SIZE * sizeof (uint32_t)); // ASCII Size Table
    font->heights = (uint32_t *)sys_mmap(FONT_TABLE_SIZE * sizeof (uint32_t)); // ASCII Size Table

    for (uint32_t i = 0; i < FONT_TABLE_SIZE; ++i) {
        font->values[i] = char_font_63_bits;
        font->widths[i] = char_font_63_width;
        font->heights[i] = char_font_63_height;
    }

    font->values[33] = char_font_33_bits; // !
    font->widths[33] = char_font_33_width;
    font->heights[33] = char_font_33_height;
    font->values[34] = char_font_34_bits; // "
    font->widths[34] = char_font_34_width;
    font->heights[34] = char_font_34_height;
    font->values[35] = char_font_35_bits; // #
    font->widths[35] = char_font_35_width;
    font->heights[35] = char_font_35_height;
    font->values[36] = char_font_36_bits; // $
    font->widths[36] = char_font_36_width;
    font->heights[36] = char_font_36_height;
    font->values[37] = char_font_37_bits; // %
    font->widths[37] = char_font_37_width;
    font->heights[37] = char_font_37_height;
    font->values[38] = char_font_38_bits; // &
    font->widths[38] = char_font_38_width;
    font->heights[38] = char_font_38_height;
    font->values[39] = char_font_39_bits; // '
    font->widths[39] = char_font_39_width;
    font->heights[39] = char_font_39_height;
    font->values[40] = char_font_40_bits; // (
    font->widths[40] = char_font_40_width;
    font->heights[40] = char_font_40_height;
    font->values[41] = char_font_41_bits; // )
    font->widths[41] = char_font_41_width;
    font->heights[41] = char_font_41_height;
    font->values[42] = char_font_42_bits; // *
    font->widths[42] = char_font_42_width;
    font->heights[42] = char_font_42_height;
    font->values[43] = char_font_43_bits; // +
    font->widths[43] = char_font_43_width;
    font->heights[43] = char_font_43_height;
    font->values[44] = char_font_44_bits; // ,
    font->widths[44] = char_font_44_width;
    font->heights[44] = char_font_44_height;
    font->values[45] = char_font_45_bits; // -
    font->widths[45] = char_font_45_width;
    font->heights[45] = char_font_45_height;
    font->values[46] = char_font_46_bits; // .
    font->widths[46] = char_font_46_width;
    font->heights[46] = char_font_46_height;
    font->values[47] = char_font_47_bits; // /
    font->widths[47] = char_font_47_width;
    font->heights[47] = char_font_47_height;
    font->values[48] = char_font_48_bits; // 0
    font->widths[48] = char_font_48_width;
    font->heights[48] = char_font_48_height;
    font->values[49] = char_font_49_bits; // 1
    font->widths[49] = char_font_49_width;
    font->heights[49] = char_font_49_height;
    font->values[50] = char_font_50_bits; // 2
    font->widths[50] = char_font_50_width;
    font->heights[50] = char_font_50_height;
    font->values[51] = char_font_51_bits; // 3
    font->widths[51] = char_font_51_width;
    font->heights[51] = char_font_51_height;
    font->values[52] = char_font_52_bits; // 4
    font->widths[52] = char_font_52_width;
    font->heights[52] = char_font_52_height;
    font->values[53] = char_font_53_bits; // 5
    font->widths[53] = char_font_53_width;
    font->heights[53] = char_font_53_height;
    font->values[54] = char_font_54_bits; // 6
    font->widths[54] = char_font_54_width;
    font->heights[54] = char_font_54_height;
    font->values[55] = char_font_55_bits; // 7
    font->widths[55] = char_font_55_width;
    font->heights[55] = char_font_55_height;
    font->values[56] = char_font_56_bits; // 8
    font->widths[56] = char_font_56_width;
    font->heights[56] = char_font_56_height;
    font->values[57] = char_font_57_bits; // 9
    font->widths[57] = char_font_57_width;
    font->heights[57] = char_font_57_height;
    font->values[58] = char_font_58_bits; // :
    font->widths[58] = char_font_58_width;
    font->heights[58] = char_font_58_height;
    font->values[59] = char_font_59_bits; // ;
    font->widths[59] = char_font_59_width;
    font->heights[59] = char_font_59_height;
    font->values[60] = char_font_60_bits; // <
    font->widths[60] = char_font_60_width;
    font->heights[60] = char_font_60_height;
    font->values[61] = char_font_61_bits; // =
    font->widths[61] = char_font_61_width;
    font->heights[61] = char_font_61_height;
    font->values[62] = char_font_62_bits; // >
    font->widths[62] = char_font_62_width;
    font->heights[62] = char_font_62_height;
    font->values[63] = char_font_63_bits; // ?
    font->widths[63] = char_font_63_width;
    font->heights[63] = char_font_63_height;
    font->values[64] = char_font_64_bits; // @
    font->widths[64] = char_font_64_width;
    font->heights[64] = char_font_64_height;
    font->values[65] = char_font_65_bits; // A
    font->widths[65] = char_font_65_width;
    font->heights[65] = char_font_65_height;
    font->values[66] = char_font_66_bits; // B
    font->widths[66] = char_font_66_width;
    font->heights[66] = char_font_66_height;
    font->values[67] = char_font_67_bits; // C
    font->widths[67] = char_font_67_width;
    font->heights[67] = char_font_67_height;
    font->values[68] = char_font_68_bits; // D
    font->widths[68] = char_font_68_width;
    font->heights[68] = char_font_68_height;
    font->values[69] = char_font_69_bits; // E
    font->widths[69] = char_font_69_width;
    font->heights[69] = char_font_69_height;
    font->values[70] = char_font_70_bits; // F
    font->widths[70] = char_font_70_width;
    font->heights[70] = char_font_70_height;
    font->values[71] = char_font_71_bits; // G
    font->widths[71] = char_font_71_width;
    font->heights[71] = char_font_71_height;
    font->values[72] = char_font_72_bits; // H
    font->widths[72] = char_font_72_width;
    font->heights[72] = char_font_72_height;
    font->values[73] = char_font_73_bits; // I
    font->widths[73] = char_font_73_width;
    font->heights[73] = char_font_73_height;
    font->values[74] = char_font_74_bits; // J
    font->widths[74] = char_font_74_width;
    font->heights[74] = char_font_74_height;
    font->values[75] = char_font_75_bits; // K
    font->widths[75] = char_font_75_width;
    font->heights[75] = char_font_75_height;
    font->values[76] = char_font_76_bits; // L
    font->widths[76] = char_font_76_width;
    font->heights[76] = char_font_76_height;
    font->values[77] = char_font_77_bits; // M
    font->widths[77] = char_font_77_width;
    font->heights[77] = char_font_77_height;
    font->values[78] = char_font_78_bits; // N
    font->widths[78] = char_font_78_width;
    font->heights[78] = char_font_78_height;
    font->values[79] = char_font_79_bits; // O
    font->widths[79] = char_font_79_width;
    font->heights[79] = char_font_79_height;
    font->values[80] = char_font_80_bits; // P
    font->widths[80] = char_font_80_width;
    font->heights[80] = char_font_80_height;
    font->values[81] = char_font_81_bits; // Q
    font->widths[81] = char_font_81_width;
    font->heights[81] = char_font_81_height;
    font->values[82] = char_font_82_bits; // R
    font->widths[82] = char_font_82_width;
    font->heights[82] = char_font_82_height;
    font->values[83] = char_font_83_bits; // S
    font->widths[83] = char_font_83_width;
    font->heights[83] = char_font_83_height;
    font->values[84] = char_font_84_bits; // T
    font->widths[84] = char_font_84_width;
    font->heights[84] = char_font_84_height;
    font->values[85] = char_font_85_bits; // U
    font->widths[85] = char_font_85_width;
    font->heights[85] = char_font_85_height;
    font->values[86] = char_font_86_bits; // V
    font->widths[86] = char_font_86_width;
    font->heights[86] = char_font_86_height;
    font->values[87] = char_font_87_bits; // W
    font->widths[87] = char_font_87_width;
    font->heights[87] = char_font_87_height;
    font->values[88] = char_font_88_bits; // X
    font->widths[88] = char_font_88_width;
    font->heights[88] = char_font_88_height;
    font->values[89] = char_font_89_bits; // Y
    font->widths[89] = char_font_89_width;
    font->heights[89] = char_font_89_height;
    font->values[90] = char_font_90_bits; // Z
    font->widths[90] = char_font_90_width;
    font->heights[90] = char_font_90_height;
    font->values[91] = char_font_91_bits; // [
    font->widths[91] = char_font_91_width;
    font->heights[91] = char_font_91_height;
    font->values[92] = char_font_92_bits;
    font->widths[92] = char_font_92_width;
    font->heights[92] = char_font_92_height;
    font->values[93] = char_font_93_bits; // ]
    font->widths[93] = char_font_93_width;
    font->heights[93] = char_font_93_height;
    font->values[94] = char_font_94_bits; // ^
    font->widths[94] = char_font_94_width;
    font->heights[94] = char_font_94_height;
    font->values[95] = char_font_95_bits; // _
    font->widths[95] = char_font_95_width;
    font->heights[95] = char_font_95_height;
    font->values[96] = char_font_96_bits; // `
    font->widths[96] = char_font_96_width;
    font->heights[96] = char_font_96_height;
    font->values[97] = char_font_97_bits; // a
    font->widths[97] = char_font_97_width;
    font->heights[97] = char_font_97_height;
    font->values[98] = char_font_98_bits; // b
    font->widths[98] = char_font_98_width;
    font->heights[98] = char_font_98_height;
    font->values[99] = char_font_99_bits; // c
    font->widths[99] = char_font_99_width;
    font->heights[99] = char_font_99_height;
    font->values[100] = char_font_100_bits; // d
    font->widths[100] = char_font_100_width;
    font->heights[100] = char_font_100_height;
    font->values[101] = char_font_101_bits; // e
    font->widths[101] = char_font_101_width;
    font->heights[101] = char_font_101_height;
    font->values[102] = char_font_102_bits; // f
    font->widths[102] = char_font_102_width;
    font->heights[102] = char_font_102_height;
    font->values[103] = char_font_103_bits; // g
    font->widths[103] = char_font_103_width;
    font->heights[103] = char_font_103_height;
    font->values[104] = char_font_104_bits; // h
    font->widths[104] = char_font_104_width;
    font->heights[104] = char_font_104_height;
    font->values[105] = char_font_105_bits; // i
    font->widths[105] = char_font_105_width;
    font->heights[105] = char_font_105_height;
    font->values[106] = char_font_106_bits; // j
    font->widths[106] = char_font_106_width;
    font->heights[106] = char_font_106_height;
    font->values[107] = char_font_107_bits; // k
    font->widths[107] = char_font_107_width;
    font->heights[107] = char_font_107_height;
    font->values[108] = char_font_108_bits; // l
    font->widths[108] = char_font_108_width;
    font->heights[108] = char_font_108_height;
    font->values[109] = char_font_109_bits; // m
    font->widths[109] = char_font_109_width;
    font->heights[109] = char_font_109_height;
    font->values[110] = char_font_110_bits; // n
    font->widths[110] = char_font_110_width;
    font->heights[110] = char_font_110_height;
    font->values[111] = char_font_111_bits; // o
    font->widths[111] = char_font_111_width;
    font->heights[111] = char_font_111_height;
    font->values[112] = char_font_112_bits; // p
    font->widths[112] = char_font_112_width;
    font->heights[112] = char_font_112_height;
    font->values[113] = char_font_113_bits; // q
    font->widths[113] = char_font_113_width;
    font->heights[113] = char_font_113_height;
    font->values[114] = char_font_114_bits; // r
    font->widths[114] = char_font_114_width;
    font->heights[114] = char_font_114_height;
    font->values[115] = char_font_115_bits; // s
    font->widths[115] = char_font_115_width;
    font->heights[115] = char_font_115_height;
    font->values[116] = char_font_116_bits; // t
    font->widths[116] = char_font_116_width;
    font->heights[116] = char_font_116_height;
    font->values[117] = char_font_117_bits; // u
    font->widths[117] = char_font_117_width;
    font->heights[117] = char_font_117_height;
    font->values[118] = char_font_118_bits; // v
    font->widths[118] = char_font_118_width;
    font->heights[118] = char_font_118_height;
    font->values[119] = char_font_119_bits; // w
    font->widths[119] = char_font_119_width;
    font->heights[119] = char_font_119_height;
    font->values[120] = char_font_120_bits; // x
    font->widths[120] = char_font_120_width;
    font->heights[120] = char_font_120_height;
    font->values[121] = char_font_121_bits; // y
    font->widths[121] = char_font_121_width;
    font->heights[121] = char_font_121_height;
    font->values[122] = char_font_122_bits; // z
    font->widths[122] = char_font_122_width;
    font->heights[122] = char_font_122_height;
    font->values[123] = char_font_123_bits; // {
    font->widths[123] = char_font_123_width;
    font->heights[123] = char_font_123_height;
    font->values[124] = char_font_124_bits; // |
    font->widths[124] = char_font_124_width;
    font->heights[124] = char_font_124_height;
    font->values[125] = char_font_125_bits; // }
    font->widths[125] = char_font_125_width;
    font->heights[125] = char_font_125_height;
    font->values[126] = char_font_126_bits; // ~
    font->widths[126] = char_font_126_width;
    font->heights[126] = char_font_126_height;

    font->max_width = 13;
    font->max_height = 0;
    font->spacing_width = 0;

    for (uint32_t i = FONT_TABLE_START; i <= FONT_TABLE_END; ++i) {
        if (font->max_height < font->heights[i]) {
            font->max_height = font->heights[i];
        }
        if (font->max_width < font->widths[i]) {
            font->max_width = font->widths[i];
        }
        font->spacing_width += font->widths[i];
    }
    font->spacing_width = getUpperBoundFromDivide32(font->spacing_width, FONT_TABLE_END - FONT_TABLE_START);

    return font;
}

FontCursor * initCursor(uint32_t cur_min_x, uint32_t cur_min_y, uint32_t cur_max_x, uint32_t cur_max_y)
{
    FontCursor* cursor = (FontCursor *)sys_mmap(sizeof (FontCursor));
    cursor->max_x = cur_max_x;
    cursor->max_y = cur_max_y;
    cursor->min_x = cur_min_x;
    cursor->min_y = cur_min_y;
    cursor->cursor_x = cur_min_x;
    cursor->cursor_y = cur_min_y;

    cursor->buffer = (CursorBuffer *)sys_mmap(sizeof (CursorBuffer));
    cursor->buffer->bufferX = (uint32_t *)sys_mmap(sizeof (uint32_t)*CURSOR_BUFFER_SIZE);
    cursor->buffer->bufferY = (uint32_t *)sys_mmap(sizeof (uint32_t)*CURSOR_BUFFER_SIZE);
    cursor->buffer->bufferLogicSize = 0;
    cursor->buffer->iBuffer = 0;

    return cursor;
}

void checkCursor(FontCursor * cursor, FontTable * font)
{
    if (cursor->cursor_x > cursor->max_x - font->max_width) {
        cursor->cursor_y += font->max_height;
        cursor->cursor_x = cursor->min_x;
    }
    if (cursor->cursor_y > cursor->max_y - font->max_height) {

        draw(cursor->min_x, cursor->min_y, cursor->max_x, cursor->max_y, 0, 0, 0);

        cursor->cursor_x = cursor->min_x;
        cursor->cursor_y = cursor->min_y;

        cursor->buffer->bufferLogicSize = 0; // On vide le buffer
    }
}

void goForwardCursor(FontCursor * cursor, FontTable * font, uint32_t width)
{
    // TODO Refactoring de ça
    cursor->buffer->bufferX[cursor->buffer->iBuffer] = cursor->cursor_x;
    cursor->buffer->bufferY[cursor->buffer->iBuffer] = cursor->cursor_y;
    cursor->buffer->iBuffer++;
    if (cursor->buffer->iBuffer >= CURSOR_BUFFER_SIZE) {
        cursor->buffer->iBuffer = 0;
    }
    cursor->buffer->bufferLogicSize++;

    cursor->cursor_x += width; // Letter spacing
    checkCursor(cursor, font);
}

void goBackCursor(FontCursor * cursor, FontTable * font)
{
    // Si le buffer est vide, rien n'est à effacer
    if (cursor->buffer->bufferLogicSize == 0) {
        return;
    }
    else {
        cursor->buffer->bufferLogicSize--;
    }

    CursorBuffer * buffer = cursor->buffer;
    if (cursor->buffer->iBuffer == 0) {
        cursor->buffer->iBuffer = CURSOR_BUFFER_SIZE;
    }
    else {
        cursor->buffer->iBuffer--;
    }
    uint32_t startX = buffer->bufferX[buffer->iBuffer];
    uint32_t startY = buffer->bufferY[buffer->iBuffer];

    if (startY == cursor->cursor_y) { // Même ligne
        draw(startX, startY, cursor->cursor_x, cursor->cursor_y + font->max_height, 0, 0, 0);
    } else {
        draw(startX, startY, cursor->max_x, startY + font->max_height, 0, 0, 0); // Fin ligne du haut
    }

    cursor->cursor_x = buffer->bufferX[buffer->iBuffer];
    cursor->cursor_y = buffer->bufferY[buffer->iBuffer];
}

void drawLetter(FontCursor * cursor, FontTable * font, char letter)
{
    uint32_t asciiValue = (uint32_t) letter;
    if (letter == '\n') { // Retour à la ligne
        // TODO Refactoring de ça
        cursor->buffer->bufferX[cursor->buffer->iBuffer] = cursor->cursor_x;
        cursor->buffer->bufferY[cursor->buffer->iBuffer] = cursor->cursor_y;
        cursor->buffer->iBuffer++;
        if (cursor->buffer->iBuffer >= CURSOR_BUFFER_SIZE) {
            cursor->buffer->iBuffer = 0;
        }
        cursor->buffer->bufferLogicSize++;

        cursor->cursor_y += font->max_height;
        cursor->cursor_x = cursor->min_x;
        checkCursor(cursor, font);

    } else if (letter == ' ') { // Espace

        goForwardCursor(cursor, font, font->spacing_width);

    } else if (letter == '\b'){

        goBackCursor(cursor, font);

    } else if (letter == '\t'){

        drawLetters(cursor, font, "    ");

    } else if (FONT_TABLE_START <= asciiValue && FONT_TABLE_END >= asciiValue) {

        char * bitmapLetter = font->values[asciiValue];
        uint32_t widthLetter = font->widths[asciiValue];
        uint32_t heightLetter = font->heights[asciiValue];
        uint32_t nbBlockPerLine = getUpperBoundFromDivide32(widthLetter, SIZE_OF_BLOCK);

        uint32_t line = 0;
        while(line < heightLetter) {
            uint32_t col = 0;
            while (col < widthLetter) {
                // Vérifie si l'on doit tracer un pixel blanc à cet endroit 
                // Attention : pour les xbm, on ignore les bits qui dépassent de la ligne
                // TODO : faire une explication plus longue dans le readme
                if ((bitmapLetter[line * nbBlockPerLine + divide32(col, SIZE_OF_BLOCK)] >> mod32(col, SIZE_OF_BLOCK)) & 1) { 
                    put_pixel_RGB24(cursor->cursor_x + col, cursor->cursor_y + line, 255, 255, 255);
                } 
                ++col;
            }
            ++line;
        }

        goForwardCursor(cursor, font, widthLetter);

    } else {
        drawLetter(cursor, font, '#');
    }
}

void drawLetters(FontCursor * cursor, FontTable * font, char * letters) {

    for (uint32_t i = 0; letters[i] && letters[i] != '\0'; ++i) {
        drawLetter(cursor, font, letters[i]);
    }

}
