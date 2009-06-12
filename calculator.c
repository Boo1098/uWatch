

#include "uWatch-op.h"
#include "calculator.h"
#include "menu.h"
#include "characterset.h"



/////////////////////////////////////////////////////////////////////////////////////////


const charSet mathLogCharset[] = {
    character_powerx,
    character_squareRoot1,
    character_y,
    character_ybar,
    character_g,
};

const menuItem mathLogMenu[] = {
    { "   e\3 ",     OperatePrecedence,  CALC_OP_EXP     },
    { " ln  ",      OperatePrecedence,  CALC_OP_LN      },
    { "\3\4\6",     OperatePrecedence,  CALC_OP_NROOT   },
    { "  10\3 ",    OperatePrecedence,  CALC_OP_10X     },
    { "lo\7 ",       OperatePrecedence,  CALC_OP_LN10    },
    { " x%\5",       OperatePrecedence,  CALC_OP_MODULUS },
};


const packedMenu2 mathLogarithmic = {
    0, //CALC-STYLE
    printMenu,
    5, mathLogCharset, 6, mathLogMenu
};

const charSet mathBaseCharset[] = {
    character_base2,
    character_base8,
    character_base16,
};

const menuItem mathModeModeMenu[] = {
    { "[fit]",  OperatePrecedence,  CALC_OP_ORIGINAL     },
    { "    ",   OperatePrecedence,  CALC_OP_NULL    },
    { "  SI",        OperatePrecedence,  CALC_OP_MODEENGN     },
    { " fix ",   OperatePrecedence,  CALC_OP_MODEFIX     },
    { " sci ",   OperatePrecedence,  CALC_OP_MODESCI     },
    { " en\7",   OperatePrecedence,  CALC_OP_MODEENG     },
};

const packedMenu2 mathModeMode = {
    0, //CALC-STYLE
    printMenu,
    5, mathLogCharset, 6, mathModeModeMenu
};

const menuItem mathBaseMenu[] = {
    { " bin\3",  OperatePrecedence,  CALC_OP_BASE2       },
    { " oct\4",  OperatePrecedence,  CALC_OP_BASE8       },
    { " hex\5",  OperatePrecedence,  CALC_OP_BASE16      },
    { " dec ",   OperatePrecedence,  CALC_OP_BASE10      },
    { " rad ",   OperatePrecedence,  CALC_OP_MODERAD     },
    { " de\7",   OperatePrecedence,  CALC_OP_MODEDEG     },
};

const packedMenu2 mathModeBase = {
    0, //CALC-STYLE
    printMenu,
    3, mathBaseCharset, 4, mathBaseMenu
};

const menuItem mathHyperMenu[] = {
    { " sin\3",      OperatePrecedence,  CALC_OP_HYP_SIN     },
    { " cos\3",      OperatePrecedence,  CALC_OP_HYP_COS     },
    { " tan\3",      OperatePrecedence,  CALC_OP_HYP_TAN     },
    { " sin\3\351",    OperatePrecedence,  CALC_OP_HYP_ASIN    },
    { "cos\3\351",     OperatePrecedence,  CALC_OP_HYP_ACOS    },
    { "tan\3\351",     OperatePrecedence,  CALC_OP_HYP_ATAN    },
};


const charSet mathTrigCharset[] = {
    character_h,
};

const menuItem mathTrigMenu[] = {
    { " sin ",  OperatePrecedence,  CALC_OP_SIN    },
    { " cos ",  OperatePrecedence,  CALC_OP_COS    },
    { " tan ",  OperatePrecedence,  CALC_OP_TAN    },
    { " sin\351", OperatePrecedence,  CALC_OP_ASIN   },
    { " cos\351", OperatePrecedence,  CALC_OP_ACOS   },
    { " tan\351", OperatePrecedence,  CALC_OP_ATAN   },
};

const packedMenu2 mathHyperbolic = {
    0,  //CALC-STYLE
    printMenu,
    1, mathTrigCharset, 6, mathHyperMenu
};

const packedMenu2 mathTrigonometry = {
    0,  //CALC-STYLE
    printMenu,
    0, 0, 6, mathTrigMenu
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
    { "  nPr ",  OperatePrecedence, CALC_OP_PERMUTATION  },
    { " nCr",  OperatePrecedence, CALC_OP_COMBINATION  },
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
    { "  clx ",      OperatePrecedence, CALC_OP_STAT_CLX     },
    { " \366x  ",   OperatePrecedence, CALC_OP_STAT_SIGMAX  },
    { "\366-",  OperatePrecedence, CALC_OP_STAT_SUB     },
    { "    \345  ",   OperatePrecedence, CALC_OP_STAT_SD      },
    { " \370 ",    OperatePrecedence, CALC_OP_STAT_MEAN    },
    { " \366+",   OperatePrecedence, CALC_OP_STAT_ADD     },
};

const packedMenu2 mathStats = {
    0, //CALC-STYLE
    printMenu,
    1, mathStatsCharset, 6, mathStatsMenu
};

     

const charSet mathConstantCharset[] = {
    character_pi,0,character_g,character_j,
};

const menuItem mathConstantMenu[] = {
    { "   \3 ", OperatePrecedence, CALC_OP_PI       },
    { "  c  ",   OperatePrecedence, CALC_OP_LIGHT    },
    { "Avo\5ad",  OperatePrecedence, CALC_OP_AVOGADRO },
    { "   e ",  OperatePrecedence, CALC_OP_E        },
    { "     ", OperatePrecedence, CALC_OP_NULL },
    { "Planck",  OperatePrecedence, CALC_OP_PLANCK   },
};

