/***************************************************************************
                          type_exp.cpp  -  description
                             -------------------
    begin                : Thu Jun 6 2002
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

#include "type_exp.h"

void Expression::SetSize (unsigned int s, Number::NumberType = Number::SIGNED) throw (InvalidSize, CastFailed)
{
	if (t == Type::SCALAR)
	{
		// Searches the constant term in the expression and set its size. This is necessary to
		// make [BYTE eax+3] be the same as [eax + BYTE 3]
		for (vector<Term *>::iterator i = Terms.begin(); i != Terms.end(); i++)
		{
			if ((*i)->Constant())
			{
				(*i)->first->SetSize (s);
				return;
			}
		}

		// If no constant term found, add a dummy zero. This is required for compatibility with NASM
		Number *dummy = new Number (0);
		dummy->SetSize (s);
		BasicExpression<Number, Symbol>::operator+= (BasicExpression<Number, Symbol> (dummy, 0));
	}
	else
	{
		// STRONG or WEAK memory. The user is allowed to change the Size at will
		Size = s;
	}
}

Expression::Expression (Number *n, Symbol *s) throw () : BasicExpression<Number, Symbol> (n, s)
{
	t = Type::SCALAR;
	Size = 0;
}

Expression &Expression::operator=  (const Expression &e) throw ()
{
	BasicExpression<Number, Symbol>::operator= (e);

	t = e.t;
	Size = e.Size;
	return *this;
}

Expression &Expression::operator+= (const Expression &e)
{
	BasicExpression<Number, Symbol>::operator+= (e);

	t += e.t;
	Size = 0;
	return *this;
}

Expression &Expression::operator-= (const Expression &e)
{
	BasicExpression<Number, Symbol>::operator-= (e);

	t -= e.t;
	Size = 0;
	return *this;
}

Expression &Expression::operator*= (const Expression &e)
{
	BasicExpression<Number, Symbol>::operator*= (e);

	t *= e.t;
	Size = 0;
	return *this;
}

Expression &Expression::operator/= (const Expression &e)
{
	BasicExpression<Number, Symbol>::operator/= (e);

	t /= e.t;
	Size = 0;
	return *this;
}

Expression &Expression::operator%= (const Expression &e)
{
	BasicExpression<Number, Symbol>::operator%= (e);

	t %= e.t;
	Size = 0;
	return *this;
}

Expression &Expression::operator- ()
{
	BasicExpression<Number, Symbol>::operator- ();

	t = -t;
	Size = 0;
	return *this;
}

Expression &Expression::operator~ ()
{
	BasicExpression<Number, Symbol>::operator~ ();

	t = ~t;
	Size = 0;
	return *this;
}

Expression &Expression::operator&= (const Expression &e)
{
	BasicExpression<Number, Symbol>::operator&= (e);

	t &= e.t;
	Size = 0;
	return *this;
}

Expression &Expression::operator|= (const Expression &e)
{
	BasicExpression<Number, Symbol>::operator|= (e);

	t |= e.t;
	Size = 0;
	return *this;
}

Expression &Expression::operator^= (const Expression &e)
{
	BasicExpression<Number, Symbol>::operator^= (e);

	t ^= e.t;
	Size = 0;
	return *this;
}

Expression &Expression::operator<<= (const Expression &e)
{
	BasicExpression<Number, Symbol>::operator<<= (e);

	t <<= e.t;
	Size = 0;
	return *this;
}

Expression &Expression::operator>>= (const Expression &e)
{
	BasicExpression<Number, Symbol>::operator>>= (e);

	t >>= e.t;
	Size = 0;
	return *this;
}
