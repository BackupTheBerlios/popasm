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
#include "variable.h"
#include "immed.h"

#include <algorithm>

Expression::Expression (Number *n, Symbol *v, const Type &t) throw ()
{
	Data = new ReferenceCount<ExpressionData> (new SimpleExpression (n, v, t));
}

Expression &Expression::operator[] (int)
{
	GetData()[0];
	return *this;
}

void Expression::SetSize (unsigned int s, Number::NumberType nt = Number::ANY) const throw (InvalidSize, CastFailed, CastConflict)
{
	GetConstData().SetSize (s, nt);
}

void Expression::SetDistanceType (int dist) const throw (InvalidSizeCast, CastConflict)
{
	GetConstData().SetDistanceType (dist);
}

string Expression::Print () const throw ()
{
	return GetConstData().Print();
}

void Expression::Write (vector<Byte> &Output) const throw ()
{
	return GetConstData().Write(Output);
}

Expression &Expression::operator=  (const Expression &e) throw ()
{
	kill (Data);
	Data = e.Data->Clone();
	return *this;
}

Expression &Expression::operator+= (const Expression &e)
{
	GetData() += e;
	return *this;
}

Expression &Expression::operator-= (const Expression &e)
{
	GetData() -= e;
	return *this;
}

Expression &Expression::operator*= (const Expression &e)
{
	GetData() *= e;
	return *this;
}

Expression &Expression::operator/= (const Expression &e)
{
	GetData() /= e;
	return *this;
}

Expression &Expression::operator%= (const Expression &e)
{
	GetData() %= e;
	return *this;
}

Expression &Expression::operator- ()
{
	-GetData();
	return *this;
}

Expression &Expression::operator~ ()
{
	~GetData();
	return *this;
}


Expression &Expression::operator&= (const Expression &e)
{
	GetData() &= e;
	return *this;
}

Expression &Expression::operator|= (const Expression &e)
{
	GetData() |= e;
	return *this;
}

Expression &Expression::operator^= (const Expression &e)
{
	GetData() ^= e;
	return *this;
}

Expression &Expression::operator<<= (const Expression &e)
{
	GetData() <<= e;
	return *this;
}

Expression &Expression::operator>>= (const Expression &e)
{
	GetData() >>= e;
	return *this;
}

bool Expression::operator== (const Expression &e) const throw ()
{
	// Check if they are identical
	if (Data == e.Data)
		return true;

	// Compare numerical values
	return GetConstData() == e;
}

Expression &Expression::BinaryShiftRight (const Expression &e)
{
	GetData().BinaryShiftRight (e);
	return *this;
}

Expression &Expression::UnsignedDivision (const Expression &e)
{
	GetData().UnsignedDivision (e);
	return *this;
}

Expression &Expression::UnsignedModulus (const Expression &e)
{
	GetData().UnsignedModulus (e);
	return *this;
}

Expression &Expression::MemberSelect (const Expression &e)
{
	GetData().MemberSelect (e);
	return *this;
}

Expression &Expression::Compose (const Expression &e)
{
	GetData().Compose (e);
	return *this;
}

Expression &Expression::AddToList (const Expression &e)
{
	ReferenceCount<ExpressionData> *NewData = GetData().AddToList (e);

	kill (Data);
	Data = NewData;
	return *this;
}

Expression &Expression::DUP (const Expression &e)
{
	ReferenceCount<ExpressionData> *NewData = GetData().DUP (e);
	kill (Data);
	Data = NewData;
	return *this;
}

const SimpleExpression *Expression::GetSimpleExpression() const throw ()
{
	return dynamic_cast <const SimpleExpression *> (& GetConstData());
}

ExpressionData::~ExpressionData () throw () {}

ReferenceCount<ExpressionData> *ExpressionData::AddToList (const Expression &e)
{
	// Must check if types conflict
	return 0;
}

SimpleExpression::SimpleExpression (Number *n, Symbol *s, const Type &tt) throw () : ExpressionData (tt), Value (n, s)
{
	SegmentPrefix = 0;

	if (n != 0)
		SetNumericalType (n->IsInteger() ? INTEGER : FLOAT);
}

SimpleExpression::SimpleExpression (const SimpleExpression &e) : ExpressionData (e), Value (e.Value)
{
	SegmentPrefix = (e.SegmentPrefix == 0) ? 0 : e.SegmentPrefix->Clone();
}

void SimpleExpression::SetSize (unsigned int s, Number::NumberType nt = Number::ANY) const throw (InvalidSize, CastFailed, CastConflict)
{
/*
	if (!Type::CombineSD (s, GetDistanceType()))
		throw CastConflict (Type::PrintDistance(GetDistanceType()), Type::PrintSize(s));

	if (GetCurrentType() == Type::SCALAR)
	{
		// Searches the constant term in the expression and set its size. This is necessary to
		// make [BYTE eax+3] be the same as [eax + BYTE 3]
		for (BasicExpression<Number, Symbol>::const_iterator i = Value.begin(); i != Value.end(); i++)
		{
			if ((*i)->Constant())
			{
				(*i)->first->SetSize (s, nt);
				Type::SetSize (s);
				return;
			}
		}

		// If no constant term found, add a dummy zero. This is required for compatibility with NASM
		Number *dummy = new Number (0);
		dummy->SetSize (s, nt);
		Value += BasicExpression<Number, Symbol> (dummy, 0);
		Type::SetSize (s);
	}
	else
	{
		// STRONG or WEAK memory. The user is allowed to change the Size at will
		Type::SetSize(s);
	}
*/
}

