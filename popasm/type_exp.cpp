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

#include "register.h"
#include "type_exp.h"
#include "symbol.h"
#include <algorithm>

void Expression::SetSize (unsigned int s, Number::NumberType nt = Number::ANY) throw (InvalidSize, CastFailed)
{
	if (t == Type::SCALAR)
	{
		// Searches the constant term in the expression and set its size. This is necessary to
		// make [BYTE eax+3] be the same as [eax + BYTE 3]
		for (vector<Term *>::iterator i = Terms.begin(); i != Terms.end(); i++)
		{
			if ((*i)->Constant())
			{
				(*i)->first->SetSize (s, nt);
				return;
			}
		}

		// If no constant term found, add a dummy zero. This is required for compatibility with NASM
		Number *dummy = new Number (0);
		dummy->SetSize (s, nt);
		BasicExpression<Number, Symbol>::operator+= (BasicExpression<Number, Symbol> (dummy, 0));
	}
	else
	{
		// STRONG or WEAK memory. The user is allowed to change the Size at will
		t.SetSize(s);
	}
}

void Expression::SetDistanceType (int dist) throw (InvalidSizeCast)
{
	if (t.GetCurrentType() != Type::STRONG_MEMORY)
	{
		// Must be a single term
		if (QuantityOfTerms() != 1) throw InvalidSizeCast();

		const pair<Number *, Symbol *> *x = TermAt (0);
		if (x->second != 0)
		{
			// Cannot have "near ax"
			if (dynamic_cast<const Register *> (x->second) != 0) throw InvalidSizeCast();
		}
	}

	t.SetDistanceType(dist);
}

Expression::Expression (Number *n, Symbol *s) throw () : BasicExpression<Number, Symbol> (n, s)
{
	t = Type (0, Type::SCALAR, UNDEFINED);
	SegmentPrefix = 0;
}

Expression::operator Symbol *() const
{
	if (Terms.size() != 1) throw SymbolExpected (*this);
	if (Terms.back()->first != 0) throw SymbolExpected (*this);
	if (Terms.back()->second == 0) throw SymbolExpected (*this);
	return Terms.back()->second;
}

Expression &Expression::operator=  (const Expression &e) throw ()
{
	BasicExpression<Number, Symbol>::operator= (e);

	delete SegmentPrefix;
	if (e.SegmentPrefix == 0)
		SegmentPrefix = 0;
	else
		SegmentPrefix = e.SegmentPrefix->Clone();

	t = e.t;
	return *this;
}

Expression &Expression::operator+= (const Expression &e)
{
	if (e.SegmentPrefix != 0)
	{
		if (SegmentPrefix != 0) throw UnexpectedSegmentPrefix (e);
		SegmentPrefix = new Expression (*e.SegmentPrefix);
	}

	BasicExpression<Number, Symbol>::operator+= (e);
	t += e.t;
	return *this;
}

Expression &Expression::operator-= (const Expression &e)
{
	if (e.SegmentPrefix != 0) throw UnexpectedSegmentPrefix (e);

	BasicExpression<Number, Symbol>::operator-= (e);

	t -= e.t;
	return *this;
}

Expression &Expression::operator*= (const Expression &e)
{
	if (e.SegmentPrefix != 0) throw UnexpectedSegmentPrefix (e);

	BasicExpression<Number, Symbol>::operator*= (e);

	t *= e.t;
	return *this;
}

Expression &Expression::operator/= (const Expression &e)
{
	if (e.SegmentPrefix != 0) throw UnexpectedSegmentPrefix (e);

	BasicExpression<Number, Symbol>::operator/= (e);

	t /= e.t;
	return *this;
}

Expression &Expression::operator%= (const Expression &e)
{
	if (e.SegmentPrefix != 0) throw UnexpectedSegmentPrefix (e);

	BasicExpression<Number, Symbol>::operator%= (e);

	t %= e.t;
	return *this;
}

Expression &Expression::operator- ()
{
	BasicExpression<Number, Symbol>::operator- ();

	t = -t;
	return *this;
}

Expression &Expression::operator~ ()
{
	BasicExpression<Number, Symbol>::operator~ ();

	t = ~t;
	return *this;
}

Expression &Expression::operator&= (const Expression &e)
{
	if (e.SegmentPrefix != 0) throw UnexpectedSegmentPrefix (e);

	BasicExpression<Number, Symbol>::operator&= (e);

	t &= e.t;
	return *this;
}

Expression &Expression::operator|= (const Expression &e)
{
	if (e.SegmentPrefix != 0) throw UnexpectedSegmentPrefix (e);

	BasicExpression<Number, Symbol>::operator|= (e);

	t |= e.t;
	return *this;
}

Expression &Expression::operator^= (const Expression &e)
{
	if (e.SegmentPrefix != 0) throw UnexpectedSegmentPrefix (e);

	BasicExpression<Number, Symbol>::operator^= (e);

	t ^= e.t;
	return *this;
}

Expression &Expression::operator<<= (const Expression &e)
{
	if (e.SegmentPrefix != 0) throw UnexpectedSegmentPrefix (e);

	BasicExpression<Number, Symbol>::operator<<= (e);

	t <<= e.t;
	return *this;
}

Expression &Expression::operator>>= (const Expression &e)
{
	if (e.SegmentPrefix != 0) throw UnexpectedSegmentPrefix (e);

	BasicExpression<Number, Symbol>::operator>>= (e);

	t >>= e.t;
	return *this;
}

Expression &Expression::MemberSelect (const Expression &e)
{
	if (e.SegmentPrefix != 0) throw UnexpectedSegmentPrefix (e);

	// Gets a pointer to the aggregate instance
	Symbol *s1 = static_cast<Symbol *> (*this);
	const AggregateInstance *ai = dynamic_cast<const AggregateInstance *> (s1->GetData());
	if (ai == 0) throw AggregateExpected (*s1->GetData());

	// Gets a pointer to the requested member
	Symbol *s2 = static_cast<Symbol *> (e);
	const Variable *v = ai->GetFather()->FindMember (s2->GetName());

	// Checks if the member exists
	if (v == 0) throw NotAMember (ai->GetName(), s2->GetName());

	// Adds the base offset and the member displacement within the structure
	Variable *v2 = new Variable (ai->GetName() + "." + v->GetName(), ai->GetOffset() + v->GetOffset(), v->GetSize(), v->GetLength());

	// Replaces the contents of this expression with a weak memory variable
	s1->SetData (v2, true);
	t.SetCurrentType (Type::WEAK_MEMORY);
	return *this;
}

Expression &Expression::Compose (const Expression &e)
{
	if ((SegmentPrefix != 0) || (e.SegmentPrefix != 0)) throw UnexpectedSegmentPrefix (e);

	Expression *seg = Clone();
	(*this) = e;
	SegmentPrefix = seg;
	return *this;
}

const pair<Number *, Symbol *> *Expression::TermAt (unsigned int n) const throw ()
{
	if (n >= Terms.size()) return 0;
	return Terms[n];
}
