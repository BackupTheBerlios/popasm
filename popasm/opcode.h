/***************************************************************************
                          opcode.h  -  description
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

#ifndef OPCODE_H
#define OPCODE_H

#include <vector>
#include "defs.h"

class Opcode :	vector<Byte>
{
	public:
	Opcode () throw () {}
	Opcode (Byte b0) throw () {push_back (b0);}
	Opcode (Byte b0, Byte b1) throw () {push_back (b0); push_back (b1);}
	Opcode (Byte b0, Byte b1, Byte b2) throw () {push_back (b0); push_back (b1); push_back (b2);}
	Opcode (Byte b0, Byte b1, Byte b2, Byte b3) throw () {push_back (b0); push_back (b1); push_back (b2); push_back (b3);}
	~Opcode () throw () {}

	const Opcode operator+ (Byte b) const;
	const Opcode operator| (const Opcode &op) const;
	const Opcode operator<< (unsigned int n) const;

	void Write (vector<Byte> &Output) const throw ();
};

#endif
