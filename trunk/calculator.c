
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
    {   { "  e\2 ",                  OperatePrecedence,        CALC_OP_EXP             },
        { " ln  ",                   OperatePrecedence,        CALC_OP_LN              },
        { "log ",                  OperatePrecedence,        CALC_OP_LN10            },
        { "  10\2 ",                 OperatePrecedence,        CALC_OP_10X             },
        { "\2\3y ",                OperatePrecedence,        CALC_OP_NROOT           },
        { " y\2  ",                    OperatePrecedence,        CALC_OP_NPOW            },
    },
};


const packedMenu mathModeBase = {
    0, //CALC-STYLE
    printMenu, increment, decrement, 6,
    {   0,0,0,0,
    },
    {   { " Rad ",            OperatePrecedence,        CALC_OP_MODERAD         },
        { " Deg ",            OperatePrecedence,        CALC_OP_MODEDEG         },
        { " BIN ",             OperatePrecedence,        CALC_OP_BASE2            },
        { " OCT ",              OperatePrecedence,        CALC_OP_BASE10            },    //DUMMY
        { " DEC ",              OperatePrecedence,        CALC_OP_BASE10            },
        { " HEX ",            OperatePrecedence,        CALC_OP_BASE16            },
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
        { " sinh\2",            OperatePrecedence,        CALC_OP_HYP_ASIN        },
        { "cosh\2",            OperatePrecedence,        CALC_OP_HYP_ACOS        },
        { "tanh\2",            OperatePrecedence,        CALC_OP_HYP_ATAN        },
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
        { " sin\3",                OperatePrecedence,        CALC_OP_ASIN            },
        { " cos\3",                OperatePrecedence,        CALC_OP_ACOS            },
        { " tan\3",                OperatePrecedence,        CALC_OP_ATAN            },
    },
};

const packedMenu mathBoolean = {
    0,  //CALC-STYLE
    printMenu, increment, decrement, 6,
    {},
    {
        { " AND ", OperatePrecedence, CALC_OP_LOGIC_AND },
        { " OR  ", OperatePrecedence, CALC_OP_LOGIC_OR },
        { " XOR ", OperatePrecedence, CALC_OP_LOGIC_XOR }, 
        { " NOT ", OperatePrecedence, CALC_OP_LOGIC_NOT },
        { " NOR ", OperatePrecedence, CALC_OP_NULL },    //DUMMY
        { " NAND", OperatePrecedence, CALC_OP_NULL },   //DUMMY
    },
};

const packedMenu mathConstant = {
    0, //CALC-STYLE
    printMenu, increment, decrement, 6,
    {   character_pi,character_minus1,
    },
    {   { "    \2",                     OperatePrecedence,        CALC_OP_PI              },
        { "   c ", OperatePrecedence, CALC_OP_LIGHT },
        { " Avog", OperatePrecedence, CALC_OP_AVOGADRO },
        { "    e", OperatePrecedence, CALC_OP_AVOGADRO },
        { "", OperatePrecedence, CALC_OP_NULL },
        { "", OperatePrecedence, CALC_OP_NULL },
    },
};


const packedMenu mathConversion1 = {
    0, //CALC-STYLE
    printMenu, increment, decrement, 6,
    {   0, character_arrow,
    },
    {   { "mils\3mm",           OperatePrecedence,       CALC_OP_CONV_MILS_TO_MM },
        { "  ",           OperatePrecedence,       CALC_OP_NULL},
        { "mm\3mils", OperatePrecedence, CALC_OP_CONV_MM_TO_MILS },
        { " \337F\3\337C", OperatePrecedence, CALC_OP_CONV_FC },
        { "   ",           OperatePrecedence,       CALC_OP_NULL},
        { " \337C\3\337F", OperatePrecedence, CALC_OP_CONV_CF },
    },
};


