/*
  Menu generator
  --------------
  By Jac Goudsmit
  Distributed under the BSD (3-clause) license.
*/


#ifndef LCDMENU_H
#define LCDMENU_H


#ifdef __cplusplus
extern "C"
{
#endif


/////////////////////////////////////////////////////////////////////////////
// MACROS
/////////////////////////////////////////////////////////////////////////////


#define MENU_STACKDEPTH (10)

#ifndef NoAutoProto
#define NoAutoProto(x) x
#endif

#ifndef MENU_EXT
#define MENU_EXT extern
#endif


/////////////////////////////////////////////////////////////////////////////
// TYPES
/////////////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------------------
// Parameter passed to display callback function
typedef enum
{
  MENUSHOW_MENU,
  MENUSHOW_ITEM,
} MenuShow;


//---------------------------------------------------------------------------
// Parameter passed to processing function
typedef enum
{
  MENUACTION_NONE,                      // No action
  MENUACTION_PREV,                      // Switch to previous menu item
  MENUACTION_NEXT,                      // Switch to next menu item
  MENUACTION_SELECT,                    // Select the current item
} MenuAction;


//---------------------------------------------------------------------------
// Parameter passed to navigation function
typedef enum
{
  MENUNAV_MENU,                         // Entering a menu
  MENUNAV_CHANGE,                       // Current menu item has changed
  MENUNAV_SELECT,                       // User pressed Select button
} MenuNav;


//---------------------------------------------------------------------------
// Function type for menu items
//
// A function of this type can be called whenever a menu option is selected.
#define MENUITEMFUNC_PROTO(name) void name(void *menuparm)
typedef MENUITEMFUNC_PROTO(MenuItemFunc_t);


//---------------------------------------------------------------------------
// Function type for navigation function
//
// A function of this type can be connected to each menu. It is called at
// the following occasions:
// - Just after the menu is loaded that the navigation function is attached
//   to. The function can change the current item before returning, e.g. to
//   show the current setting.
// - After the user changes the selection with the Up or Down button (but
//   only if the selection was actually changed, not when the user tries to
//   navigate past the first or last options)
// - After the user makes a selection, and the according menu item function
//   has been called. The navigation function can return true to leave the
//   menu in this case (the return values for the other cases is ignored).
//   Note, if a menu item function was called before calling the navigation
//   function, the current menu may have changed and you may not want to
//   return true or you will leave the wrong menu!
#define MENUNAVFUNC_PROTO(name) boolean name(MenuNav navmode)
typedef MENUNAVFUNC_PROTO(MenuNavFunc_t);


//---------------------------------------------------------------------------
// Type definition for a menu item
typedef struct MenuItem_t
{
  prog_char        *m_name;             // EEPROM based string, NULL=end list
  MenuItemFunc_t   *m_func;             // Optional item function
  void             *m_parm;             // Parameter to pass to function
} MenuItem;
#define MENUITEMLIST(name) MenuItem name[] PROGMEM = {
#define MENUITEMLISTEND { NULL, NULL, NULL } };
#define MENUITEM(name, func, parm) { name, func, (void*)parm },


//---------------------------------------------------------------------------
// Type definition for a menu
typedef struct Menu_t
{
  prog_char        *m_menuname;         // EEPROM based string for screen
  MenuItem PROGMEM *m_items;            // EEPROM based list of items
  MenuNavFunc_t    *m_navfunc;          // Optional navigation function
} Menu;


//---------------------------------------------------------------------------
// Menu navigation stack item
typedef struct MenuState_t
{
  Menu PROGMEM     *m_pmenu;            // Pointer to menu
  byte              m_itemindex;        // Selected item
} MenuState;


/////////////////////////////////////////////////////////////////////////////
// DATA
/////////////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------------------
// Working variables
MENU_EXT Menu       menu_gCurMenu;      // RAM copy of current menu
MENU_EXT MenuItem   menu_gCurItem;      // RAM copy of current item
MENU_EXT MenuState  menu_gStack[MENU_STACKDEPTH]; // Stack of selected items
MENU_EXT MenuState *menu_gpCurState;    // Pointer to current state on stack


/////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
/////////////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------------------
// Forward declarations for menu item functions
MenuItemFunc_t 
  menu_EnterMenu,                       // Enter menu (parm=pointer to Menu)
  menu_LeaveMenu;                       // Leave current menu (parm ignored)


/////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
/////////////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------------------
//! Initialize menu
void menu_Init(
  Menu *pmenu);


//---------------------------------------------------------------------------
//! Process menu action
void 
menu_Process(
  MenuAction action);


//---------------------------------------------------------------------------
//! Show something on the output device
//
// This is provided by the application and called by the menu module
NoAutoProto(
void
menu_ShowCB(
  MenuShow displaymode));


/////////////////////////////////////////////////////////////////////////////
// END
/////////////////////////////////////////////////////////////////////////////


#ifdef __cplusplus
}
#endif

#endif
