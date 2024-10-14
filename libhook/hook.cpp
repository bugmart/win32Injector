#include "pch.h"
#include "hook.h"
#include <stdlib.h>
#include "define.h"

#define NOP_COUNT			0x10
/*
_declspec(naked) void asmfunc_jump_wrapper()
{
	_asm
	{
		pushad			//0 offset
		push 0xEEEEEEEE	//1 offset	//the return address after executing custom code
		push 0xFFFFFFFF	//6 offset	//address of custom code
		retn			//0xB offset
		popad			//0xC offset
		nop				//0xD offset
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop				//jmp to origin code flow
		nop				
		nop
		nop
		nop		
	}
}
*/

//byte codes of asmfunc_jump_wrapper function
unsigned char g_jump_wrapper_bytes[] = {0x60,0x68,0xEE,0xEE,0xEE,0xEE,0x68,0xFF,0xFF,0xFF,0xFF,0xC3,0x61,0x90,0x90,0x90,0x90,0x90,0x90,
										0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90 };

bool InsertJumpCode(unsigned int jumpFrom, unsigned int jumpTo, unsigned int replaceBytes)
{
	unsigned char jumpOpcodes[5] = { 0xE9,0,0,0,0 };

	//0x10 - 0x5
	if (replaceBytes >= 0xB)
		return false;

	//allocate dynamic memory for executing wrapper code
	unsigned char* wrapper_code = (unsigned char*)malloc(sizeof(g_jump_wrapper_bytes));
	if (wrapper_code == NULL)
	{
		return false;
	}

	//copy code from asmfunc_jump_wrapper function codes
	memcpy(wrapper_code, g_jump_wrapper_bytes, sizeof(g_jump_wrapper_bytes));

	//set jump addresses
	dword_val(wrapper_code + 1 + 1) = (DWORD)wrapper_code + 0xC;
	dword_val(wrapper_code + 6 + 1) = jumpTo;

	//copy old codes to dynamic memory
	memcpy(wrapper_code + 0xD, (void*)jumpFrom, replaceBytes);

	DWORD diff = jumpFrom + replaceBytes - ((DWORD)wrapper_code + 0xD + replaceBytes ) - sizeof(jumpOpcodes);
	memcpy(jumpOpcodes + 1, &diff, 4);
	memcpy(wrapper_code + 0xD + replaceBytes, jumpOpcodes, 5);
	DWORD oldProtect;
	VirtualProtect(wrapper_code, sizeof(g_jump_wrapper_bytes), PAGE_EXECUTE_READWRITE, &oldProtect);
	
	diff = (DWORD)wrapper_code - jumpFrom - sizeof(jumpOpcodes);
	memcpy(jumpOpcodes + 1, &diff, 4);

	VirtualProtect((void*)jumpFrom, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy((void*)jumpFrom, jumpOpcodes, sizeof(jumpOpcodes));
	VirtualProtect((void*)jumpFrom, 5, oldProtect, &oldProtect);

	return true;
}

