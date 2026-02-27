/*
 * editor.h
 *
 * Created: 19.01.2017 11:01:08
 *  Author: ChrisMicro
 * 
 *    Modified by: nyxaru
 */ 

#ifdef __cplusplus
extern "C"
{
	#endif

	#ifndef EXTENDED_EDITOR_H_
	#define EXTENDED_EDITOR_H_

	#include "mcurses.h"
	void editLine (char * str, uint8_t lineLength );
	int32_t editInt32(int32_t initialNumber);
	uint32_t editUInt32(uint32_t initialNumber);

	#endif /* EDITOR_H_ */

	#ifdef __cplusplus
}
#endif