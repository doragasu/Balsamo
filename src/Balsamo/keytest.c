/* This file is part of BALSAMO source package.
 *
 * BALSAMO is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * Some open source application is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BALSAMO.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "keyb.h"
#include "xlcd\\xlcd.h"

void LcdPrint(char string[])
{
	while(BusyXLCD());
	WriteCmdXLCD(CLEAR_XLCD);
	// Escribir cadena de saludo
	while(BusyXLCD());
	PutsXLCD((char*)string);
}

void KeybUpLeftPress(void)
{
	LcdPrint("UP_LEFT_PRESS");
}

void KeybUpLeftRelease(void)
{
	LcdPrint("UP_LEFT_RELEASE");
}

void KeybDownRightPress(void)
{
	LcdPrint("DOWN_RIGHT_PRESS");
}

void KeybDownRightRelease(void)
{
	LcdPrint("DOWN_RIGHT_RELEA");
}

void KeybEnterPress(void)
{
	LcdPrint("ENTER_PRESS");
}

void KeybEnterRelease(void)
{
	LcdPrint("ENTER_RELEASE");
}

void KeybEscPress(void)
{
	LcdPrint("ESC_PRESS");
}

void KeybEscRelease(void)
{
	LcdPrint("ESC_RELEASE");
}
