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
#include <string>
#include <utility>
#include <vector>

#include "express.h"
#include "lexsym.h"
#include "lexnum.h"
#include "type.h"
#include "defs.h"

class UnexpectedSegmentPrefix;
class InvalidSizeCast;

class ExpressionData;
class SimpleExpression;
class DupExpression;

class Expression
{
	ExpressionData *Data;

	public:
	Expression () throw ();
	Expression (Number *n, Symbol *v, const Type &t = Type ()) throw ();
	Expression (const Expression &e) throw ();
	~Expression () throw ();

	ExpressionData &GetData () throw () {return *Data;}
	const ExpressionData &GetConstData () const throw () {return *Data;}
	void SetSize (unsigned int s, Number::NumberType nt = Number::ANY) const throw (InvalidSize, CastFailed, CastConflict);
	void SetDistanceType (int dist) const throw (InvalidSizeCast, CastConflict);

	const SimpleExpression *GetSegmentPrefix () const throw ();
	const Symbol *GetSymbol () const throw ();
	const Symbol *GetSymbol (const SimpleExpression * &Prefix) const throw ();
	const Number *GetNumber () const throw ();
	const Number *GetNumber (const SimpleExpression * &Prefix) const throw ();

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
	Expression &operator[] (int);
	bool operator== (const Expression &e) const throw ();

	// Methods for implementing NASM >>, // and %% operators
	Expression &BinaryShiftRight (const Expression &e);
	Expression &UnsignedDivision (const Expression &e);
	Expression &UnsignedModulus (const Expression &e);

	Expression &MemberSelect (const Expression &e);
	Expression &Compose (const Expression &e);
	Expression &AddToList (const Expression &e);
	Expression &DUP (const Expression &e);

	const SimpleExpression *GetSimpleExpression() const throw ();
	const DupExpression *GetDupExpression() const throw ();
	string Print () const throw ();
	void Write (vector<Byte> &Output) const throw ();
};

class ExpressionData : public Type
{
	public:
	ExpressionData (const Type &t = Type(0, Type::SCALAR, UNDEFINED)) throw () : Type(t) {}
	virtual ~ExpressionData () throw ();

	virtual ExpressionData *Clone() const = 0;
	virtual string Print () const throw () = 0;
	virtual void Write (vector<Byte> &Output) const throw () = 0;
	virtual void SetSize (unsigned int s, Number::NumberType nt = Number::ANY) const throw (InvalidSize, CastFailed, CastConflict) = 0;
	virtual void SetDistanceType (int dist) const throw (InvalidSizeCast, CastConflict) = 0;

	virtual ExpressionData &operator=  (const ExpressionData &e) throw () {Type::operator= (e); return *this;}
	virtual ExpressionData &operator+= (const ExpressionData &e) {Type::operator+= (e); return *this;}
	virtual ExpressionData &operator-= (const ExpressionData &e) {Type::operator-= (e); return *this;}
	virtual ExpressionData &operator*= (const ExpressionData &e) {Type::operator*= (e); return *this;}
	virtual ExpressionData &operator/= (const ExpressionData &e) {Type::operator/= (e); return *this;}
	virtual ExpressionData &operator%= (const ExpressionData &e) {Type::operator%= (e); return *this;}

	virtual ExpressionData &operator- () {Type::operator- (); return *this;}
	virtual ExpressionData &operator~ () {Type::operator~ (); return *this;}

	virtual ExpressionData &operator&= (const ExpressionData &e) {Type::operator&= (e); return *this;}
	virtual ExpressionData &operator|= (const ExpressionData &e) {Type::operator|= (e); return *this;}
	virtual ExpressionData &operator^= (const ExpressionData &e) {Type::operator^= (e); return *this;}
	virtual ExpressionData &operator<<= (const ExpressionData &e) {Type::operator<<= (e); return *this;}
	virtual ExpressionData &operator>>= (const ExpressionData &e) {Type::operator>>= (e); return *this;}
	virtual bool operator== (const Expression &e) const throw () {return Type::operator== (e.GetConstData());}
	virtual bool operator== (const ExpressionData &e) const throw () {return Type::operator== (e);}

	// Methods for implementing NASM >>, // and %% operators
	virtual ExpressionData &BinaryShiftRight (const ExpressionData &e) {Type::operator>>= (e); return *this;}
	virtual ExpressionData &UnsignedDivision (const ExpressionData &e) {Type::operator/= (e); return *this;}
	virtual ExpressionData &UnsignedModulus (const ExpressionData &e) {Type::operator%= (e); return *this;}

	virtual ExpressionData &MemberSelect (const ExpressionData &e) {Type::operator= (e); return *this;}
	virtual ExpressionData &Compose (const ExpressionData &e) {Type::operator= (e); return *this;}
	const SimpleExpression *GetSimpleExpression() const throw ();
	const DupExpression *GetDupExpression() const throw ();
};

class SimpleExpression : public ExpressionData
{
	BasicExpression<Number, Symbol> Value;	// Expression value
	SimpleExpression *SegmentPrefix;			// Segment prefix (0 if none)

	public:
	SimpleExpression () throw () {SegmentPrefix = 0;}
	SimpleExpression (Number *n, Symbol *s, const Type &tt = Type ()) throw ();
	SimpleExpression (const SimpleExpression &e);
	~SimpleExpression () throw () {delete SegmentPrefix;}

