
#include "uWatch-op.h"
#include "calculator.h"
#include "menu.h"
#include "characterset.h"


int doMenu( int op )
{
    return genericMenu2( (packedMenu *)op, 0 );
}



const packedMenu menuConstant = {
    "Constant:",
    printMenu, increment, decrement, 1,
    {   character_pi,0,0,0,
    },
    {   { "\2 pi",                     OperatePrecedence,        CALC_OP_PI              },
        // e,phi, ... ?
    },
};

const packedMenu menuKeystroke = {
    "Keystroke:",
    printMenu, increment, decrement, 2,
    {   0,0,0,0,
    },
    {   { "Record",                 OperatePrecedence,        CALC_OP_RECORD          },
        { "Play",                   OperatePrecedence,        CALC_OP_PLAY            },
    },
};

const packedMenu menuConversion = {
    "Convert:",
    printMenu, increment, decrement, 10,
    {   degree, character_arrow, 0,0,
    },
    {   { "\3Hour Min Sec",     OperatePrecedence,        CALC_OP_HMS             },
        { "Rect\3Polar",          OperatePrecedence,        CALC_OP_R2P              },
        { "Polar\3Rect",          OperatePrecedence,        CALC_OP_P2R              },
        { "\3Day Mnth Year",     OperatePrecedence,        CALC_OP_DMY             },
        { "mils \3 mm",           OperatePrecedence,       CALC_OP_CONV_MILS_TO_MM },
        { "mm \3 mils", OperatePrecedence, CALC_OP_CONV_MM_TO_MILS },
        { "\2F \3 \2C", OperatePrecedence, CALC_OP_CONV_FC },
        { "\2C \3 \2F", OperatePrecedence, CALC_OP_CONV_CF },
        { "kg \3 lb", OperatePrecedence, CALC_OP_CONV_KG_LB },
        { "lb \3 kg", OperatePrecedence, CALC_OP_CONV_LB_KG },
    },
};

const packedMenu menuLogarithmic = {
    "Logarithmic:",
    printMenu, increment, decrement, 4,
    {   character_powerx,0,0,0,
    },
    {   { "e\2",                  OperatePrecedence,        CALC_OP_EXP             },
        { "ln",                   OperatePrecedence,        CALC_OP_LN              },
        { "log",                  OperatePrecedence,        CALC_OP_LN10            },
        { "10\2",                 OperatePrecedence,        CALC_OP_10X             },
    },
};

const packedMenu menuTimeDayDate = {
    "Time/Day/Date:",
    printMenu, increment, decrement, 2,
    {   0,0,0,0,
    },
    {   { "Sunset",               OperatePrecedence,        CALC_OP_SUNSET          },
        { "Days",                 OperatePrecedence,        CALC_OP_DAYS            },            // what does this do?
        { "Hours",              OperatePrecedence,        CALC_OP_HOURS           },
    },
};

const packedMenu menuBaseChange = {
    "Mode/Base:",
    printMenu, increment, decrement, 5,
    {   character_arrow,0,0,0,
    },
    {   { "\2Radians",            OperatePrecedence,        CALC_OP_MODERAD         },
        { "\2Degrees",            OperatePrecedence,        CALC_OP_MODEDEG         },
        { "\2Binary",             OperatePrecedence,        CALC_OP_BASE2            },
        { "\2Decimal",              OperatePrecedence,        CALC_OP_BASE10            },
        { "\2Hexadecimal",            OperatePrecedence,        CALC_OP_BASE16            },
    },
};

const packedMenu menuBitwise = {
    "Bitwise:",
    printMenu, increment, decrement, 4,
    {   character_arrow,0,0,0,
    },
    {   { "AND", OperatePrecedence, CALC_OP_LOGIC_AND },
        { "OR", OperatePrecedence, CALC_OP_LOGIC_OR },
        { "XOR", OperatePrecedence, CALC_OP_LOGIC_XOR }, 
        { "NOT", OperatePrecedence, CALC_OP_LOGIC_NOT },
    },
};