const packedMenu mathConversion2 = {
    0, //CALC-STYLE
    printMenu, increment, decrement, 6,
    {   0, character_arrow,
    },
    {   { " kg\3lb", OperatePrecedence, CALC_OP_CONV_KG_LB },
        { "   ",           OperatePrecedence,       CALC_OP_NULL},
        { "lb\3kg", OperatePrecedence, CALC_OP_CONV_LB_KG },
        { "Rec\3Pol",          OperatePrecedence,        CALC_OP_R2P              },
        { "",           OperatePrecedence,       CALC_OP_NULL},
        { " Pol\3Rec",          OperatePrecedence,        CALC_OP_P2R              },
    },
};

const packedMenu mathTimeDate = {
    0, //CALC-STYLE
    printMenu, increment, decrement, 6,
    {},
    {   { "Sunset",               OperatePrecedence,        CALC_OP_SUNSET          },
        { " Days",                 OperatePrecedence,        CALC_OP_DAYS            },
        { " DMY ",     OperatePrecedence,        CALC_OP_DMY             },
        { " HMS ",     OperatePrecedence,        CALC_OP_HMS             },
        { "  Hour",              OperatePrecedence,        CALC_OP_HOURS           },
        { "",           OperatePrecedence,       CALC_OP_NULL},
    },
};

/*const packedMenu menuKeystroke;

const packedMenu menuKeystroke2 = {
    0,  //CALC-STYLE,
    printMenu, increment, decrement, 6,
    {   left,right,0,0,
    },
    {   { "HOLY",                 OperatePrecedence,        CALC_OP_RECORD          },
        { "",           OperatePrecedence,       CALC_OP_NULL},
        { "SMOKE",                   OperatePrecedence,        CALC_OP_PLAY            },
        { "2\2\3",           doCalcMenu, (int)(&menuKeystroke) },
        { "",           OperatePrecedence,       CALC_OP_NULL},
        { "",           OperatePrecedence,       CALC_OP_NULL},
    },
};
*/
const packedMenu menuKeystroke = {
    0, //CALC-STYLE,
    printMenu, increment, decrement, 6,
    {},
    {   { "Record",                 OperatePrecedence,        CALC_OP_RECORD          },
        { "   ",           OperatePrecedence,       CALC_OP_NULL},
        { "  Play",                   OperatePrecedence,        CALC_OP_PLAY            },
//        { "2\2\3",           doCalcMenu, (int)(&menuKeystroke2) },
        { "",           OperatePrecedence,       CALC_OP_NULL},
        { "",           OperatePrecedence,       CALC_OP_NULL},
        { "",           OperatePrecedence,       CALC_OP_NULL},
    },
};




const packedMenu mathOther = {
    0, //CALC-STYLE
    printMenu, increment, decrement, 6,
    {   character_powerx,               //2
        character_squaring,             //3
    },
    {   { " 1/x ",                    OperatePrecedence,        CALC_OP_RECIPROCAL      },
        { "  x! ",                     OperatePrecedence,        CALC_OP_FACTORIAL       },
        { "  x\3 ",                    OperatePrecedence,        CALC_OP_SQUARE          },
        { " sqrt",                    OperatePrecedence,        CALC_OP_SQRT            },
        { " abs ",                    OperatePrecedence,        CALC_OP_ABS             },
        { "",           OperatePrecedence,       CALC_OP_NULL},
    },
};




const packedMenu mathComplex = {
    0,  //CALC-STYLE
    printMenu, increment, decrement, 6,
    {   character_right_menu,
    },
    {   { " Con ", OperatePrecedence,        CALC_OP_CONV            },     //???
        { "  // ", OperatePrecedence,        CALC_OP_PARALLEL        },     //???
        { "Split", OperatePrecedence,        CALC_OP_COMPLEX_SPLIT   },
        { " Join", OperatePrecedence,        CALC_OP_COMPLEX_JOIN            },
        { " Conj", OperatePrecedence,        CALC_OP_CONJUGATE       },
        { " Real", OperatePrecedence,        CALC_OP_REAL_PART            },

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
