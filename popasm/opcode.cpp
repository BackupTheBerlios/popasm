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
	Output.insert (Output.end(), begin(), end());
}

const Opcode Opcode::operator+ (Byte b) const
{
	Opcode answer (*this);
	answer.push_back (b);
	return answer;
}

const Opcode Opcode::operator| (const Opcode &op) const
{
	Opcode answer(*this);
	vector<Byte>::reverse_iterator i;
	vector<Byte>::const_reverse_iterator j;

	for (i = answer.rbegin(), j = op.rbegin(); (i != answer.rend()) && (j != op.rend()); i++, j++)
		*i |= *j;

	return answer;
}

const Opcode Opcode::operator<< (unsigned int n) const
{
	Opcode answer (*this);
	answer.back() <<= n;
	return answer;
}
