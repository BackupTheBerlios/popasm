/***************************************************************************
                          lexical.h  -  description
                             -------------------
    copyright            : (C) 2001 by Helcio Mello
    email                : helcio@users.sourceforge.net
 ***************************************************************************/

//--------------------------------------------------------------------------
// Lexical analyser. Returns tokens from the input file.
//--------------------------------------------------------------------------

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LEXICAL_H
#define LEXICAL_H

#include <string>
#include <exception>

#include "inp_file.h"
#include "number.h"
#include "register.h"
#include "operator.h"
#include "express.h"
#include "type.h"

// Tokens base class
class Token
{
	public:
	Token () throw () {}
	virtual ~Token () throw () = 0;

	virtual void Print () const = 0;
};

// Reads a token from the given file
Token *GetToken (InputFile &inp) throw ();

class IntegerExpected : public exception
{
	static const char WhatString[];

	public:
	IntegerExpected () throw () {}
	~IntegerExpected () throw () {}

	const char *what() const {return WhatString;}
};

// Numbers represent quantities (including negative and real ones)
class Number : public Token
{
	RealNumber n;
	Type t;

	// Verifies if a number makes sense to its meaning (eg.: floating point numbers cannot be memory addresses)
	void CheckSemantics () const;

	public:
	Number () throw () : t(Type::SCALAR) {}
	Number (const RealNumber &x) throw () : n(x), t(Type::SCALAR) {}
	Number (const Number &x) throw () : n(x.n), t(x.t) {}
	~Number () throw () {}

	Number &operator+= (const Number &x) {n += x.n; t += x.t; CheckSemantics(); return *this;}
	Number &operator-= (const Number &x) {n -= x.n; t -= x.t; CheckSemantics(); return *this;}
	Number &operator*= (const Number &x) {n *= x.n; t *= x.t; CheckSemantics(); return *this;}
	Number &operator/= (const Number &x);
	Number &operator%= (const Number &x);

	Number &operator&= (const Number &x);
	Number &operator|= (const Number &x);
	Number &operator^= (const Number &x);
	Number &operator<<= (const Number &x);
	Number &operator>>= (const Number &x);

	Number &operator~ ();
	Number &operator- () {n.ChangeSign(); t = -t; CheckSemantics(); return *this;}

	bool operator== (const Number &x) const throw () {return (n == x.n) && (t == x.t);}
	bool operator!= (const Number &x) const throw () {return (n != x.n) && (t == x.t);}

	// Attempts to build a Number from the given string. Gets more from inp if necessary. Returns 0 if failed.
	static Number *Read (const string &str, InputFile &inp);

	void Print () const throw () {cout << "Number: " << n.Print();}
	static Word GetDefautBase() throw () {return NaturalNumber::GetDefaultBase();}

	virtual Number *Clone() const throw () {return new Number (*this);}
	virtual bool Zero () const {return n.Zero();}
};

// Encapsulates a pointer to a table of iX86 registers
class Register : public Token
{
	const RegisterData *Data;

	public:
	Register (const RegisterData *d) throw () : Data(d) {}
	virtual ~Register () throw () = 0;

	bool operator== (const Register &r) const throw () {return Data == r.Data;}

	// Attempts to read a register from the given string. Gets more from inp if necessary. Returns 0 if failed.
	static Register *Read (const string &str, InputFile &inp) throw ();

	void Print () const throw () {cout << "Register: " << Data->GetName();}
	virtual Register *Clone() const throw () = 0;
};

// Segment Registers
class SegmentRegister : public Register
{
	static RegisterData RegisterTable[];

	public:
	SegmentRegister (RegisterData *d) throw () : Register (d) {}
	~SegmentRegister () throw () {}

	static SegmentRegister *Read (const string &s, InputFile &inp) throw ();
	Register *Clone() const throw () {return new SegmentRegister (*this);}
};

// General-Purpose Registers
class GPRegister : public Register
{
	static RegisterData RegisterTable[];

	public:
	GPRegister (RegisterData *d) throw () : Register (d) {}
	~GPRegister () throw () {}

	static GPRegister *Read (const string &s, InputFile &inp) throw ();
	Register *Clone() const throw () {return new GPRegister (*this);}
};

// Control Registers
class ControlRegister : public Register
{
	static RegisterData RegisterTable[];

