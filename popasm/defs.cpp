//--------------------------------------------------------------------------
//                          defs.cpp  -  description
//                             -------------------
//    begin                : Wed May 2 2001
//    copyright            : (C) 2001 by Helcio Mello
//    email                : helcio@users.sourceforge.net
//--------------------------------------------------------------------------

//---------------------------------------------------------------------------
// This file contains definitions and functions used throughout the project
//---------------------------------------------------------------------------

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
Dword ZeroExtend (Byte b) throw ()
{
	Dword answer = b;
	answer &= 0xFF;
	return answer;
}

// Zero-extends a word to a double word
Dword ZeroExtend (Word w) throw ()
{
	Dword answer = w;
	answer &= 0xFFFF;
	return answer;
}

Dword SignExtend (Byte b) throw ()
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

Dword SignExtend (Word w) throw ()
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

Dword SHR (Dword d, int n) throw ()
{
	Dword mask = ((1 << (32 - n)) - 1);
	return (d >> n) & mask;
}

Dword SAR (Dword d, int n) throw ()
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

void BasicDataTypes::Write (vector<Byte> &v, Dword dd, unsigned int n) throw ()
{
	d = dd;
	for (unsigned int i = 0; i < n; i++)
		v.push_back (b[i]);
}

string Print (unsigned long int i) throw ()
{
	string s;
	unsigned long int d;

	// Pushes each digit to a temporary string.
	do
	{
		d = i % 10;
		d += '0';
		s += static_cast<char> (d);
		i /= 10;
	} while (i != 0);

	// Digits came from least to most significant. Order must be adjusted.
	reverse(s.begin(), s.end());
	return s;
}

void UpperCase (string &s) throw ()
{
	for (string::iterator i = s.begin(); i != s.end(); i++)
		if ((*i <= 'z') && (*i >= 'a')) *i -= 32;
}