void SimpleExpression::SetDistanceType (int dist) const throw (InvalidSizeCast, CastConflict)
{
	if (!Type::CombineSD (GetSize(), dist))
		throw CastConflict (Type::PrintSize(GetSize()), Type::PrintDistance(dist));

	if (GetCurrentType() != Type::STRONG_MEMORY)
	{
		const Symbol *s = Value.GetVariable ();

		// Must be a single term
		if (Value.QuantityOfTerms() != 1)
			throw InvalidSizeCast();

		if (s != 0)
		{
			// Cannot have "near ax"
			if (dynamic_cast<const Register *> (s->GetData()) != 0)
				throw InvalidSizeCast();
		}
	}

	const_cast<SimpleExpression *>(this)->Type::SetDistanceType(dist);
}

const Symbol *SimpleExpression::GetSymbol () const throw ()
{
	return Value.GetVariable();
}

const Number *SimpleExpression::GetNumber () const throw ()
{
	return Value.GetNumber();
}

void SimpleExpression::Write (vector<Byte> &Output) const
{
	const Number *n = Value.GetNumber();
	if (n != 0)
		n->Write (Output);
	else
		throw 0;
}

const SimpleExpression *SimpleExpression::GetSegmentPrefix () const throw ()
{
	if (SegmentPrefix == 0)
		return 0;

	return &SegmentPrefix->GetData();
}

SimpleExpression &SimpleExpression::operator+= (const Expression &exp)
{
	const SimpleExpression *e = exp.GetSimpleExpression();
	if (e == 0)
		throw 0;

	if (e->SegmentPrefix != 0)
	{
		if (SegmentPrefix != 0)
			throw UnexpectedSegmentPrefix (*e);

		SegmentPrefix = e->SegmentPrefix->Clone();
	}

	Value += e->Value;
	ExpressionData::operator+= (exp);
	return *this;
}

SimpleExpression &SimpleExpression::operator-= (const Expression &exp)
{
	const SimpleExpression *e = exp.GetSimpleExpression();
	if (e == 0)
		throw 0;

	if (e->SegmentPrefix != 0)
		throw UnexpectedSegmentPrefix (*e);

	Value -= e->Value;
	ExpressionData::operator-= (exp);
	return *this;
}

SimpleExpression &SimpleExpression::operator*= (const Expression &exp)
{
	const SimpleExpression *e = exp.GetSimpleExpression();
	if (e == 0)
		throw 0;

	if (e->SegmentPrefix != 0)
		throw UnexpectedSegmentPrefix (*e);

	Value *= e->Value;
	ExpressionData::operator*= (exp);
	return *this;
}

SimpleExpression &SimpleExpression::operator/= (const Expression &exp)
{
	const SimpleExpression *e = exp.GetSimpleExpression();
	if (e == 0)
		throw 0;

	if (e->SegmentPrefix != 0)
		throw UnexpectedSegmentPrefix (*e);

	Value /= e->Value;
	ExpressionData::operator/= (exp);
	return *this;
}

SimpleExpression &SimpleExpression::operator%= (const Expression &exp)
{
	const SimpleExpression *e = exp.GetSimpleExpression();
	if (e == 0)
		throw 0;

	if (e->SegmentPrefix != 0)
		throw UnexpectedSegmentPrefix (*e);

	Value %= e->Value;
	ExpressionData::operator%= (exp);
	return *this;
}

SimpleExpression &SimpleExpression::operator- ()
{
	-Value;
	ExpressionData::operator-();
	return *this;
}

SimpleExpression &SimpleExpression::operator~ ()
{
	~Value;
	ExpressionData::operator~();
	return *this;
}

SimpleExpression &SimpleExpression::operator&= (const Expression &exp)
{
	const SimpleExpression *e = exp.GetSimpleExpression();
	if (e == 0)
		throw 0;

	if (e->SegmentPrefix != 0)
		throw UnexpectedSegmentPrefix (*e);

	Value &= e->Value;
	ExpressionData::operator&= (exp);
	return *this;
}

SimpleExpression &SimpleExpression::operator|= (const Expression &exp)
{
	const SimpleExpression *e = exp.GetSimpleExpression();
	if (e == 0)
		throw 0;

	if (e->SegmentPrefix != 0)
		throw UnexpectedSegmentPrefix (*e);

	Value |= e->Value;
	ExpressionData::operator|= (exp);
	return *this;
}

