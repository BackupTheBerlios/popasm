/***************************************************************************
                          constant.cpp  -  description
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

#include "constant.h"
#include "asmer.h"

Constant::Constant (const string &n, Expression *exp = 0, bool m = false) throw () : UserDefined (n, Type(UNDEFINED, SCALAR))
{
	if (exp != 0)
		Values.push_back (exp);

	CurrentPass = CurrentAssembler->GetCurrentPass();
	DefinitionCount = (exp == 0) ? 0 : 1;
	Mutable = m;
}

Constant::~Constant () throw ()
{
	for (vector<Expression *>::iterator i = Values.begin(); i != Values.end(); i++)
		delete *i;
}

const Expression *Constant::GetValue() const throw ()
{
	if (CurrentPass != CurrentAssembler->GetCurrentPass())
	{
		CurrentPass = CurrentAssembler->GetCurrentPass();
		DefinitionCount = 0;
	}

	return Values[CurrentIndex()];
}

void Constant::SetValue(Expression *NewValue) throw ()
{
	if (Mutable)
	{
		if (DefinitionCount < Values.size())
		{
			delete Values[DefinitionCount];
			Values[DefinitionCount] = NewValue;
		}
		else
			Values.push_back(NewValue);

		DefinitionCount++;
	}
	else
		throw MultidefinedSymbol (GetName());
}

bool Constant::Changed (const BasicSymbol *s) throw ()
{
	const Constant *c = dynamic_cast<const Constant *> (s);
	if (c == 0)
	{
		cout << "Internal error. Please send a bug report." << endl;
		return false;
	}

	if (*GetValue() != *c->GetValue())
		return true;

	return UserDefined::Changed (s);
}
