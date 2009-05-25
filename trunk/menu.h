
#ifndef __MENU_H
#define __MENU_H

#define MENU_ITEM_MAX 6

typedef struct {
    const char *name;
    int ( *run )( int op );
    int op;
} menuItem;


typedef const unsigned char * charSet;

typedef struct {
    const char *title;
    char *( *print )( int *sel, const menuItem *menu );
    int csetsize;
    const charSet *charset;
    int menusize;
    const menuItem *menu;
} packedMenu2;



char *printMenu( int *item, const menuItem *menu );
void increment( int *selection, int max );
void decrement( int *selection, int max );

int genericMenu2( const packedMenu2 *menu );

int genericMenu( char *title,
                 char *( *printFunc )( int *num, int max ),
//                 int ( *idleFunc )( int *halt ),
                 void ( *incrementFunc )( int *num, int max ),
                 void ( *decrementFunc )( int *num, int max ),
                 int max, int *selection );

#define VIEW_ONESHOT 1
#define VIEW_AUTOSCROLL 2

int viewString( char *title, char *string, int *selection, int viewControl );
int calculatorMenu( const packedMenu2 *menu[], int size );

extern const packedMenu2 mathMenu;
extern const packedMenu2 *calcMenus[];
#define CALC_MENU_SIZE 16


#endif
