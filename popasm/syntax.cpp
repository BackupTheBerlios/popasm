/***************************************************************************
                          syntax.cpp  -  description
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

#include "syntax.h"

Syntax::Syntax (unsigned int p, unsigned int sz, Byte b0, Byte b1, Byte b2, bool i = false,
	const type_info *t0 = 0, const type_info *t1 = 0, const type_info *t2 = 0) throw ()
{
	switch (sz)
	{
		case 0:
			break;

		case 1:
			Encoding = Opcode (b0);
			break;

		case 2:
			Encoding = Opcode (b0, b1);
			break;

		case 3:
			Encoding = Opcode (b0, b1, b2);
			break;

		default:
			throw 0;
	}

	ArgumentTypes[0] = t0;
	ArgumentTypes[1] = t1;
	ArgumentTypes[2] = t2;

	Interchangeable = i;
	Precedence = p;
}

unsigned int Syntax::GetArity () const throw ()
{
	unsigned int i, count;

	for (i = count = 0; i < MaximumArity; i++)
	{
		if (ArgumentTypes[i] != 0) count++;
		else break;
	}

	return count;
}

bool Syntax::Match (vector<Argument *> &Arguments) const throw ()
{
	// Checks arity
	if (GetArity() != Arguments.size()) return false;

	bool Result = true;
	unsigned int i = 0;
	vector<Argument *>::const_iterator j = Arguments.begin();

	// Checks the types of each argument
	for (; j != Arguments.end(); i++, j++)
	{
		if (!(*j)->Match (*ArgumentTypes[i]))
		{
			Result = false;
			break;
		}
	}

	// If the argument types of the syntax are interchangeable, try matching in reverse order
	if (Interchangeable && (Result == false) && (Arguments.size() >= 2))
		return (*ArgumentTypes[0] != typeid (*Arguments[1])) && (*ArgumentTypes[1] != typeid (*Arguments[0]));

	return Result;
}

bool Syntax::Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const
{
	if (!Match (Arguments)) return false;

	const Immediate *immed;

	switch (Arguments.size())
	{
		case 0:
			// Command with no arguments: just copy its opcode to output
			Encoding.Write (Output);
			break;

		case 1:
			Encoding.Write (Output);

			// Checks whether the sole argument is an immediate
			immed = dynamic_cast<const Immediate *> (Arguments.back()->GetData());
			if (immed != 0)
			{
				// If it is, just write it to the output
				immed->Write (Output);
			}
			else
			{
			}

			break;

		case 2:
		case 3:
			break;
	}

	return true;
}
