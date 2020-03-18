/**************************************************************************/
/*!
    @file     Menu.h
    @author   The Firebrand Forge

    @section LICENSE

    Copyright (C) 2018 The Firebrand Forge Inc

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
/**************************************************************************/
#ifndef MENU_H
#define MENU_H

#ifndef MENU_FUNCTIONS_COUNT
	#define MENU_FUNCTIONS_COUNT 10
#endif

#ifndef STATE_POINTERS_COUNT
	#define STATE_POINTERS_COUNT 2
#endif

typedef const struct Menu
{
	union
	{
		struct
		{
			const struct Menu *	NextMenu;
			const struct Menu *	PrevMenu;
		};
		const struct Menu *	PathMap[STATE_POINTERS_COUNT];
	};
	union
	{
		struct
		{
			void (* const InitFunction)(void);
			void (* const MainFunction)(void);
			void (* const FunctionMap[MENU_FUNCTIONS_COUNT-2])(void);
		};
		void (* const Function[MENU_FUNCTIONS_COUNT])(void);
	};
} MENU_T;

extern MENU_T * Menu_GetMenu(void);
extern void Menu_SetMenu(MENU_T * target);
extern void Menu_SetNextMenu(void);
extern void Menu_SetPrevMenu(void);
extern void Menu_StartMenu(MENU_T * target);
extern void Menu_StartNextMenu(void);
extern void Menu_StartPrevMenu(void);
extern void Menu_DoMainFunction(void);
extern void Menu_DoFunction(uint8_t num);

#endif
