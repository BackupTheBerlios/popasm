/***************************************************************************
                          operator.h  -  description
                             -------------------
    begin                : Sun Mar 17 2002
    copyright            : (C) 2002 by Helcio Mello
    email                : helcio@users.sourceforge.net
 ***************************************************************************/

//--------------------------------------------------------------------------
// Templates to model operators' behavior
//--------------------------------------------------------------------------

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef OPERATOR_H
#define OPERATOR_H

#include <string>
#include <exception>
#include "defs.h"

class OperatorException;
class InvalidArity;
class InvalidUsage;

// Contains all data needed to describe an operator
template <class T = int>
class OperatorData
{
	// Operator name (eg. "/")
	string Name;

	// Operator precedences for unary prefix, unary postfix and binary usages, in this order.
	// The higher the number the higher the precedence. Precedence zero means the operator cannot
	// be used in the corresponding way. Eg.: if Precedence[PREFIX] == 0, then the operator cannot
	// be used as a unary prefix one.
	unsigned int Precedences[2];

	public:
	OperatorData (const string &n, unsigned int p0, unsigned int p1) throw ()
		: Name(n) {Precedences[0] = p0; Precedences[1] = p1;}
	virtual ~OperatorData () throw () {}

	// Unary prefix
	virtual T &operator() (T &x) const throw (InvalidArity) {throw InvalidArity(Name, "unary prefix");}
	// Binary
	virtual T &operator() (T &x, T &y) const throw (InvalidArity) {throw InvalidArity(Name, "binary");}

	// Returns operator name
	const string &GetName () const throw () {return Name;};

	// Returns the precedence for the given usage
	enum Usage {PREFIX = 0, BINARY = 1};
	unsigned int Precedence (Usage u) const throw (InvalidUsage)
	{
		if (u > 1) throw InvalidUsage(u);
		return Precedences[u];
	}
};

// Extends OperatorData, including a pointer to the typeinfo object that describes the closing encloser
template <class T = int>
class EncloserData : public OperatorData<T>
{
	// Operator that closes the encloser
	string Closer;

	public:
	EncloserData (const string &n, const string &c = "") : OperatorData<T> (n, 0, 0), Closer (c) {}
	~EncloserData () {}

	// Checks if the encloser is an opening one (such as '[' or '(')
	bool Opens () const {return Closer != "";}
	// Checks if (*this) and ed form a matching encloser pair (such as () or [])
	bool Matches (const OperatorData<T> &ed) const {return Closer == ed.GetName();}
};

template <class T = int>
class PlusEqual : public OperatorData<T>
{
	public:
	PlusEqual (const string &n, unsigned int p0, unsigned int p1) throw () : OperatorData (n, p0, p1) {}
	~PlusEqual () throw () {}

	T &operator() (T &x) const {return x;}
	T &operator() (T &x, T &y) const {return x += y;}
};

template <class T = int>
class MinusEqual : public OperatorData<T>
{
	public:
	MinusEqual (const string &n, unsigned int p0, unsigned int p1) throw () : OperatorData (n, p0, p1) {}
	~MinusEqual () throw () {}

	T &operator() (T &x) const {x = -x; return x;}
	T &operator() (T &x, T &y) const {return x -= y;}
};

template <class T = int>
class TimesEqual : public OperatorData<T>
{
	public:
	TimesEqual (const string &n, unsigned int p0, unsigned int p1) throw () : OperatorData (n, p0, p1) {}
	~TimesEqual () throw () {}

	T &operator() (T &x, T &y) const {return x *= y;}
};

template <class T = int>
class DividesEqual : public OperatorData<T>
{
	public:
	DividesEqual (const string &n, unsigned int p0, unsigned int p1) throw () : OperatorData (n, p0, p1) {}
	~DividesEqual () throw () {}

	T &operator() (T &x, T &y) const {return x /= y;}
};

template <class T = int>
class ModEqual : public OperatorData<T>
{
	public:
	ModEqual (const string &n, unsigned int p0, unsigned int p1) throw () : OperatorData (n, p0, p1) {}
	~ModEqual () throw () {}

	T &operator() (T &x, T &y) const {return x %= y;}
};

template <class T = int>
class Period : public OperatorData<T>
{
	public:
	Period (const string &n, unsigned int p0, unsigned int p1) throw () : OperatorData (n, p0, p1) {}
	~Period () throw () {}