const packedMenu menuHyperbolic = {
    "Hyperbolic Trig:",
    printMenu, increment, decrement, 6,
    {   character_minus1,0,0,0,
    },
    {
        { "hyp-sin",                OperatePrecedence,        CALC_OP_HYP_SIN         },
        { "hyp-cos",                OperatePrecedence,        CALC_OP_HYP_COS         },
        { "hyp-tan",                OperatePrecedence,        CALC_OP_HYP_TAN         },
        { "hyp-sin\2",            OperatePrecedence,        CALC_OP_HYP_ASIN        },
        { "hyp-tan\2",            OperatePrecedence,        CALC_OP_HYP_ATAN        },
        { "hyp-cos\2",            OperatePrecedence,        CALC_OP_HYP_ACOS        },
    },
};

const packedMenu menuTrigonometry = {
    "Trigonometry:",
    printMenu, increment, decrement, 7,
    {   characterMenu,
        character_minus1,0,0,
    },
    {
        { "sin",                    OperatePrecedence,        CALC_OP_SIN             },
        { "cos",                    OperatePrecedence,        CALC_OP_COS             },
        { "tan",                    OperatePrecedence,        CALC_OP_TAN             },
        { "sin\3",                OperatePrecedence,        CALC_OP_ASIN            },
        { "tan\3",                OperatePrecedence,        CALC_OP_ATAN            },
        { "cos\3",                OperatePrecedence,        CALC_OP_ACOS            },
        { "\2Hyperbolic",           doMenu,         (int)&menuHyperbolic          },
    },
};

const packedMenu menuOther = {
    "Assorted:",
    printMenu, increment, decrement, 7,
    {   character_powerx,               //2
        character_squareRoot1,          //3
        character_squaring,             //4
        0,
    },
    {   { "1/x",                    OperatePrecedence,        CALC_OP_RECIPROCAL      },
        { "x!",                     OperatePrecedence,        CALC_OP_FACTORIAL       },
        { "x\4",                    OperatePrecedence,        CALC_OP_SQUARE          },
        { "\3x",                    OperatePrecedence,        CALC_OP_SQRT            },
        { "abs",                    OperatePrecedence,        CALC_OP_ABS             },
        { "y\2",                    OperatePrecedence,        CALC_OP_NPOW            },
        { "y^(1/x)",                OperatePrecedence,        CALC_OP_NROOT           },
    },
};

const packedMenu menuComplex = {
    "Complex:",
    printMenu, increment, decrement, 6,
    {   0,0,0,0,
    },
    {   { "Conv",                   OperatePrecedence,        CALC_OP_CONV            },     //???
        { "// Parallel",            OperatePrecedence,        CALC_OP_PARALLEL        },     //???
        { "Split",                  OperatePrecedence,        CALC_OP_COMPLEX_SPLIT   },
        { "Join",                   OperatePrecedence,        CALC_OP_COMPLEX_JOIN            },
        { "Conjugate",              OperatePrecedence,        CALC_OP_CONJUGATE       },
        { "Real Part",              OperatePrecedence,        CALC_OP_REAL_PART            },
    },
};


const packedMenu mathMenu = {
    "Function:",
    printMenu, increment, decrement, 10,
    {   characterMenu,0,0,0,
    },
    {   { "\2Trigonometry",       doMenu,         (int)&menuTrigonometry        },
        { "\2Logarithmic",        doMenu,         (int)&menuLogarithmic         },
        { "\2Complex",            doMenu,         (int)&menuComplex             },
        { "\2Mode/Base",          doMenu,         (int)&menuBaseChange          },
        { "\2Constants",          doMenu,         (int)&menuConstant            },
        { "\2Conversion",         doMenu,         (int)&menuConversion          },
        { "\2Time/Day/Date",      doMenu,         (int)&menuTimeDayDate         },
        { "\2Keystroke",          doMenu,         (int)&menuKeystroke           },
        { "\2Assorted",           doMenu,         (int)&menuOther               },
        { "\2Bit Operations", doMenu, (int)&menuBitwise },
    },
};

