/***************************************************************************
                          full_ptr.cpp  -  description
                             -------------------
    begin                : Sun Aug 4 2002
    copyright            : (C) 2002 by Helcio Mello
    email                : helcio@users.sf.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "full_ptr.h"
#include "defs.h"

const char InvalidFullPointer::WhatString[] = "Invalid immediate full-pointer.";

FullPointer::FullPointer (unsigned int sz, Word seg, Dword off) throw (InvalidFullPointer) : BasicArgument (sz)
{
	if ((sz == 0) && (CurrentAssembler->GetCurrentMode() == 16) && (off > 0xFFFF)) throw InvalidFullPointer();

	Segment = seg;
	Offset = off;
}

void FullPointer::Write (vector<Byte> &Output) const throw ()
{
	BasicDataTypes TypesUnion;

	TypesUnion.Write (Output, Offset, (GetSize() == 0) ? (CurrentAssembler-> GetCurrentMode() / 8) : (GetSize() / 8));
	TypesUnion.Write (Output, Segment, 2);
}