	SimpleExpression *Clone() const {return new SimpleExpression(*this);}
	void SetSize (unsigned int s, Number::NumberType nt = Number::ANY) const throw (InvalidSize, CastFailed, CastConflict);
	void SetDistanceType (int dist) const throw (InvalidSizeCast, CastConflict);
	void Write (vector<Byte> &Output) const;

	const SimpleExpression *GetSegmentPrefix () const throw () {return SegmentPrefix;}
	const Symbol *GetSymbol () const throw ();
	const Symbol *GetSymbol (const SimpleExpression * &Prefix) const throw ();
	const Number *GetNumber () const throw ();
	const Number *GetNumber (const SimpleExpression * &Prefix) const throw ();

	typedef BasicExpression<Number, Symbol>::const_iterator const_iterator;
	const_iterator begin() const throw () {return Value.begin();}
	const_iterator end() const throw () {return Value.end();}

	SimpleExpression &operator=  (const SimpleExpression &e);
	SimpleExpression &operator+= (const ExpressionData &e);
	SimpleExpression &operator-= (const ExpressionData &e);
	SimpleExpression &operator*= (const ExpressionData &e);
	SimpleExpression &operator/= (const ExpressionData &e);
	SimpleExpression &operator%= (const ExpressionData &e);

	SimpleExpression &operator- ();
	SimpleExpression &operator~ ();

	SimpleExpression &operator&= (const ExpressionData &e);
	SimpleExpression &operator|= (const ExpressionData &e);
	SimpleExpression &operator^= (const ExpressionData &e);
	SimpleExpression &operator<<= (const ExpressionData &e);
	SimpleExpression &operator>>= (const ExpressionData &e);
	bool operator== (const ExpressionData &e) const throw ();
	bool operator== (const Expression &e) const throw ();

	// Methods for implementing NASM >>, // and %% operators
	SimpleExpression &BinaryShiftRight (const ExpressionData &e);
	SimpleExpression &UnsignedDivision (const ExpressionData &e);
	SimpleExpression &UnsignedModulus (const ExpressionData &e);

	SimpleExpression &MemberSelect (const ExpressionData &e);
	SimpleExpression &Compose (const ExpressionData &e);
	static void AddToList (Expression &e1, const Expression &e2);
	static void DUP (Expression &e1, const Expression &e2);

	string Print () const throw ()
	{
		string s;

		if (SegmentPrefix != 0)
			s += SegmentPrefix->Print() + ":";

		s += Value.Print();
		return s;
	}
};

class DupExpression : public ExpressionData
{
	SimpleExpression *Count;
	vector<ExpressionData *> DupList;

	public:
	DupExpression (const Type &t = Type(0, Type::SCALAR, UNDEFINED)) throw ();
	DupExpression (const DupExpression &exp) throw ();
	~DupExpression () throw ();

	DupExpression *Clone() const {return new DupExpression(*this);}
	string Print () const throw ();
	void Write (vector<Byte> &Output) const throw ();
	void SetSize (unsigned int s, Number::NumberType nt = Number::ANY) const throw (InvalidSize, CastFailed, CastConflict);
	void SetDistanceType (int dist) const throw (InvalidSizeCast, CastConflict);
	void SetNumericalType (int num) throw ();

	DupExpression &operator= (const ExpressionData &e);
	DupExpression &operator+= (const ExpressionData &e);
	DupExpression &operator-= (const ExpressionData &e);
	DupExpression &operator*= (const ExpressionData &e);
	DupExpression &operator/= (const ExpressionData &e);
	DupExpression &operator%= (const ExpressionData &e);

	DupExpression &operator- ();
	DupExpression &operator~ ();

	DupExpression &operator&= (const ExpressionData &e);
	DupExpression &operator|= (const ExpressionData &e);
	DupExpression &operator^= (const ExpressionData &e);
	DupExpression &operator<<= (const ExpressionData &e);
	DupExpression &operator>>= (const ExpressionData &e);
	bool operator== (const DupExpression &e) const throw ();
	bool operator== (const Expression &e) const throw ();

	// Methods for implementing NASM >>, // and %% operators
	DupExpression &BinaryShiftRight (const ExpressionData &e);
	DupExpression &UnsignedDivision (const ExpressionData &e);
	DupExpression &UnsignedModulus (const ExpressionData &e);

	DupExpression &MemberSelect (const ExpressionData &e);
	DupExpression &Compose (const ExpressionData &e);

	static void AddToList (Expression &e1, const Expression &e2);
};

class UnexpectedSegmentPrefix : public ExpressionException
{
	public:
	UnexpectedSegmentPrefix (const ExpressionData &e) : ExpressionException ("Unexpected segment prefix: ") {WhatString += e.Print();}
	~UnexpectedSegmentPrefix () {}
};

class SymbolExpected : public ExpressionException
{
	public:
	SymbolExpected (const ExpressionData &e) : ExpressionException ("Symbol expected, got ") {WhatString += e.Print();}
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
	InvalidArgument (const ExpressionData &e) : ExpressionException ("Invalid argument: ") {WhatString += e.Print();}
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
