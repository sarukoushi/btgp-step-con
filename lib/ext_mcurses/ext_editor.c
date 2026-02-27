/*-----------------------------------------------------------------------------------------------------------------

   line editor 

   Revision History:
   V1.0 2017 01 18 ChrisMicro, initial version

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Modified by: nyxaru

  ---------------------------------------------------------------------------------------------------------------*/

#include "mcurses.h"
#include <stdlib.h>     // strtol 
#include <string.h>

int32_t editInt32(int32_t initialNumber)
{
	  char text[12];
      uint8_t starty;
      uint8_t startx;
	      
      getyx (starty, startx);   // get current cursor position

	  itoa(initialNumber,text,10);
	  editLine (text,11);
	  
	  int32_t number = strtol(text, 0, 10);

	  itoa(number,text,10);
	  move (starty, startx);    // set cursor position from the beginning
	  addstr (text);            // show text
	  return number;
}

uint32_t editUInt32(uint32_t initialNumber)
{
	  char text[11];
      uint8_t starty;
      uint8_t startx;
	      
      getyx (starty, startx);   // get current cursor position

      utoa(initialNumber,text,10);
	  editLine (text,10);
	  
	  uint32_t number = strtoul(text, 0, 10);

	  utoa(number,text,10);
	  move (starty, startx);    // set cursor position from the beginning
	  addstr (text);            // show text
	  return number;
}