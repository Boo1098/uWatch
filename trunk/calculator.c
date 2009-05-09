
#include "uWatch-op.h"
#include "calculator.h"
#include "menu.h"
#include "characterset.h"


//int doCalcMenu( int op )
//{
//    return genericMenu2( (packedMenu *)op, 0 );
//}



/////////////////////////////////////////////////////////////////////////////////////////

const packedMenu mathLogarithmic = {
    0, //CALC-STYLE
    printMenu, increment, decrement, 6,
    {   character_powerx,character_squareRoot1,
    },
    {   { "  e\4 ",                  OperatePrecedence,        CALC_OP_EXP             },
        { " ln  ",                   OperatePrecedence,        CALC_OP_LN              },
        { "log ",                  OperatePrecedence,        CALC_OP_LN10            },
        { "  10\4 ",                 OperatePrecedence,        CALC_OP_10X             },
        { "\4\5y ",                OperatePrecedence,        CALC_OP_NROOT           },
        { " y\4  ",                    OperatePrecedence,        CALC_OP_NPOW            },
    },
};


const packedMenu mathModeBase = {
    0, //CALC-STYLE
    printMenu, increment, decrement, 6,
    {},
    {   { "  rad ",            OperatePrecedence,        CALC_OP_MODERAD         },
        { "deg ",            OperatePrecedence,        CALC_OP_MODEDEG         },
        { "bin",              OperatePrecedence,        CALC_OP_BASE2            },
        { "  oct",             OperatePrecedence,        CALC_OP_BASE8            },
        { " dec ",              OperatePrecedence,        CALC_OP_BASE10            },
        { "hex",            OperatePrecedence,        CALC_OP_BASE16            },
    },
};


const packedMenu mathHyperbolic = {
    0,  //CALC-STYLE
    printMenu, increment, decrement, 6,
    {   character_minus1,
    },
    {   { " sinh",                OperatePrecedence,        CALC_OP_HYP_SIN         },
        { " cosh",                OperatePrecedence,        CALC_OP_HYP_COS         },
        { " tanh",                OperatePrecedence,        CALC_OP_HYP_TAN         },
        { " sinh\4",            OperatePrecedence,        CALC_OP_HYP_ASIN        },
        { "cosh\4",            OperatePrecedence,        CALC_OP_HYP_ACOS        },
        { "tanh\4",            OperatePrecedence,        CALC_OP_HYP_ATAN        },
    },
};


const packedMenu mathTrigonometry = {
    0,  //CALC-STYLE
    printMenu, increment, decrement, 6,
    {   0,character_minus1,
    },
    {   { " sin ",                    OperatePrecedence,        CALC_OP_SIN             },
        { " cos ",                    OperatePrecedence,        CALC_OP_COS             },
        { " tan ",                    OperatePrecedence,        CALC_OP_TAN             },
        { " sin\5",                OperatePrecedence,        CALC_OP_ASIN            },
        { " cos\5",                OperatePrecedence,        CALC_OP_ACOS            },
        { " tan\5",                OperatePrecedence,        CALC_OP_ATAN            },
    },
};

const packedMenu mathBoolean = {
    0,  //CALC-STYLE
    printMenu, increment, decrement, 6,
    {},
    {
        { "  and ", OperatePrecedence, CALC_OP_LOGIC_AND },
        { " or ", OperatePrecedence, CALC_OP_LOGIC_OR },
        { " xor", OperatePrecedence, CALC_OP_LOGIC_XOR }, 
        { " nand", OperatePrecedence, CALC_OP_LOGIC_NAND },
        { " nor", OperatePrecedence, CALC_OP_LOGIC_NOR },
        { "  not", OperatePrecedence, CALC_OP_LOGIC_NOT },
    },
};

const packedMenu mathConstant = {
    0, //CALC-STYLE
    printMenu, increment, decrement, 6,
    {   character_pi,character_minus1,
    },
    {   { "    \4",                     OperatePrecedence,        CALC_OP_PI              },
        { "  c ", OperatePrecedence, CALC_OP_LIGHT },
        { " Avo ", OperatePrecedence, CALC_OP_AVOGADRO },
        { "    e", OperatePrecedence, CALC_OP_E },
        { "", OperatePrecedence, CALC_OP_NULL },
        { "", OperatePrecedence, CALC_OP_NULL },
    },
};


