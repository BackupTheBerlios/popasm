#ifndef LEXICAL_H
#define LEXICAL_H

#include <string>

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
	Token () {}
	virtual ~Token () = 0;

	virtual void Print () const = 0;
};

// Reads a token from the given file
Token *GetToken (InputFile &inp);

// Numbers represent quantities (including negative and real ones)
class Number : public Token
{
	RealNumber n;
	Type t;

	// Verifies if a number makes sense to its meaning (eg.: floating point numbers cannot be memory addresses)
	void CheckSemantics () const;

	public:
	Number (const RealNumber &nn) : n(nn), t(Type::SCALAR) {}
	~Number () {}

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

	bool operator== (const Number &x) const {return n == x.n;}
	bool operator!= (const Number &x) const {return n != x.n;}

	// Attempts to build a Number from the given string. Gets more from inp if necessary. Returns 0 if failed.
	static Number *Read (const string &str, InputFile &inp);

	void Print () const {cout << "Number: " << n.Print() << endl;}
	static Word GetDefautBase() {return NaturalNumber::GetDefaultBase();}
};

// Encapsulates a pointer to a table of iX86 registers
class Register : public Token
{
	const RegisterData *Data;

	public:
	Register (const RegisterData *d) : Data(d) {}
	virtual ~Register () = 0;

	bool operator== (const Register &r) const {return Data == r.Data;}

	// Attempts to read a register from the given string. Gets more from inp if necessary. Returns 0 if failed.
	static Register *Read (const string &str, InputFile &inp);

	void Print () const {cout << "Register: " << Data->GetName() <<  endl;}
	virtual Register *Clone() const = 0;
};

// Segment Registers
class SegmentRegister : public Register
{
	static RegisterData RegisterTable[];

	public:
	SegmentRegister (RegisterData *d) : Register (d) {}
	~SegmentRegister () {}

	static SegmentRegister *Read (const string &s, InputFile &inp);
	Register *Clone() const {return new SegmentRegister (*this);}
};

// General-Purpose Registers
class GPRegister : public Register
{
	static RegisterData RegisterTable[];

	public:
	GPRegister (RegisterData *d) : Register (d) {}
	~GPRegister () {}

	static GPRegister *Read (const string &s, InputFile &inp);
	Register *Clone() const {return new GPRegister (*this);}
};

// Control Registers
class ControlRegister : public Register
{
	static RegisterData RegisterTable[];

	public:
	ControlRegister (RegisterData *d) : Register (d) {}
	~ControlRegister () {}

	static ControlRegister *Read (const string &s, InputFile &inp);
	Register *Clone() const {return new ControlRegister (*this);}
};

// Test Registers
class TestRegister : public Register
{
	static RegisterData RegisterTable[];

	public:
	TestRegister (RegisterData *d) : Register (d) {}
	~TestRegister () {}

	static TestRegister *Read (const string &s, InputFile &inp);
	Register *Clone() const {return new TestRegister (*this);}
};

// Debug Registers
class DebugRegister : public Register
{
	static RegisterData RegisterTable[];

	public:
	DebugRegister (RegisterData *d) : Register (d) {}
	~DebugRegister () {}

	static DebugRegister *Read (const string &s, InputFile &inp);
	Register *Clone() const {return new DebugRegister (*this);}
};

// MMX Registers
class MMXRegister : public Register
{
	static RegisterData RegisterTable[];

	public:
	MMXRegister (RegisterData *d) : Register (d) {}
	~MMXRegister () {}

	static MMXRegister *Read (const string &s, InputFile &inp);
	Register *Clone() const {return new MMXRegister (*this);}
};

// XMM Registers
class XMMRegister : public Register
{
	static RegisterData RegisterTable[];

	public:
	XMMRegister (RegisterData *d) : Register (d) {}
	~XMMRegister () {}

	static XMMRegister *Read (const string &s, InputFile &inp);
	Register *Clone() const {return new XMMRegister (*this);}
};

class FPURegister : public Register
{
	static RegisterData RegisterTable[];

	public:
	FPURegister (RegisterData *d) : Register (d) {}
	~FPURegister () {}

	static FPURegister *Read (const string &s, InputFile &inp);
	Register *Clone() const {return new FPURegister (*this);}
};

//typedef BasicExpression <Register, Number> Expression;
class Expression : public BasicExpression <Register, Number>
{
	public:
	Expression () {}
	~Expression () {}

	Expression &operator- ();
	Expression &operator~ ();

	Expression &operator+= (const Expression &e);
	Expression &operator-= (const Expression &e);
	Expression &operator*= (const Expression &e);
	Expression &operator/= (const Expression &e);
	Expression &operator%= (const Expression &e);

	Expression &operator&= (const Expression &e);
	Expression &operator|= (const Expression &e);
	Expression &operator^= (const Expression &e);
	Expression &operator<<= (const Expression &e);
	Expression &operator>>= (const Expression &e);
};

class Operator : public Token
{
	// Table of all valid operators
	static OperatorData<Expression> OperatorTable[];

	const OperatorData<Expression> *Data;

	public:
	Operator (const OperatorData<Expression> *d) : Data(d) {}
	~Operator () {}

	// Attempts to read an operator from the given string. Gets more from inp if necessary. Returns 0 if failed.
	static Operator *Read (const string &str, InputFile &inp);
	void Print () const {cout << "Operator: " << Data->GetName() << endl;}
};

// Neither of above
class UnknownToken : public Token
{
	string Name;

	public:
	UnknownToken (const string &s) : Name (s) {}
	~UnknownToken () {}

	void Print () const {cout << "Unknown: " << Name << endl;}
};

#endif
