
#include "uWatch-op.h"
#include "calculator.h"
#include "menu.h"
#include "characterset.h"



/////////////////////////////////////////////////////////////////////////////////////////


const charSet mathLogCharset[] = {
    character_powerx,
    character_squareRoot1,
};

const menuItem mathLogMenu[] = {
    { "  e\4 ",     OperatePrecedence,  CALC_OP_EXP     },
    { " ln  ",      OperatePrecedence,  CALC_OP_LN      },
    { "log ",       OperatePrecedence,  CALC_OP_LN10    },
    { "  10\4 ",    OperatePrecedence,  CALC_OP_10X     },
    { "\4\5y ",     OperatePrecedence,  CALC_OP_NROOT   },
    { " y\4  ",     OperatePrecedence,  CALC_OP_NPOW    },
};


const packedMenu2 mathLogarithmic = {
    0, //CALC-STYLE
    printMenu,
    2, mathLogCharset, 6, mathLogMenu
};

const menuItem mathBaseMenu[] = {
    { "  rad ", OperatePrecedence,  CALC_OP_MODERAD     },
    { "deg ",   OperatePrecedence,  CALC_OP_MODEDEG     },
    { "bin",    OperatePrecedence,  CALC_OP_BASE2       },
    { "  oct",  OperatePrecedence,  CALC_OP_BASE8       },
    { " dec ",  OperatePrecedence,  CALC_OP_BASE10      },
    { "hex",    OperatePrecedence,  CALC_OP_BASE16      },
};

const packedMenu2 mathModeBase = {
    0, //CALC-STYLE
    printMenu,
    0, 0, 6, mathBaseMenu
};

const menuItem mathHyperMenu[] = {
    { " sinh",      OperatePrecedence,  CALC_OP_HYP_SIN     },
    { " cosh",      OperatePrecedence,  CALC_OP_HYP_COS     },
    { " tanh",      OperatePrecedence,  CALC_OP_HYP_TAN     },
    { " sinh\4",    OperatePrecedence,  CALC_OP_HYP_ASIN    },
    { "cosh\4",     OperatePrecedence,  CALC_OP_HYP_ACOS    },
    { "tanh\4",     OperatePrecedence,  CALC_OP_HYP_ATAN    },
};


const charSet mathTrigCharset[] = {
    character_minus1
};

const menuItem mathTrigMenu[] = {
    { " sin ",  OperatePrecedence,  CALC_OP_SIN    },
    { " cos ",  OperatePrecedence,  CALC_OP_COS    },
    { " tan ",  OperatePrecedence,  CALC_OP_TAN    },
    { " sin\4", OperatePrecedence,  CALC_OP_ASIN   },
    { " cos\4", OperatePrecedence,  CALC_OP_ACOS   },
    { " tan\4", OperatePrecedence,  CALC_OP_ATAN   },
};

const packedMenu2 mathHyperbolic = {
    0,  //CALC-STYLE
    printMenu,
    1, mathTrigCharset, 6, mathHyperMenu
};

const packedMenu2 mathTrigonometry = {
    0,  //CALC-STYLE
    printMenu,
    1, mathTrigCharset, 6, mathTrigMenu
};


const menuItem mathBooleanMenu[] = {
    { "  and ", OperatePrecedence, CALC_OP_LOGIC_AND    },
    { " or ",   OperatePrecedence, CALC_OP_LOGIC_OR     },
    { " xor",   OperatePrecedence, CALC_OP_LOGIC_XOR    }, 
    { " nand",  OperatePrecedence, CALC_OP_LOGIC_NAND   },
    { " nor",   OperatePrecedence, CALC_OP_LOGIC_NOR    },
    { "  not",  OperatePrecedence, CALC_OP_LOGIC_NOT    },
};

const packedMenu2 mathBoolean = {
    0,  //CALC-STYLE
    printMenu,
    0, 0, 6, mathBooleanMenu
};

const menuItem mathCombMenu[] = {
    { " nPr ",  OperatePrecedence, CALC_OP_PERMUTATION  },
    { " nCr ",  OperatePrecedence, CALC_OP_COMBINATION  },
    { "", OperatePrecedence, CALC_OP_NULL },
    { "", OperatePrecedence, CALC_OP_NULL },
    { "", OperatePrecedence, CALC_OP_NULL },
    { "", OperatePrecedence, CALC_OP_NULL },
};

const packedMenu2 mathComb = {
    0, //CALC-STYLE
    printMenu,
    0, 0, 6, mathCombMenu
};

const charSet mathStatsCharset[] = {
    character_squaring
};

const menuItem mathStatsMenu[] = {
    { " clx ",      OperatePrecedence, CALC_OP_STAT_CLX     },
    { " \366x  ",   OperatePrecedence, CALC_OP_STAT_SIGMAX  },
    { " \366x\4 ",  OperatePrecedence, CALC_OP_STAT_SIGMAX2 },
    { "  \345  ",   OperatePrecedence, CALC_OP_STAT_SD      },
    { "  \370 ",    OperatePrecedence, CALC_OP_STAT_MEAN    },
    { "  \366+ ",   OperatePrecedence, CALC_OP_STAT_ADD     },
};

const packedMenu2 mathStats = {
    0, //CALC-STYLE
    printMenu,
    1, mathStatsCharset, 6, mathStatsMenu
};

     

const charSet mathConstantCharset[] = {
    character_pi,character_minus1
};

