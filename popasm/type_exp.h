/***************************************************************************
                          type_exp.h  -  description
                             -------------------
    begin                : Thu Jun 6 2002
    copyright            : (C) 2002 by Helcio Mello
    email                : helcio@users.sf.net
 ***************************************************************************/

//--------------------------------------------------------------------------
// Expressions that keep track of their types
//--------------------------------------------------------------------------

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TYPE_EXP_H
#define TYPE_EXP_H

#include <exception>

#include "express.h"
#include "lexsym.h"
#include "lexnum.h"
#include "type.h"

class Expression : BasicExpression<Number, Symbol>
{
	Type t;					// Expression type
	unsigned int Size;	// Explicit size in bits, if any. Must be multiple of 8. Zero means no size restrictions

	public:
	Expression () throw () : t(Type::SCALAR), Size(0) {}
	Expression (Number *n, Symbol *s) throw ();
	~Expression () throw () {}

	unsigned int GetSize () const throw () {return Size;}
	void SetSize (unsigned int s, Number::NumberType = Number::SIGNED) throw (InvalidSize, CastFailed);

	Expression &operator=  (const Expression &e) throw ();
	Expression &operator+= (const Expression &e);
	Expression &operator-= (const Expression &e);
	Expression &operator*= (const Expression &e);
	Expression &operator/= (const Expression &e);
	Expression &operator%= (const Expression &e);

	Expression &operator- ();
	Expression &operator~ ();

	Expression &operator&= (const Expression &e);
	Expression &operator|= (const Expression &e);
	Expression &operator^= (const Expression &e);
	Expression &operator<<= (const Expression &e);
	Expression &operator>>= (const Expression &e);

	void operator[] (int) {t[0];}

	string Print () const throw () {return BasicExpression<Number, Symbol>::Print();}
};

#endif
