//--------------------------------------------------------------------------
//                          defs.cpp  -  description
//                             -------------------
//    begin                : Wed May 2 2001
//    copyright            : (C) 2001 by Helcio Mello
//    email                : helcio@users.sourceforge.net
//--------------------------------------------------------------------------

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "defs.h"

// Zero-extends a byte to a double word
Dword ZeroExtend (Byte b)
{
	Dword answer = b;
	answer &= 0xFF;
	return answer;
}

// Zero-extends a word to a double word
Dword ZeroExtend (Word w)
{
	Dword answer = w;
	answer &= 0xFFFF;
	return answer;
}

Dword SignExtend (Byte b)
{
	Dword answer = b;
	if (b & 0x80 != 0)
	{
		answer |= 0xFFFFFF00;
	}
	else
	{
		answer &= 0xFF;
	}

	return answer;
}

Dword SignExtend (Word w)
{
	Dword answer = w;
	if (w & 0x8000 != 0)
	{
		answer |= 0xFFFFFF00;
	}
	else
	{
		answer &= 0xFFFF;
	}

	return answer;

}

Dword SHR (Dword d, int n)
{
	Dword mask = ((1 << (32 - n)) - 1);
	return (d >> n) & mask;
}

Dword SAR (Dword d, int n)
{
	bool negative = (d & 0x80000000) != 0;
	d >>= n;

	Dword mask = ((1 << (32 - n)) - 1);
	if (negative)
	{
		d |= ~mask;
	}
	else
	{
		d &= mask;
	}

	return d;
}