const menuItem mathConstantMenu[] = {
    { "    \4", OperatePrecedence, CALC_OP_PI       },
    { "  c ",   OperatePrecedence, CALC_OP_LIGHT    },
    { " Avo ",  OperatePrecedence, CALC_OP_AVOGADRO },
    { "    e",  OperatePrecedence, CALC_OP_E        },
    { "", OperatePrecedence, CALC_OP_NULL },
    { "", OperatePrecedence, CALC_OP_NULL },
};

const packedMenu2 mathConstant = {
    0, //CALC-STYLE
    printMenu,
    2, mathConstantCharset, 6, mathConstantMenu
};

const menuItem mathConversion1Menu[] = {
    { "mils\4mm",       OperatePrecedence, CALC_OP_CONV_MILS_TO_MM  },
    { "  ", OperatePrecedence, CALC_OP_NULL },
    { "mm\4mils",       OperatePrecedence, CALC_OP_CONV_MM_TO_MILS  },
    { " \337F\4\337C",  OperatePrecedence, CALC_OP_CONV_FC          },
    { "  ", OperatePrecedence, CALC_OP_NULL },
    { " \337C\4\337F",  OperatePrecedence, CALC_OP_CONV_CF          },
};

const charSet mathConversion2Charset[] = {
    character_arrow
};

const menuItem mathConversion2Menu[] = {
    { " kg\4lb",    OperatePrecedence,  CALC_OP_CONV_KG_LB  },
    { "   ", OperatePrecedence, CALC_OP_NULL },
    { " lb\4kg",    OperatePrecedence,  CALC_OP_CONV_LB_KG  },
    { "rec\4pol",   OperatePrecedence,  CALC_OP_R2P         },
    { "", OperatePrecedence, CALC_OP_NULL },
    { "  pol\4rec", OperatePrecedence,  CALC_OP_P2R         },
};

const packedMenu2 mathConversion1 = {
    0, //CALC-STYLE
    printMenu,
    1, mathConversion2Charset, 6, mathConversion1Menu
};

const packedMenu2 mathConversion2 = {
    0, //CALC-STYLE
    printMenu,
    1, mathConversion2Charset, 6, mathConversion2Menu
};


const menuItem mathTimeDateMenu[] = {
    { "  DMY",  OperatePrecedence,  CALC_OP_DMY             },
    { " days ", OperatePrecedence,  CALC_OP_DAYS            },
    { "sunst",  OperatePrecedence,  CALC_OP_SUNSET          },
    { "  HMS ", OperatePrecedence,  CALC_OP_HMS             },
    { "hour",   OperatePrecedence,  CALC_OP_HOURS           },
    { "", OperatePrecedence, CALC_OP_NULL },
};

const packedMenu2 mathTimeDate = {
    0, //CALC-STYLE
    printMenu,
    0, 0, 6, mathTimeDateMenu
};


const menuItem menuKeystrokeMenu[] = {
    { "record",  OperatePrecedence,  CALC_OP_RECORD          },
    { "", OperatePrecedence, CALC_OP_NULL },
    { "  play",  OperatePrecedence,  CALC_OP_PLAY            },
    { "", OperatePrecedence, CALC_OP_NULL },
    { "", OperatePrecedence, CALC_OP_NULL },
    { "", OperatePrecedence, CALC_OP_NULL },
};

const packedMenu2 menuKeystroke = {
    0, //CALC-STYLE,
    printMenu,
    0, 0, 6, menuKeystrokeMenu 
};



const charSet mathOtherCharset[] = {
    character_squaring,             //4
};

const menuItem mathOtherMenu[] = {
    { "  1/x ",  OperatePrecedence, CALC_OP_RECIPROCAL     },
    { " x!",     OperatePrecedence, CALC_OP_FACTORIAL      },
    { "  x\4",   OperatePrecedence, CALC_OP_SQUARE         },
    { " sqrt",   OperatePrecedence, CALC_OP_SQRT           },
    { " abs ",   OperatePrecedence, CALC_OP_ABS            },
    { "", OperatePrecedence, CALC_OP_NULL },
};

const packedMenu2 mathOther = {
    0, //CALC-STYLE
    printMenu,
    1, mathOtherCharset, 6, mathOtherMenu
};



const menuItem mathComplexMenu[] = {
    { " conv",  OperatePrecedence, CALC_OP_CONV            },     //???
    { "  // ",  OperatePrecedence, CALC_OP_PARALLEL        },     //???
    { " split", OperatePrecedence, CALC_OP_COMPLEX_SPLIT   },
    { " join",  OperatePrecedence, CALC_OP_COMPLEX_JOIN    },
    { " conj",  OperatePrecedence, CALC_OP_CONJUGATE       },
    { " real",  OperatePrecedence, CALC_OP_REAL_PART       },
};

const packedMenu2 mathComplex = {
    0,  //CALC-STYLE
    printMenu,
    0, 0, 6, mathComplexMenu
};


const packedMenu2 *calcMenus[CALC_MENU_SIZE] = {
    &mathOther,
    &mathTrigonometry,
    &mathHyperbolic,
    &mathBoolean,
    &mathModeBase,
    &mathComb,
    &mathStats,
    &mathLogarithmic,
    &mathConstant,
    &mathConversion1,
    &mathConversion2,
    &mathTimeDate,
    &mathComplex,
    &menuKeystroke,
};    
