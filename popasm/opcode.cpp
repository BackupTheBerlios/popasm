/***************************************************************************
                          opcode.cpp  -  description
                             -------------------
    begin                : Tue Jun 18 2002
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

#include "opcode.h"

void Opcode::Write (vector<Byte> &Output) const throw ()
{
	for (unsigned int i = 0; i < Size; i++)
		Output.push_back (Encoding[i]);
}

const Opcode Opcode::operator+ (Byte b) const
{
	// Checks if there's more room to place the aditional byte
	if (Size == MaximumEncodingLength) throw 0;

	Opcode answer (*this);
	answer.Encoding[answer.Size++] = b;
	return answer;
}

const Opcode Opcode::operator| (const Opcode &op) const
{
	Opcode answer(*this);
	unsigned int i, j;

	for (i = Size, j = op.Size; (i > 0) && (j > 0); i--, j--)
		answer.Encoding[i - 1] |= op.Encoding[j - 1];

	return answer;
}
