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

Expression::Expression () throw ()
{
	Data = new SimpleExpression ();
}

Expression::Expression (Number *n, Symbol *v, const Type &t) throw ()
{
	Data = new SimpleExpression (n, v, t);
}

Expression::Expression (const Expression &e) throw ()
{
	Data = e.Data->Clone();
}

Expression::~Expression () throw ()
{
}

Expression &Expression::operator[] (int)
{
	(*Data)[0];
	return *this;
}

void Expression::SetSize (unsigned int s, Number::NumberType nt = Number::ANY) const throw (InvalidSize, CastFailed, CastConflict)
{
	Data->SetSize (s, nt);
}

void Expression::SetDistanceType (int dist) const throw (InvalidSizeCast, CastConflict)
{
	Data->SetDistanceType (dist);
}

string Expression::Print () const throw ()
{
	return Data->Print();
}

void Expression::Write (vector<Byte> &Output) const throw ()
{
	Data->Write(Output);
}

const SimpleExpression *Expression::GetSegmentPrefix () const throw ()
{
	const SimpleExpression *exp = GetSimpleExpression ();
	if (exp == 0)
		return 0;

	return exp->GetSegmentPrefix();
}

const Symbol *Expression::GetSymbol () const throw ()
{
	const SimpleExpression *exp = GetSimpleExpression ();
	if (exp == 0)
		return 0;

	return exp->GetSymbol();
}

const Symbol *Expression::GetSymbol (const SimpleExpression * &Prefix) const throw ()
{
	const SimpleExpression *exp = GetSimpleExpression ();
	if (exp == 0)
		return 0;

	return exp->GetSymbol(Prefix);
}

const Number *Expression::GetNumber () const throw ()
{
	const SimpleExpression *exp = GetSimpleExpression ();
	if (exp == 0)
		return 0;

	return exp->GetNumber();
}

const Number *Expression::GetNumber (const SimpleExpression * &Prefix) const throw ()
{
	const SimpleExpression *exp = GetSimpleExpression ();
	if (exp == 0)
		return 0;

	return exp->GetNumber(Prefix);
}

Expression &Expression::operator=  (const Expression &e) throw ()
{
	delete Data;
	Data = e.Data->Clone();
	return *this;
}

Expression &Expression::operator+= (const Expression &e)
{
	*Data += *e.Data;
	return *this;
}

Expression &Expression::operator-= (const Expression &e)
{
	*Data -= *e.Data;
	return *this;
}

Expression &Expression::operator*= (const Expression &e)
{
	*Data *= *e.Data;
	return *this;
}

Expression &Expression::operator/= (const Expression &e)
{
	*Data /= *e.Data;
	return *this;
}

Expression &Expression::operator%= (const Expression &e)
{
	*Data %= *e.Data;
	return *this;
}

Expression &Expression::operator- ()
{
	-*Data;
	return *this;
}

Expression &Expression::operator~ ()
{
	~*Data;
	return *this;
}


Expression &Expression::operator&= (const Expression &e)
{
	*Data &= *e.Data;
	return *this;
}

Expression &Expression::operator|= (const Expression &e)
{
	*Data |= *e.Data;
	return *this;
}

Expression &Expression::operator^= (const Expression &e)
{
	*Data ^= *e.Data;
	return *this;
}

Expression &Expression::operator<<= (const Expression &e)
{
	*Data <<= *e.Data;
	return *this;
}

Expression &Expression::operator>>= (const Expression &e)
{
	*Data >>= *e.Data;
	return *this;
}

bool Expression::operator== (const Expression &e) const throw ()
{
	// Compare numerical values
	return *Data == *e.Data;
}

Expression &Expression::BinaryShiftRight (const Expression &e)
{
	Data->BinaryShiftRight (*e.Data);
	return *this;
}

Expression &Expression::UnsignedDivision (const Expression &e)
{
	Data->UnsignedDivision (*e.Data);
	return *this;
}

Expression &Expression::UnsignedModulus (const Expression &e)
{
	Data->UnsignedModulus (*e.Data);
	return *this;
}

Expression &Expression::MemberSelect (const Expression &e)
{
	// Member select here

	return *this;
}

Expression &Expression::Compose (const Expression &e)
{
	Data->Compose (*e.Data);
	return *this;
}

