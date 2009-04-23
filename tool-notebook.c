#include "notebook.h"
#include "menu.h"
#include "def.h"


const char *view = "Four score and seven years ago our fathers brought forth, upon this continent, a new nation, conceived in Liberty, and dedicated to the proposition that all men are created equal.";
int notebook() {

    viewString( "Memo:", (char *)view, 0, 2 );
    return MODE_EXIT;
}

