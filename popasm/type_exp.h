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
#include <utility>

#include "express.h"
#include "lexsym.h"
#include "lexnum.h"
#include "type.h"

class UnexpectedSegmentPrefix;
class InvalidSizeCast;

class Expression : BasicExpression<Number, Symbol>
{
	Type t;								// Expression type
	Expression *SegmentPrefix;		// Segment prefix (0 if none)

	public:
	Expression () throw () : t(0, Type::SCALAR, Type::NONE), SegmentPrefix(0) {}
	Expression (Number *n, Symbol *s) throw ();
	Expression (const Expression &e) : BasicExpression<Number, Symbol> (e)
	{
		t = e.t;
		SegmentPrefix = (e.SegmentPrefix == 0) ? 0 : e.SegmentPrefix->Clone();
	}

	~Expression () throw () {delete SegmentPrefix;}
	virtual Expression *Clone() const {return new Expression(*this);}

	unsigned int GetSize () const throw () {return t.GetSize();}
	void SetSize (unsigned int s, Number::NumberType nt = Number::ANY) throw (InvalidSize, CastFailed);
	Type::TypeName GetType () const throw () {return t.GetCurrentType();}
	const Expression *GetSegmentPrefix () const throw () {return SegmentPrefix;}

	Type::Distance GetDistanceType () const throw () {return t.GetDistanceType();}
	void SetDistanceType (Type::Distance dist) throw (InvalidSizeCast);

	unsigned int QuantityOfTerms () const throw () {return Terms.size();}

	// Attempts to convert an expression to a symbol. Succeeds only if expression is in the form ((0, Symbol*))
	operator Symbol *() const;

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
	Expression &MemberSelect (const Expression &e);
	Expression &Compose (const Expression &e);

	const pair<Number *, Symbol *> *TermAt (unsigned int n) const throw ();
	string Print () const throw ()
	{
		string s;

		if (SegmentPrefix != 0)	s += SegmentPrefix->Print() + ":";

		s += BasicExpression<Number, Symbol>::Print();
		return s;
	}
};

class UnexpectedSegmentPrefix : public ExpressionException
{
	public:
	UnexpectedSegmentPrefix (const Expression &e) : ExpressionException ("Unexpected segment prefix: ") {WhatString += e.Print();}
	~UnexpectedSegmentPrefix () {}
};

class SymbolExpected : public ExpressionException
{
	public:
	SymbolExpected (const Expression &e) : ExpressionException ("Symbol expected, got ") {WhatString += e.Print();}
	~SymbolExpected () {}
};

class AggregateExpected : public ExpressionException
{
	public:
	AggregateExpected (const BasicSymbol &s) : ExpressionException ("Aggregate type expected, got ") {WhatString += s.Print();}
	~AggregateExpected () {}
};

class InvalidArgument : public ExpressionException
{
	public:
	InvalidArgument (const Expression &e) : ExpressionException ("Invalid argument: ") {WhatString += e.Print();}
	~InvalidArgument () {}
};

class NotAMember : public ExpressionException
{
	public:
	NotAMember (const string &s1, const string &s2) : ExpressionException (s2 + " is not a member of " + s1) {}
	~NotAMember () {}
};

class InvalidSizeCast : public ExpressionException
{
	public:
	InvalidSizeCast () throw () : ExpressionException ("Cannot size cast this expression. Must be a memory reference or immediate.") {}
	~InvalidSizeCast () throw () {}
};

#endif