// This method must check compatibility between types before doing anything else
Expression &Expression::AddToList (const Expression &e)
{
/*
	DupExpression *dexp = GetDupExpression ();

	if (dexp == 0)
	{
		ReferenceCount<ExpressionData> *temp = Data;
		Data = new ReferenceCount<ExpressionData> (new DupExpression());
		Data->GetData().AddToList (temp);
	}

	Data->GetData().AddToList (e.Data);
*/
	return *this;
}

Expression &Expression::DUP (const Expression &e)
{
/*
	ReferenceCount<ExpressionData> *NewData = GetData().DUP (e);
	kill (Data);
	Data = NewData;
*/
	return *this;
}

const SimpleExpression *Expression::GetSimpleExpression() const throw ()
{
	return dynamic_cast <const SimpleExpression *> (& GetConstData());
}

const DupExpression *Expression::GetDupExpression() const throw ()
{
	return dynamic_cast <const DupExpression *> (& GetConstData());
}

ExpressionData::~ExpressionData () throw () {}

const SimpleExpression *ExpressionData::GetSimpleExpression() const throw ()
{
	return dynamic_cast <const SimpleExpression *> (this);
}

const DupExpression *ExpressionData::GetDupExpression() const throw ()
{
	return dynamic_cast <const DupExpression *> (this);
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
	SimpleExpression *This = const_cast<SimpleExpression *> (this);

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
				This->Type::SetSize (s);
				return;
			}
		}

		// If no constant term found, add a dummy zero. This is required for compatibility with NASM
		Number *dummy = new Number (0);
		dummy->SetSize (s, nt);
		This->Value += BasicExpression<Number, Symbol> (dummy, 0);
		This->Type::SetSize (s);
	}
	else
	{
		// STRONG or WEAK memory. The user is allowed to change the Size at will
		This->Type::SetSize(s);
	}
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
	const SimpleExpression *pref;
	const Symbol *sym;

	sym = GetSymbol (pref);
	return (pref == 0) ? sym : 0;
}

const Symbol *SimpleExpression::GetSymbol (const SimpleExpression * &Prefix) const throw ()
{
	Prefix = SegmentPrefix;
	return Value.GetVariable();
}

const Number *SimpleExpression::GetNumber () const throw ()
{
	const SimpleExpression *pref;
	const Number *num;

	num = GetNumber(pref);
	return (pref == 0) ? num : 0;
}

