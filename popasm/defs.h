//--------------------------------------------------------------------------
//                          defs.h  -  description
//                             -------------------
//    begin                : Mon Apr 23 2001
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

#ifndef DEFS_H
#define DEFS_H

#include <string>
#include <vector>
#include <algorithm>

#include "asmer.h"

// typedefs used here to save some typing and increase portability
typedef unsigned char Byte;
typedef unsigned short int Word;
typedef unsigned long int Dword;

Dword ZeroExtend (Byte b) throw ();
Dword ZeroExtend (Word w) throw ();

Dword SignExtend (Byte b) throw ();
Dword SignExtend (Word w) throw ();

// SHR and SAR stand for SHift Right and Shift Arithmetic Right, respectivelly
Dword SHR (Dword d, int n) throw ();
Dword SAR (Dword d, int n) throw ();

union BasicDataTypes
{
	private:
	Byte b[4];
	Dword d;

	public:
	void Write (vector<Byte> &v, Dword dd, unsigned int n) throw ();
};

string Print (unsigned long int i) throw ();

extern Assembler *CurrentAssembler;

#endif