SimpleExpression &SimpleExpression::operator^= (const Expression &exp)
{
	const SimpleExpression *e = exp.GetSimpleExpression();
	if (e == 0)
		throw 0;

	if (e->SegmentPrefix != 0)
		throw UnexpectedSegmentPrefix (*e);

	Value ^= e->Value;
	ExpressionData::operator^= (exp);
	return *this;
}

SimpleExpression &SimpleExpression::operator<<= (const Expression &exp)
{
	const SimpleExpression *e = exp.GetSimpleExpression();
	if (e == 0)
		throw 0;

	if (e->SegmentPrefix != 0)
		throw UnexpectedSegmentPrefix (*e);

	Value <<= e->Value;
	ExpressionData::operator<<= (exp);
	return *this;
}

SimpleExpression &SimpleExpression::operator>>= (const Expression &exp)
{
	const SimpleExpression *e = exp.GetSimpleExpression();
	if (e == 0)
		throw 0;

	if (e->SegmentPrefix != 0)
		throw UnexpectedSegmentPrefix (*e);

	Value >>= e->Value;
	ExpressionData::operator>>= (exp);
	return *this;
}

bool SimpleExpression::operator== (const ExpressionData &e) const throw ()
{
	const SimpleExpression *exp = dynamic_cast<const SimpleExpression *> (&e);
	if (exp == 0)
		return false;

	if (SegmentPrefix == 0)
	{
		if (exp->SegmentPrefix != 0)
			return false;
	}
	else
	{
		if (exp->SegmentPrefix == 0)
			return false;

		if (*SegmentPrefix != *exp->SegmentPrefix)
			return false;
	}

	return ExpressionData::operator== (e) && Value == exp->Value;
}

bool SimpleExpression::operator== (const Expression &exp) const throw ()
{
	const SimpleExpression *e = exp.GetSimpleExpression();
	if (e == 0)
		throw 0;

	return (*this)==*e;
}

SimpleExpression &SimpleExpression::BinaryShiftRight (const Expression &exp)
{
	const SimpleExpression *e = exp.GetSimpleExpression();
	if (e == 0)
		throw 0;

	if (e->SegmentPrefix != 0)
		throw UnexpectedSegmentPrefix (*e);

	Value.BinaryShiftRight (e->Value);

	// The effect on the type is the same as the arithmetic counterpart
	ExpressionData::operator>>= (exp);
	return *this;
}

SimpleExpression &SimpleExpression::UnsignedDivision (const Expression &exp)
{
	const SimpleExpression *e = exp.GetSimpleExpression();
	if (e == 0)
		throw 0;

	if (e->SegmentPrefix != 0)
		throw UnexpectedSegmentPrefix (*e);

	Value.UnsignedDivision (e->Value);

	// The effect on the type is the same as the signed counterpart
	ExpressionData::operator/= (exp);
	return *this;
}

SimpleExpression &SimpleExpression::UnsignedModulus (const Expression &exp)
{
	const SimpleExpression *e = exp.GetSimpleExpression();
	if (e == 0)
		throw 0;

	if (e->SegmentPrefix != 0)
		throw UnexpectedSegmentPrefix (*e);

	Value.UnsignedModulus (e->Value);

	// The effect on the type is the same as the arithmetic counterpart
	ExpressionData::operator%= (exp);
	return *this;
}

SimpleExpression &SimpleExpression::MemberSelect (const Expression &exp)
{
/*
	if (e->SegmentPrefix != 0)
		throw UnexpectedSegmentPrefix (*e);

	// Gets a pointer to the aggregate instance
	Symbol *s1 = GetSymbol();
	const AggregateInstance *ai = dynamic_cast<const AggregateInstance *> (s1->GetData());
	if (ai == 0) throw AggregateExpected (*s1->GetData());

	// Gets a pointer to the requested member
	Symbol *s2 = e.GetSymbol();
	const Variable *v = ai->GetFather()->FindMember (s2->GetName());

	// Checks if the member exists
	if (v == 0) throw NotAMember (ai->GetName(), s2->GetName());

	// Adds the base offset and the member displacement within the structure
	Variable *v2 = new Variable (ai->GetName() + "." + v->GetName(), ai->GetOffset() + v->GetOffset(), v->GetSize(), v->GetLength());

	// Replaces the contents of this expression with a weak memory variable
	s1->SetData (v2, true);
	SetCurrentType (Type::WEAK_MEMORY);
*/
	return *this;
}

SimpleExpression &SimpleExpression::Compose (const Expression &exp)
{
/*
	if ((SegmentPrefix != 0) || (e.SegmentPrefix != 0))
		throw UnexpectedSegmentPrefix (*e);

	SimpleExpression *seg = Clone();
	(*this) = e;
	SegmentPrefix = seg;
*/
	return *this;
}

ReferenceCount<ExpressionData> *SimpleExpression::AddToList (const Expression &e)
{
	return 0;
}

ReferenceCount<ExpressionData> *SimpleExpression::DUP (const Expression &e)
{
	return 0;
}
