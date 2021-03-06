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
	delete Data;
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

Expression &Expression::Compose (Expression &e)
{
	Data = Data->Compose (*e.Data);

	// Takes the second argument's data to prevent the evaluator to delete it
	// (because it is now part of the result)
	e.Data = 0;
	return *this;
}

Expression &Expression::AddToList (Expression &e)
{
	Data = DupExpression::AddToList (Data, e.Data);
	e.Data = 0;
	return *this;
}

Expression &Expression::DUP (Expression &e)
{
	Data = DupExpression::DUP (Data, e.Data);

	// Takes the second argument's data to prevent the evaluator to delete it
	// (because it is now part of the result)
	e.Data = 0;
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

void ExpressionData::SetSize (unsigned int s, Number::NumberType nt = Number::ANY) const throw (InvalidSize, CastFailed, CastConflict)
{
	const_cast<ExpressionData *> (this)->Type::SetSize (s);
}

void ExpressionData::SetDistanceType (int dist) const throw (InvalidSizeCast, CastConflict)
{
	const_cast<ExpressionData *> (this)->Type::SetDistanceType (dist);
}

const SimpleExpression *ExpressionData::GetSimpleExpression() const throw ()
{
	return dynamic_cast <const SimpleExpression *> (this);
}

SimpleExpression *ExpressionData::GetSimpleExpression() throw ()
{
	return dynamic_cast <SimpleExpression *> (this);
}

const DupExpression *ExpressionData::GetDupExpression() const throw ()
{
	return dynamic_cast <const DupExpression *> (this);
}

DupExpression *ExpressionData::GetDupExpression() throw ()
{
	return dynamic_cast <DupExpression *> (this);
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
				ExpressionData::SetSize (s);
				return;
			}
		}

		// If no constant term found, add a dummy zero. This is required for compatibility with NASM
		Number *dummy = new Number (0);
		dummy->SetSize (s, nt);
		This->Value += BasicExpression<Number, Symbol> (dummy, 0);
	}

	// STRONG or WEAK memory references are allowed to change the Size at will
	ExpressionData::SetSize (s);
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

		if (e.SegmentPrefix != 0)
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

SimpleExpression *SimpleExpression::Compose (ExpressionData &exp)
{
	SimpleExpression *e = exp.GetSimpleExpression();
	if (e == 0)
		throw 0;

	if ((SegmentPrefix != 0) || (e->SegmentPrefix != 0))
		throw UnexpectedSegmentPrefix (*e);

	e->SegmentPrefix = this;
	return e;
}

DupExpression::DupExpression (const Type &t) throw () : ExpressionData(t)
{
	Count = 0;
}

DupExpression::DupExpression (const DupExpression &t) throw () : ExpressionData(t)
{
	Count = (t.Count == 0) ? 0 : t.Count->Clone();

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
	string s;

	if (Count != 0)
	{
		s = Count->Print();
		s += " DUP ";
	}

	s += "(";

	s += DupList.front()->Print();

	for (vector<ExpressionData *>::const_iterator i = DupList.begin() + 1; i != DupList.end(); i++)
	{
		s += ", ";
		s += (*i)->Print();
	}

	s += ")";
	return s;
}

void DupExpression::Write (vector<Byte> &Output) const throw ()
{
	if (GetSize() == 0)
		throw 0;

	unsigned int sz1 = Output.size();

	try
	{
		for (vector <ExpressionData *>::const_iterator i = DupList.begin(); i != DupList.end(); i++)
			(*i)->Write (Output);

		if (Count != 0)
		{
			const Number *num = Count->GetNumber();
			if (num == 0)
				throw 0;

			unsigned long int len1 = Output.size() - sz1;
			unsigned long int n1 = num->GetUnsignedLong();

			if (n1 == 0)
				throw 0;

			Output.resize (sz1 + len1 * n1);
			vector<Byte>::iterator i = Output.begin() + sz1;
			vector<Byte>::iterator j = i + len1;

			for (; j != Output.end(); i++, j++)
				*j = *i;
		}
	}
	catch (...)
	{
		Output.resize(sz1);
		throw;
	}
}

void DupExpression::SetSize (unsigned int s, Number::NumberType nt = Number::ANY) const throw (InvalidSize, CastFailed, CastConflict)
{
	for (vector <ExpressionData *>::const_iterator i = DupList.begin(); i != DupList.end(); i++)
		(*i)->SetSize (s, nt);

	ExpressionData::SetSize (s, nt);
}

void DupExpression::SetDistanceType (int dist) const throw (InvalidSizeCast, CastConflict)
{
}

void DupExpression::SetNumericalType (int num) throw ()
{
}

DupExpression &DupExpression::operator= (const ExpressionData &e)
{
	if (&e != this)
	{
		const DupExpression *exp = e.GetDupExpression();
		if (exp == 0)
			throw 0;

		// Copies the Count member
		delete Count;
		Count = (exp->Count == 0) ? 0 : exp->Count->Clone();

		// Erases the old DupList
		for (vector<ExpressionData *>::iterator i = DupList.begin(); i != DupList.end(); i++)
			delete *i;

		DupList.clear();

		// Duplicates new DupList
		for (vector<ExpressionData *>::const_iterator i = exp->DupList.begin(); i != exp->DupList.end(); i++)
			DupList.push_back ((*i)->Clone());
	}

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

DupExpression *DupExpression::Compose (ExpressionData &e)
{
	throw 0;
	return this;
}

ExpressionData *DupExpression::AddToList (ExpressionData *e1, ExpressionData *e2)
{
	DupExpression *dexp = e1->GetDupExpression();

	if (dexp != 0)
	{
		// If there is no Count, just append e2 contents to the sequence
		if (dexp->Count == 0)
		{
			dexp->DupList.push_back (e2);
			return dexp;
		}
	}

	// Replace e1 contents with a DupExpression containing the former data
	dexp = new DupExpression (*e1);
	dexp->DupList.push_back (e1);

	// Adds the second expression and returns
	dexp->DupList.push_back (e2);
	return dexp;
}

ExpressionData *DupExpression::DUP (ExpressionData *e1, ExpressionData *e2)
{
	// First argument must be a SimpleExpression
	SimpleExpression *sexp = e1->GetSimpleExpression();
	if (sexp == 0)
		throw 0;

	// If Count is NULL, use it to store the counter
	DupExpression *dexp = e2->GetDupExpression();
	if (dexp != 0)
	{
		if (dexp->Count == 0)
		{
			dexp->Count = sexp;
			return dexp;
		}
	}

	// If we do not have a DupExpression or its Count is not NULL, create another one.
	dexp = new DupExpression (*e2);
	dexp->DupList.push_back (e2);
	dexp->Count = sexp;
	return dexp;
}