const packedMenu mathConversion1 = {
    0, //CALC-STYLE
    printMenu, increment, decrement, 6,
    {   0, character_arrow,
    },
    {   { "mils\5mm",           OperatePrecedence,       CALC_OP_CONV_MILS_TO_MM },
        { "  ",           OperatePrecedence,       CALC_OP_NULL},
        { "mm\5mils", OperatePrecedence, CALC_OP_CONV_MM_TO_MILS },
        { " \337F\5\337C", OperatePrecedence, CALC_OP_CONV_FC },
        { "   ",           OperatePrecedence,       CALC_OP_NULL},
        { " \337C\5\337F", OperatePrecedence, CALC_OP_CONV_CF },
    },
};


const packedMenu mathConversion2 = {
    0, //CALC-STYLE
    printMenu, increment, decrement, 6,
    {   0, character_arrow,
    },
    {   { " kg\5lb", OperatePrecedence, CALC_OP_CONV_KG_LB },
        { "   ",           OperatePrecedence,       CALC_OP_NULL},
        { " lb\5kg", OperatePrecedence, CALC_OP_CONV_LB_KG },
        { "rec\5pol",          OperatePrecedence,        CALC_OP_R2P              },
        { "",           OperatePrecedence,       CALC_OP_NULL},
        { "  pol\5rec",          OperatePrecedence,        CALC_OP_P2R              },
    },
};

const packedMenu mathTimeDate = {
    0, //CALC-STYLE
    printMenu, increment, decrement, 6,
    {},
    {
        { "  DMY",     OperatePrecedence,        CALC_OP_DMY             },
        { " days ",                 OperatePrecedence,        CALC_OP_DAYS            },
        { "sunst",               OperatePrecedence,        CALC_OP_SUNSET          },
        { "  HMS ",     OperatePrecedence,        CALC_OP_HMS             },
        { "hour",              OperatePrecedence,        CALC_OP_HOURS           },
        { "",           OperatePrecedence,       CALC_OP_NULL},
    },
};


const packedMenu menuKeystroke = {
    0, //CALC-STYLE,
    printMenu, increment, decrement, 6,
    {},
    {   { "record",                 OperatePrecedence,        CALC_OP_RECORD          },
        { "   ",           OperatePrecedence,       CALC_OP_NULL},
        { "  play",                   OperatePrecedence,        CALC_OP_PLAY            },
//        { "2\2\3",           doCalcMenu, (int)(&menuKeystroke2) },
        { "",           OperatePrecedence,       CALC_OP_NULL},
        { "",           OperatePrecedence,       CALC_OP_NULL},
        { "",           OperatePrecedence,       CALC_OP_NULL},
    },
};




const packedMenu mathOther = {
    0, //CALC-STYLE
    printMenu, increment, decrement, 6,
    {   character_powerx,               //4
        character_squaring,             //5
    },
    {   { "  1/x ",                    OperatePrecedence,        CALC_OP_RECIPROCAL      },
        { " x!",                     OperatePrecedence,        CALC_OP_FACTORIAL       },
        { "  x\5",                    OperatePrecedence,        CALC_OP_SQUARE          },
        { " sqrt",                    OperatePrecedence,        CALC_OP_SQRT            },
        { " abs ",                    OperatePrecedence,        CALC_OP_ABS             },
        { "",           OperatePrecedence,       CALC_OP_NULL},
    },
};




const packedMenu mathComplex = {
    0,  //CALC-STYLE
    printMenu, increment, decrement, 6,
    {},
    {   { " conv", OperatePrecedence,        CALC_OP_CONV            },     //???
        { "  // ", OperatePrecedence,        CALC_OP_PARALLEL        },     //???
        { " split", OperatePrecedence,        CALC_OP_COMPLEX_SPLIT   },
        { " join", OperatePrecedence,        CALC_OP_COMPLEX_JOIN            },
        { " conj", OperatePrecedence,        CALC_OP_CONJUGATE       },
        { " real", OperatePrecedence,        CALC_OP_REAL_PART            },

    },
};





const packedMenu *calcMenus[CALC_MENU_SIZE] = {
    &mathOther,
    &mathTrigonometry,
    &mathHyperbolic,
    &mathBoolean,
    &mathModeBase,
    &mathLogarithmic,
    &mathConstant,
    &mathConversion1,
    &mathConversion2,
    &mathTimeDate,
    &mathComplex,
    &menuKeystroke,

};    
