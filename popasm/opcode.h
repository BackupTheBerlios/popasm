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

class Opcode
{
	unsigned int Size;
	enum {MaximumEncodingLength = 3};
	Byte Encoding[MaximumEncodingLength];

	public:
	Opcode () throw () : Size(0) {}
	Opcode (Byte b0) throw () : Size(1) {Encoding[0] = b0;}
	Opcode (Byte b0, Byte b1) throw () : Size(2) {Encoding[0] = b0; Encoding[1] = b1;}
	Opcode (Byte b0, Byte b1, Byte b2) throw () : Size(3) {Encoding[0] = b0; Encoding[1] = b1; Encoding[2] = b2;}
	~Opcode () throw () {}

	void Write (vector<Byte> &Output) const throw ();
};

#endif