const Number *SimpleExpression::GetNumber (const SimpleExpression * &Prefix) const throw ()
{
	Prefix = SegmentPrefix;
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

SimpleExpression &SimpleExpression::operator=  (const SimpleExpression &e)
{
	// Prevents self-assignment
	if (&e != this)
	{
		Value = e.Value;
		delete SegmentPrefix;
		SegmentPrefix = e.SegmentPrefix->Clone();
		ExpressionData::operator= (e);
	}

	return *this;
}

SimpleExpression &SimpleExpression::operator+= (const ExpressionData &exp)
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

SimpleExpression &SimpleExpression::operator-= (const ExpressionData &exp)
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

SimpleExpression &SimpleExpression::operator*= (const ExpressionData &exp)
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

SimpleExpression &SimpleExpression::operator/= (const ExpressionData &exp)
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

SimpleExpression &SimpleExpression::operator%= (const ExpressionData &exp)
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

SimpleExpression &SimpleExpression::operator&= (const ExpressionData &exp)
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

SimpleExpression &SimpleExpression::operator|= (const ExpressionData &exp)
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

SimpleExpression &SimpleExpression::operator^= (const ExpressionData &exp)
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

SimpleExpression &SimpleExpression::operator<<= (const ExpressionData &exp)
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

SimpleExpression &SimpleExpression::operator>>= (const ExpressionData &exp)
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

SimpleExpression &SimpleExpression::BinaryShiftRight (const ExpressionData &exp)
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

SimpleExpression &SimpleExpression::UnsignedDivision (const ExpressionData &exp)
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

SimpleExpression &SimpleExpression::UnsignedModulus (const ExpressionData &exp)
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

SimpleExpression &SimpleExpression::MemberSelect (const ExpressionData &exp)
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

SimpleExpression &SimpleExpression::Compose (const ExpressionData &exp)
{
	const SimpleExpression *e = exp.GetSimpleExpression();
	if (e == 0)
		throw 0;

	if ((SegmentPrefix != 0) || (e->SegmentPrefix != 0))
		throw UnexpectedSegmentPrefix (*e);

	SimpleExpression *seg = Clone();
	(*this) = *e;
	SegmentPrefix = seg;

	return *this;
}

void SimpleExpression::AddToList (Expression &e1, const Expression &e2)
{
}

void SimpleExpression::DUP (Expression &e1, const Expression &e2)
{
}

DupExpression::DupExpression (const Type &t) throw () : ExpressionData(t)
{
	Count = 0;
}

DupExpression::DupExpression (const DupExpression &t) throw () : ExpressionData(t)
{
	Count = t.Count->Clone();

	for (vector<ExpressionData *>::const_iterator i = t.DupList.begin(); i != t.DupList.end(); i++)
		DupList.push_back ((*i)->Clone());
}

DupExpression::~DupExpression () throw ()
{
	delete Count;

	for (vector<ExpressionData *>::iterator i = DupList.begin(); i != DupList.end(); i++)
		delete *i;
}

string DupExpression::Print () const throw ()
{
	return string();
}

void DupExpression::Write (vector<Byte> &Output) const throw ()
{
}

void DupExpression::SetSize (unsigned int s, Number::NumberType nt = Number::ANY) const throw (InvalidSize, CastFailed, CastConflict)
{
}

void DupExpression::SetDistanceType (int dist) const throw (InvalidSizeCast, CastConflict)
{
}

void DupExpression::SetNumericalType (int num) throw ()
{
}

DupExpression &DupExpression::operator= (const ExpressionData &e)
{
	const DupExpression *exp = e.GetDupExpression();
	if (exp == 0)
		throw 0;

	delete Count;
	Count = exp->Count->Clone();

	for (vector<ExpressionData *>::iterator i = DupList.begin(); i != DupList.end(); i++)
		delete *i;

	DupList.clear();

	for (vector<ExpressionData *>::const_iterator i = exp->DupList.begin(); i != exp->DupList.end(); i++)
		DupList.push_back ((*i)->Clone());

	return *this;
}

DupExpression &DupExpression::operator+= (const ExpressionData &e)
{
	throw 0;
	return *this;
}

DupExpression &DupExpression::operator-= (const ExpressionData &e)
{
	throw 0;
	return *this;
}

DupExpression &DupExpression::operator*= (const ExpressionData &e)
{
	throw 0;
	return *this;
}

DupExpression &DupExpression::operator/= (const ExpressionData &e)
{
	throw 0;
	return *this;
}

DupExpression &DupExpression::operator%= (const ExpressionData &e)
{
	throw 0;
	return *this;
}

DupExpression &DupExpression::operator- ()
{
	throw 0;
	return *this;
}

DupExpression &DupExpression::operator~ ()
{
	throw 0;
	return *this;
}

DupExpression &DupExpression::operator&= (const ExpressionData &e)
{
	throw 0;
	return *this;
}

DupExpression &DupExpression::operator|= (const ExpressionData &e)
{
	throw 0;
	return *this;
}

DupExpression &DupExpression::operator^= (const ExpressionData &e)
{
	throw 0;
	return *this;
}

DupExpression &DupExpression::operator<<= (const ExpressionData &e)
{
	throw 0;
	return *this;
}

DupExpression &DupExpression::operator>>= (const ExpressionData &e)
{
	throw 0;
	return *this;
}

bool DupExpression::operator== (const DupExpression &exp) const throw ()
{
	if ((Count != 0) && (exp.Count != 0))
	{
		if (*Count != *exp.Count)
			return false;
	}
	else
		if (Count != exp.Count)
			return false;

	if (DupList.size() != exp.DupList.size())
		return false;

	vector<ExpressionData *>::const_iterator i, j;
	for (i = DupList.begin(), j = exp.DupList.begin(); i != DupList.end(); i++, j++)
	{
		if (**i != **j)
			return false;
	}

	return true;
}

bool DupExpression::operator== (const Expression &e) const throw ()
{
	const DupExpression *exp = e.GetDupExpression();
	if (exp == 0)
		throw 0;

	return operator== (*exp);
}

DupExpression &DupExpression::BinaryShiftRight (const ExpressionData &e)
{
	throw 0;
	return *this;
}

DupExpression &DupExpression::UnsignedDivision (const ExpressionData &e)
{
	throw 0;
	return *this;
}

DupExpression &DupExpression::UnsignedModulus (const ExpressionData &e)
{
	throw 0;
	return *this;
}

DupExpression &DupExpression::MemberSelect (const ExpressionData &e)
{
	throw 0;
	return *this;
}

DupExpression &DupExpression::Compose (const ExpressionData &e)
{
	throw 0;
	return *this;
}

void DupExpression::AddToList (Expression &e1, const Expression &e2)
{
}