	T &operator() (T &x, T &y) const {return x.MemberSelect(y);}
};

template <class T = int>
class OpenBracket : public EncloserData<T>
{
	public:
	OpenBracket (const string &n, const string &c) throw () : EncloserData (n, c) {}
	~OpenBracket () throw () {}

	// Unary prefix
	T &operator() (T &x) const throw () {x[0]; return x;}
};

template <class T = int>
class CloseBracket : public EncloserData<T>
{
	public:
	CloseBracket (const string &n) throw () : EncloserData (n) {}
	~CloseBracket () throw () {}
};

template <class T = int>
class OpenParenthesis : public EncloserData<T>
{
	public:
	OpenParenthesis (const string &n, const string &c) throw ()	: EncloserData (n, c) {}
	~OpenParenthesis () throw () {}

	// Unary prefix
	T &operator() (T &x) const throw () {return x;}
};

template <class T = int>
class CloseParenthesis : public EncloserData<T>
{
	public:
	CloseParenthesis (const string &n) throw () : EncloserData (n, "") {}
	~CloseParenthesis () throw () {}
};

template <class T = int>
class And : public OperatorData<T>
{
	public:
	And (const string &n, unsigned int p0, unsigned int p1) throw () : OperatorData (n, p0, p1) {}
	~And () throw () {}

	T &operator() (T &x, T &y) const {return x &= y;}
};

template <class T = int>
class Or : public OperatorData<T>
{
	public:
	Or (const string &n, unsigned int p0, unsigned int p1) throw () : OperatorData (n, p0, p1) {}
	~Or () throw () {}

	T &operator() (T &x, T &y) const {return x |= y;}
};

template <class T = int>
class Xor : public OperatorData<T>
{
	public:
	Xor (const string &n, unsigned int p0, unsigned int p1) throw () : OperatorData (n, p0, p1) {}
	~Xor () throw () {}

	T &operator() (T &x, T &y) const {return x ^= y;}
};

template <class T = int>
class Not : public OperatorData<T>
{
	public:
	Not (const string &n, unsigned int p0, unsigned int p1) throw () : OperatorData (n, p0, p1) {}
	~Not () throw () {}

	T &operator() (T &x) const {x = ~x; return x;}
};

template <class T = int>
class ShiftLeft : public OperatorData<T>
{
	public:
	ShiftLeft (const string &n, unsigned int p0, unsigned int p1) throw () : OperatorData (n, p0, p1) {}
	~ShiftLeft () throw () {}

	T &operator() (T &x, T &y) const {return x <<= y;}
};

template <class T = int>
class ShiftRight : public OperatorData<T>
{
	public:
	ShiftRight (const string &n, unsigned int p0, unsigned int p1) throw () : OperatorData (n, p0, p1) {}
	~ShiftRight () throw () {}

	T &operator() (T &x, T &y) const {return x >>= y;}
};

template <class T = int>
class SizeCast : public OperatorData<T>
{
	unsigned int Size;

	public:
	SizeCast (const string &n, unsigned int p0, unsigned int p1, unsigned int s) throw ()
		: OperatorData (n, p0, p1), Size(s) {}
	~SizeCast () throw () {}

	T &operator() (T &x) const {x.SetSize(Size); return x;}
};

template <class T = int>
class Colon : public OperatorData<T>
{
	public:
	Colon (const string &n, unsigned int p0, unsigned int p1) throw () : OperatorData (n, p0, p1) {}
	~Colon () throw () {}

	T &operator() (T &x, T &y) const {return x.Compose(y);}
};

//------- Exceptions

// Root of Operator exceptions hierarchy
class OperatorException : public exception
{
	string WhatString;

	public:
	OperatorException (string s = "") throw () : WhatString(s) {}
	~OperatorException () throw () {}

	const char *what() const throw () {return WhatString.c_str();}
};

// Operator hsa been used with wrong number or position of its arguments
class InvalidArity : public OperatorException
{
	public:
	InvalidArity (const string &s, const string &t) throw () :
		OperatorException (string ("Operator ") + s + " has no " + t + " form") {}
	~InvalidArity () throw () {}
};

// Precedence has been asked for an usage that does not exist
class InvalidUsage : public OperatorException
{
	public:
	InvalidUsage (unsigned long int u) throw () : OperatorException (string ("Invalid usage: ") + Print(u)) {}
	~InvalidUsage () throw () {}
};

#endif