	public:
	ControlRegister (RegisterData *d) throw () : Register (d) {}
	~ControlRegister () throw () {}

	static ControlRegister *Read (const string &s, InputFile &inp) throw ();
	Register *Clone() const throw () {return new ControlRegister (*this);}
};

// Test Registers
class TestRegister : public Register
{
	static RegisterData RegisterTable[];

	public:
	TestRegister (RegisterData *d) throw () : Register (d) {}
	~TestRegister () throw () {}

	static TestRegister *Read (const string &s, InputFile &inp) throw ();
	Register *Clone() const throw () {return new TestRegister (*this);}
};

// Debug Registers
class DebugRegister : public Register
{
	static RegisterData RegisterTable[];

	public:
	DebugRegister (RegisterData *d) throw () : Register (d) {}
	~DebugRegister () throw () {}

	static DebugRegister *Read (const string &s, InputFile &inp) throw ();
	Register *Clone() const throw () {return new DebugRegister (*this);}
};

// MMX Registers
class MMXRegister : public Register
{
	static RegisterData RegisterTable[];

	public:
	MMXRegister (RegisterData *d) throw () : Register (d) {}
	~MMXRegister () throw () {}

	static MMXRegister *Read (const string &s, InputFile &inp) throw ();
	Register *Clone() const throw () {return new MMXRegister (*this);}
};

// XMM Registers
class XMMRegister : public Register
{
	static RegisterData RegisterTable[];

	public:
	XMMRegister (RegisterData *d) throw () : Register (d) {}
	~XMMRegister () throw () {}

	static XMMRegister *Read (const string &s, InputFile &inp) throw ();
	Register *Clone() const throw () {return new XMMRegister (*this);}
};

class FPURegister : public Register
{
	static RegisterData RegisterTable[];

	public:
	FPURegister (RegisterData *d) throw () : Register (d) {}
	~FPURegister () throw () {}

	static FPURegister *Read (const string &s, InputFile &inp) throw ();
	Register *Clone() const throw () {return new FPURegister (*this);}
};

typedef BasicExpression <Number, Register> Expression;

class Operator : public Token
{
	// Table of all valid operators
	static const OperatorData<Expression> **OperatorTable;

	protected:
	const OperatorData<Expression> *Data;

	public:
	Operator (const OperatorData<Expression> *d) throw () : Data(d) {}
	virtual ~Operator () throw () {}

	// Attempts to read an operator from the given string. Gets more from inp if necessary. Returns 0 if failed.
	static Operator *Read (const string &str, InputFile &inp) throw ();
	static void SetupOperatorTable (const OperatorData<Expression> **ot) throw () {OperatorTable = ot;}

	// Unary prefix
	virtual Expression &operator() (Expression &x) const {return (*Data)(x);}
	// Binary
	virtual Expression &operator() (Expression &x, Expression &y) const {return (*Data)(x, y);}

	// Returns the precedence for the given usage
	enum Usage {PREFIX = 0, BINARY = 1};
	unsigned int Precedence (Usage u) const throw (InvalidUsage) {return Data->Precedence ((OperatorData<Expression>::Usage) u);}

	const string &GetName () const throw () {return Data->GetName();}
	void Print () const throw () {cout << "Operator: " << Data->GetName() << endl;}
};

class Encloser : public Operator
{
	// Table of all valid enclosers
	static const EncloserData<Expression> **EncloserTable;
	typedef EncloserData<Expression> EncData;

	public:
	Encloser (const EncloserData<Expression> *ed) throw () : Operator(ed) {}
	~Encloser () throw () {}

	static void SetupEncloserTable (const EncloserData<Expression> **et) throw () {EncloserTable = et;}
	static Encloser *Read (const string &str, InputFile &inp) throw ();

	// Checks if (*this) and op form an open-close enclosement
	bool Matches (const Encloser &op) const throw () {return dynamic_cast<const EncData *>(Data)->Matches (*op.Data);}

	// Checks if the operator is an opening encloser
	bool Opens () const throw () {return dynamic_cast<const EncData *>(Data)->Opens();}
};

// Neither of above
class UnknownToken : public Token
{
	string Name;

	public:
	UnknownToken (const string &s) throw () : Name (s) {}
	~UnknownToken () throw () {}

	void Print () const throw () {cout << "Unknown: " << Name << endl;}
};

#endif
