#include "characterset.h"

// Custom character symbols for menus

const unsigned char character_base2[] = { 0x00, 0x00, 0x00, 0x18, 0x04, 0x08, 0x10, 0x1c };
const unsigned char character_base8[] = { 0x00, 0x00, 0x00, 0x0C, 0x14, 0x1C, 0x14, 0x18 };
const unsigned char character_base16[] = { 0x00, 0x00, 0x00, 0x13, 0x14, 0x17, 0x15, 0x16 };

const unsigned char character_left_menu[] = { 0x02, 0x06, 0x0E, 0x1E, 0x0E, 0x06, 0x02, 0x00 };
const unsigned char character_right_menu[] = { 0x08, 0x0C, 0x0E, 0x0F, 0x0E, 0x0C, 0x08, 0x00 };

const unsigned char character_squaring[] = { 0x0E, 0x01, 0x06, 0x08, 0x0F, 0x00, 0x00, 0x00 };  // as in x^2
const unsigned char character_squareRoot1[] = { 0x07, 0x04, 0x04, 0x04, 0x04, 0x04, 0x14, 0x08 };
//const unsigned char character_squareRoot2[] = { 0x1f, 0x00, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x00 };
const unsigned char character_powerx[] = { 0x0A, 0x04, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00 };
const unsigned char character_pi[] = { 0x00, 0x1F, 0x0A, 0x0A, 0x0A, 0x0A, 0x12, 0x00 };
//const unsigned char character_nd1[] = { 0x00, 0x0E, 0x09, 0x09, 0x09, 0x00, 0x00, 0x00 };
//const unsigned char character_nd2[] = { 0x02, 0x0E, 0x12, 0x12, 0x0E, 0x00, 0x00, 0x00 };
const unsigned char character_minus1[] = { 0x04, 0x14, 0x04, 0x00, 0x0, 0x00, 0x00, 0x00 };
//const unsigned char character_minus1b[] = { 0x04, 0x04, 0x14, 0x04, 0x04, 0x00, 0x00, 0x00 };
const unsigned char character_arrow[] = { 0x00, 0x04, 0x02, 0x1F, 0x02, 0x04, 0x00, 0x00 };
//const unsigned char character_submenu[] = { 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
//const unsigned char character_1x1[] = { 0x11, 0x12, 0x12, 0x14, 0x14, 0x00, 0x00, 0x00 };
//const unsigned char character_1x2[] = { 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x00, 0x00, 0x00 };
//const unsigned char degree[] = { 0x06, 0x09, 0x09, 0x06, 0x00, 0x00, 0x00, 0x00 };
const unsigned char characterDST[] = { 0x00, 0x04, 0x1F, 0x0E, 0x0A, 0x00, 0x00, 0x00 };
//const unsigned char characterMenu[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x0E, 0x04 };


const unsigned char AMPM[][8] = {
    { 0x02, 0x05, 0x09, 0x0f, 0x09, 0x00, 0x00, 0x00 },  // "A"
    { 0x0E, 0x09, 0x09, 0x0E, 0x08, 0x00, 0x00, 0x00 },  // "P"
    { 0x11, 0x1B, 0x15, 0x11, 0x11, 0x00, 0x00, 0x00 }  // "M"
};

//const unsigned char characterEllipsis[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x1B, 0x1B, 0x00 };
//const unsigned char characterEllipsis2[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00 };
//const unsigned char characterEnter[] = { 0x18, 0x04, 0x02, 0x02, 0x12, 0x14, 0x18, 0x1E };

const unsigned char left[] = {

    0x00, 0x0E, 0x09, 0x09, 0x09, 0x00, 0x00, 0x00,  // nd
    0x04, 0x0E, 0x04, 0x04, 0x03, 0x00, 0x00, 0x00,  // th
    0x06, 0x08, 0x07, 0x01, 0x0E, 0x00, 0x00, 0x00,  // st
    0x00, 0x07, 0x04, 0x04, 0x04, 0x00, 0x00, 0x00,  // rd
};

const unsigned char right[] = {

    0x02, 0x0E, 0x12, 0x12, 0x0E, 0x00, 0x00, 0x00,  //nd
    0x10, 0x1C, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00,  //th
    0x08, 0x1C, 0x08, 0x08, 0x06, 0x00, 0x00, 0x00,  //st
    0x02, 0x0E, 0x12, 0x12, 0x0E, 0x00, 0x00, 0x00,  //rd
};

const unsigned char character_Monday[] = { 0x11, 0x1B, 0x15, 0x11, 0x11, 0x00, 0x00, 0x00 };
const unsigned char character_Tuesday[] = { 0x1F, 0x04, 0x04, 0x04, 0x04, 0x00, 0x00, 0x00 };
const unsigned char character_Wednesday[] = { 0x11, 0x11, 0x15, 0x1B, 0x11, 0x00, 0x00, 0x00 };
const unsigned char character_Thursday[] = { 0x1F, 0x04, 0x04, 0x04, 0x05, 0x00, 0x00, 0x00 };
const unsigned char character_Friday[] = { 0x1F, 0x10, 0x1E, 0x10, 0x10, 0x00, 0x00, 0x00 };
const unsigned char character_Saturday[] = { 0x0E, 0x10, 0x0E, 0x01, 0x1E, 0x00, 0x00, 0x00 };
const unsigned char character_Sunday[] = { 0x0E, 0x10, 0x0E, 0x01, 0x1E, 0x00, 0x1F, 0x00 };

/*
const unsigned char character_bold0[] = { 0x0E, 0x13, 0x13, 0x13, 0x13, 0x0E, 0x00, 0x1F };
const unsigned char character_bold1[] = { 0x06, 0x0E, 0x06, 0x06, 0x06, 0x06, 0x00, 0x1F };
const unsigned char character_bold2[] = { 0x0E, 0x13, 0x03, 0x0E, 0x18, 0x1F, 0x00, 0x1F };
const unsigned char character_bold3[] = { 0x0E, 0x03, 0xE, 0x03, 0x03, 0x1E, 0x00, 0x1F };
const unsigned char character_bold4[] = { 0x07, 0x0B, 0x13, 0x13, 0x1F, 0x03, 0x00, 0x1F };
const unsigned char character_bold5[] = { 0x1F, 0x18, 0x1E, 0x03, 0x13, 0x0E, 0x00, 0x1F };
const unsigned char character_bold6[] = { 0x0E, 0x18, 0x1F, 0x19, 0x19, 0x0E, 0x00, 0x1F };
const unsigned char character_bold7[] = { 0x1F, 0x03, 0x06, 0x0C, 0x0C, 0x0C, 0x00, 0x1F };
const unsigned char character_bold8[] = { 0x0E, 0x13, 0x0E, 0x13, 0x13, 0x0E, 0x00, 0x1F };
const unsigned char character_bold9[] = { 0x0E, 0x13, 0x13, 0x1F, 0x03, 0x03, 0x00, 0x1F };
const unsigned char character_boldSpace[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F };
*/

const unsigned char character_arrow_down[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x0E, 0x04 };
const unsigned char character_arrow_up[] = { 0x04, 0x0E, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00 };
const unsigned char character_arrow_updown[] = { 0x04, 0x0E, 0x1F, 0x00, 0x00, 0x1F, 0x0E, 0x04 };

