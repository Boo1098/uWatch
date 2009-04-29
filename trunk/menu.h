#ifndef __MENU_H
#define __MENU_H

#define MENU_CUSTOM_CHAR_COUNT 4
#define MENU_ITEM_MAX 10

typedef struct {
    const char *name;
    int ( *run )( int op );
    int op;
} menuItem;

typedef struct {
    const char *title;
    char *( *print )( int *sel, menuItem *menu );
    void ( *inc )( int *sel, int max );
    void ( *dec )( int *sel, int max );
    const int max;
    const unsigned char *customCharacter[MENU_CUSTOM_CHAR_COUNT];
    const menuItem menu[MENU_ITEM_MAX];
} packedMenu;

char *printMenu( int *item, menuItem *menu );
void increment( int *selection, int max );
void decrement( int *selection, int max );

int genericMenu2( const packedMenu *menu, int *selection );

int genericMenu( char *title,
                 char *( *printFunc )( int *num, int max ),
//                 int ( *idleFunc )( int *halt ),
                 void ( *incrementFunc )( int *num, int max ),
                 void ( *decrementFunc )( int *num, int max ),
                 int max, int *selection );

#define VIEW_ONESHOT 1
#define VIEW_AUTOSCROLL 2

int viewString( char *title, char *string, int *selection, int viewControl );
int calculatorMenu( const packedMenu *menu[], int size );

extern const packedMenu mathMenu;
extern const packedMenu *calcMenus[];
#define CALC_MENU_SIZE 12


#endif
