/***************************************************************************
                          constant.h  -  description
                             -------------------
    begin                : Sat Feb 1 2003
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

#ifndef CONSTANT_H
#define CONSTANT_H

#include <exception>
#include <string>

#include "variable.h"
#include "type_exp.h"

class RedefinedConstant : public exception
{
	string WhatString;

	public:
	RedefinedConstant (const string &s) throw () : WhatString ("Illegal constant redefinition: ") {WhatString += s;}
	~RedefinedConstant () throw () {}

	const char *what () const throw () {return WhatString.c_str();}
};

class Constant : public UserDefined
{
	Expression *Value;
	bool Mutable;

	public:
	Constant (const string &n, Expression *exp = 0, bool m = false) throw () : UserDefined (n), Value(exp), Mutable(m) {}
	~Constant () throw () {delete Value;}

	const Expression *GetValue() const throw () {return Value;}
	void SetValue(Expression *NewValue) throw ();

	bool IsMutable () const throw () {return Mutable;}
};

#endif
