/***************************************************************************
                          syntax.h  -  description
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

#ifndef SYNTAX_H
#define SYNTAX_H

#include <vector>
#include <typeinfo>

#include "argument.h"
#include "opcode.h"

class Syntax
{
	Opcode Encoding;
	enum {MaximumArity = 3};
	const type_info *ArgumentTypes[MaximumArity];
	bool Interchangeable;

	unsigned int Precedence;

	bool Match (vector<Argument *> &Arguments) const throw ();
	unsigned int GetArity () const throw ();

	public:
	Syntax (unsigned int p, unsigned int sz, Byte b0, Byte b1, Byte b2, bool i = false,
		const type_info *t0 = 0, const type_info *t1 = 0, const type_info *t2 = 0) throw ();
	~Syntax () throw () {}

	bool operator< (const Syntax &s) const throw() {return Precedence < s.Precedence;}
	bool Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const;
};

#endif
