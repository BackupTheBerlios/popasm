/***************************************************************************
                          dup_arg.h  -  description
                             -------------------
    begin                : Sun Mar 9 2003
    copyright            : (C) 2003 by Helcio Mello
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

#ifndef DUP_ARG_H
#define DUP_ARG_H

#include <string>

#include "argument.h"
#include "type_exp.h"
#include "immed.h"

class DupArgument : public BasicArgument
{
	DupExpression Data;

	public:
	DupArgument (const DupExpression &exp) : BasicArgument (exp), Data(exp) {}
	~DupArgument () {}

	static Argument *MakeArgument (const Expression &e) throw (InvalidArgument, exception);
	void Write (vector<Byte> &Output) const throw (UnknownImmediateSize, Overflow);
	string Print () const throw ();
};

#endif
