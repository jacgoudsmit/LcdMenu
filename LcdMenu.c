/*
  Menu generator
  --------------
  By Jac Goudsmit
  Distributed under the BSD (3-clause) license.
*/


/////////////////////////////////////////////////////////////////////////////
// INCLUDES
/////////////////////////////////////////////////////////////////////////////


#include <Arduino.h>

#define MENU_EXT
#include "LcdMenu.h"


/////////////////////////////////////////////////////////////////////////////
// CODE
/////////////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------------------
//! Initialize the current menu item and show it on the second line
static boolean
load_item()
{
  boolean result = false;

  memcpy_P(&menu_gCurItem, &menu_gCurMenu.m_items[menu_gpCurState->m_itemindex], 
    sizeof(menu_gCurItem));
  if (menu_gCurItem.m_name)
  {
    menu_ShowCB(MENUSHOW_ITEM);
    result = true;
  }

  return result;
}


//---------------------------------------------------------------------------
//! Initialize the menu and show it on the top line
//
// This doesn't initialize the current item.
static void
load_menu()
{
  memcpy_P(&menu_gCurMenu, menu_gpCurState->m_pmenu, sizeof(menu_gCurMenu));
  menu_ShowCB(MENUSHOW_MENU);
}


//---------------------------------------------------------------------------
//! Enter a given menu, increasing the stack depth. Show menu and first item
MENUITEMFUNC_PROTO(menu_EnterMenu)
{
  Menu PROGMEM *menu = (Menu PROGMEM *)menuparm;

  // Go to next entry of stack
  ++menu_gpCurState;

  menu_gpCurState->m_pmenu = menu;
  load_menu();

  menu_gpCurState->m_itemindex = 0;

  // If the new menu has a navigation function, call it immediately
  // The navigation function may change the current item so don't load the
  // initial item until afterwards.
  if (menu_gCurMenu.m_navfunc)
  {
    (void)menu_gCurMenu.m_navfunc(MENUNAV_MENU);
  }

  load_item();
}


//---------------------------------------------------------------------------
//! Leave the current menu. Show the previous menu and its most recent item
MENUITEMFUNC_PROTO(menu_LeaveMenu)
{
  if (menu_gpCurState != &menu_gStack[0])
  {
    --menu_gpCurState;
  }

  load_menu();
  load_item();
}


//---------------------------------------------------------------------------
//! Process menu action
MENUPROCFUNC_PROTO(menu_ProcessMenu)
{
  switch(action)
  {
  case MENUACTION_PREV:
    {
      if (menu_gpCurState->m_itemindex > 0)
      {
        menu_gpCurState->m_itemindex--;
        load_item();

        // If there's a navigation function, call it when selection changes
        if (menu_gCurMenu.m_navfunc)
        {
          (void)menu_gCurMenu.m_navfunc(MENUNAV_CHANGE);
        }
      }
    }
    break;

  case MENUACTION_NEXT:
    {
      menu_gpCurState->m_itemindex++;

      if (!load_item())
      {
        menu_gpCurState->m_itemindex--;
      }
      else
      {
        // If there's a navigation function, call it when selection changes
        if (menu_gCurMenu.m_navfunc)
        {
          (void)menu_gCurMenu.m_navfunc(MENUNAV_CHANGE);
        }
      }
    }
    break;

  case MENUACTION_SELECT:
    {
      // If there's a navigation function, pushing Select leaves the menu
      // after calling the navigation function in "leave" mode.
      // But we should get the navigation function here (not after calling
      // the item function) because the item function might enter a different
      // menu which would make us get the wrong navigation function from the
      // wrong menu.
      MenuNavFunc_t *navfunc = menu_gCurMenu.m_navfunc;

      if (menu_gCurItem.m_func)
      {
        menu_gCurItem.m_func(menu_gCurItem.m_parm);
      }

      if (navfunc)
      {
        if (navfunc(MENUNAV_SELECT))
        {
          menu_LeaveMenu(NULL);
        }
      }
    }
    break;

  default:
    ; // Nothing to do here
  }
}


//---------------------------------------------------------------------------
//! Initialize menu
void menu_Init(
  Menu PROGMEM *pmenu)
{
  menu_Process = menu_ProcessMenu;
  menu_gpCurState = &menu_gStack[-1]; // Following line will change index to 0
  menu_EnterMenu(pmenu);
}


/////////////////////////////////////////////////////////////////////////////
// END
/////////////////////////////////////////////////////////////////////////////
