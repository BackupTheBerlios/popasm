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

#include <vector>
#include <string>

#include "variable.h"
#include "type_exp.h"

class Constant : public UserDefined
{
	vector <Expression *> Values;
	mutable unsigned int DefinitionCount;
	mutable unsigned int CurrentPass;

	bool Mutable;

	Constant (const Constant &c) throw () : UserDefined (c) {}
	unsigned int CurrentIndex () const throw () {return (DefinitionCount == 0) ? 0 : DefinitionCount - 1;}

	public:
	Constant (const string &n, Expression *exp = 0, bool m = false) throw ();
	~Constant () throw ();

	const Expression *GetValue() const throw ();
	void SetValue(Expression *NewValue) throw ();

	bool IsMutable () const throw () {return Mutable;}
	bool Changed (const BasicSymbol *s) throw ();
};

#endif