const packedMenu2 mathConstant = {
    0, //CALC-STYLE
    printMenu,
    3, mathConstantCharset, 6, mathConstantMenu
};

const menuItem mathConversion1Menu[] = {
    { "mils\176mm",       OperatePrecedence, CALC_OP_CONV_MILS_TO_MM  },
    { "  ", OperatePrecedence, CALC_OP_NULL },
    { "mm\176mils",       OperatePrecedence, CALC_OP_CONV_MM_TO_MILS  },
    { " \337F\176\337C",  OperatePrecedence, CALC_OP_CONV_FC          },
    { "  ", OperatePrecedence, CALC_OP_NULL },
    { " \337C\176\337F",  OperatePrecedence, CALC_OP_CONV_CF          },
};

//const charSet mathConversion2Charset[] = {
//    character_arrow
//};

const menuItem mathConversion2Menu[] = {
    { "k\5\176lb ",    OperatePrecedence,  CALC_OP_CONV_KG_LB  },
    { " R\176P  ",   OperatePrecedence,  CALC_OP_R2P         },
    { "\337\176r", OperatePrecedence, CALC_OP_D2R },
    { "lb\176k\5 ",    OperatePrecedence,  CALC_OP_CONV_LB_KG  },
    { " P\176R  ", OperatePrecedence,  CALC_OP_P2R         },
    { "r\176\337", OperatePrecedence, CALC_OP_R2D },
};

const packedMenu2 mathConversion1 = {
    0, //CALC-STYLE
    printMenu,
    3,mathConstantCharset, 6, mathConversion1Menu
};

const packedMenu2 mathConversion2 = {
    0, //CALC-STYLE
    printMenu,
    3,mathConstantCharset, 6, mathConversion2Menu
};


const charSet mathOtherCharset[] = {
    character_squaring,             //4
    character_powerx,        
    character_squareRoot1,
    character_y,
    character_g,
};

const menuItem mathTimeDateMenu[] = {
    { "\176D.MY",  OperatePrecedence,  CALC_OP_DMY             },
    { " da\6s ", OperatePrecedence,  CALC_OP_DAYS            },
    { "sunst",  OperatePrecedence,  CALC_OP_SUNSET          },
    { "\176H.MS ", OperatePrecedence,  CALC_OP_HMS             },
    { "hour",   OperatePrecedence,  CALC_OP_HOURS           },
    { "", OperatePrecedence, CALC_OP_NULL },
};

const packedMenu2 mathTimeDate = {
    0, //CALC-STYLE
    printMenu,
    5, mathOtherCharset, 6, mathTimeDateMenu
};


const menuItem menuKeystrokeMenu[] = {
    { " Record",  OperatePrecedence,  CALC_OP_RECORD          },
    { "  ", OperatePrecedence, CALC_OP_PLAY },
    { " Pla\6",  OperatePrecedence,  CALC_OP_PLAY            },
    { "", OperatePrecedence, CALC_OP_NULL },
    { "", OperatePrecedence, CALC_OP_NULL },
    { "", OperatePrecedence, CALC_OP_NULL },
};

const packedMenu2 menuKeystroke = {
    0, //CALC-STYLE,
    printMenu,
    5, mathOtherCharset, 6, menuKeystrokeMenu 
};



const menuItem mathOtherMenu[] = {
    { "  1/x ",  OperatePrecedence, CALC_OP_RECIPROCAL     },
    { " x!",     OperatePrecedence, CALC_OP_FACTORIAL      },
    { "  x\3",   OperatePrecedence, CALC_OP_SQUARE         },
    { "   \5\370 ",   OperatePrecedence, CALC_OP_SQRT           },
    { "\174x\174 ",   OperatePrecedence, CALC_OP_ABS            },
    { " \6\4",  OperatePrecedence, CALC_OP_NPOW           },
};

const packedMenu2 mathOther = {
    0, //CALC-STYLE
    printMenu,
    5, mathOtherCharset, 6, mathOtherMenu
};



const menuItem mathComplexMenu[] = {
    { " Ima\5",  OperatePrecedence, CALC_OP_IMAGINARY_PART  },  
    { "  // ",  OperatePrecedence, CALC_OP_PARALLEL        },  // 1/(1/X+1/y)
    { " Cpx\176R", OperatePrecedence, CALC_OP_COMPLEX_SPLIT   },
    { " Real",  OperatePrecedence, CALC_OP_REAL_PART       },
    { " con\6",  OperatePrecedence, CALC_OP_CONJUGATE       },
    { " R\176Cpx",  OperatePrecedence, CALC_OP_COMPLEX_JOIN    },
};

const packedMenu2 mathComplex = {
    0,  //CALC-STYLE
    printMenu,
    4, mathConstantCharset, 6, mathComplexMenu
};

const menuItem mathBitOperationsMenu[] = {
    { " x>>1",  OperatePrecedence, CALC_OP_BITSHIFT_R1  },  
    { " \6>>x",  OperatePrecedence, CALC_OP_BITSHIFT_RN        },
    { "", OperatePrecedence, CALC_OP_NULL },
    { " x<<1", OperatePrecedence, CALC_OP_BITSHIFT_L1   },
    { " \6<<x",  OperatePrecedence, CALC_OP_BITSHIFT_LN       },
};

const packedMenu2 mathBitOperations = {
    0,  //CALC-STYLE
    printMenu,
    5, mathOtherCharset, 5, mathBitOperationsMenu
};

const packedMenu2 *calcMenus[CALC_MENU_SIZE] = {
    &mathOther,
    &mathTrigonometry,
    &mathHyperbolic,
    &mathBoolean,
    &mathModeMode,
    &mathModeBase,
    &mathBitOperations,
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
