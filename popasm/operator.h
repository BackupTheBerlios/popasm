/***************************************************************************
                          operator.h  -  description
                             -------------------
    begin                : Sun Mar 17 2002
    copyright            : (C) 2002 by Helcio Mello
    email                : helcio@users.sourceforge.net
 ***************************************************************************/

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

class OperatorException;
class InvalidArity;

// Contains all data needed to describe an operator
template <class T = int>
class OperatorData
{
	string Name;					// Operator name (eg. "/")
	unsigned int Precedence;	// Operator precedence. The higher the number the higher the precedence

	public:
	OperatorData (const string &n, unsigned int p) throw () : Name(n), Precedence(p) {}
	virtual ~OperatorData () throw () {}

	// Unary prefix
	virtual T &operator() (T &x) const throw (InvalidArity) {throw InvalidArity(Name, "unary prefix");}
	// Unary postfix
	virtual T &operator() (T &x, int) const throw (InvalidArity) {throw InvalidArity(Name, "unary postfix");}
	// Binary
	virtual T &operator() (T &x, T &y) const throw (InvalidArity) {throw InvalidArity(Name, "binary");}

	// Returns operator name
	const string &GetName () const {return Name;};
	// Checks if (*this) has higher precedence than op
	bool Precedes (const OperatorData &op) const throw () {return Precedence > op.Precedence;}
};

template <class T = int>
class PlusEqual : public OperatorData<T>
{
	public:
	PlusEqual () : OperatorData ("+", 10) {}
	~PlusEqual () {}

	T &operator() (T &x) const {return x;}
	T &operator() (T &x, T &y) const {return x += y;}
};

template <class T = int>
class MinusEqual : public OperatorData<T>
{
	public:
	MinusEqual () : OperatorData ("-", 10) {}
	~MinusEqual () {}

	T &operator() (T &x) const {x = -x; return x;}
	T &operator() (T &x, T &y) const {return x -= y;}
};

template <class T = int>
class TimesEqual : public OperatorData<T>
{
	public:
	TimesEqual () : OperatorData ("*", 20) {}
	~TimesEqual () {}

	T &operator() (T &x, T &y) const {return x *= y;}
};

template <class T = int>
class DividesEqual : public OperatorData<T>
{
	public:
	DividesEqual () : OperatorData ("/", 20) {}
	~DividesEqual () {}

	T &operator() (T &x, T &y) const {return x /= y;}
};

template <class T = int>
class ModEqual : public OperatorData<T>
{
	public:
	ModEqual () : OperatorData ("MOD", 20) {}
	~ModEqual () {}

	T &operator() (T &x, T &y) const {return x %= y;}
};

template <class T = int>
class Period : public OperatorData<T>
{
	public:
	Period () : OperatorData (".", 20) {}
	~Period () {}
};

template <class T = int>
class OpenBracket : public OperatorData<T>
{
	public:
	OpenBracket () : OperatorData ("[", 20) {}
	~OpenBracket () {}
};

template <class T = int>
class CloseBracket : public OperatorData<T>
{
	public:
	CloseBracket () : OperatorData ("]", 20) {}
	~CloseBracket () {}
};

template <class T = int>
class OpenParenthesis : public OperatorData<T>
{
	public:
	OpenParenthesis () : OperatorData ("(", 20) {}
	~OpenParenthesis () {}
};

template <class T = int>
class CloseParenthesis : public OperatorData<T>
{
	public:
	CloseParenthesis () : OperatorData (")", 20) {}
	~CloseParenthesis () {}
};

template <class T = int>
class And : public OperatorData<T>
{
	public:
	And () : OperatorData ("AND", 20) {}
	~And () {}

	T &operator() (T &x, T &y) const {return x &= y;}
};

template <class T = int>
class Or : public OperatorData<T>
{
	public:
	Or () : OperatorData ("OR", 20) {}
	~Or () {}

	T &operator() (T &x, T &y) const {return x |= y;}
};

template <class T = int>
class Xor : public OperatorData<T>
{
	public:
	Xor () : OperatorData ("XOR", 20) {}
	~Xor () {}

	T &operator() (T &x, T &y) const {return x ^= y;}
};

template <class T = int>
class Not : public OperatorData<T>
{
	public:
	Not () : OperatorData ("NOT", 20) {}
	~Not () {}

	T &operator() (T &x) const {x = ~x; return x;}
};

template <class T = int>
class ShiftLeft : public OperatorData<T>
{
	public:
	ShiftLeft () : OperatorData ("SHL", 20) {}
	~ShiftLeft () {}

	T &operator() (T &x, T &y) const {return x <<= y;}
};

template <class T = int>
class ShiftRight : public OperatorData<T>
{
	public:
	ShiftRight () : OperatorData ("SHR", 20) {}
	~ShiftRight () {}

	T &operator() (T &x, T &y) const {return x >>= y;}
};

//------- Exceptions

// Root of Operator exceptions hierarchy
class OperatorException : public exception
{
	string WhatString;

	public:
	OperatorException (string s = "") : WhatString(s) {}
	~OperatorException () {}

	const char *what() const {return WhatString.c_str();}
};

// Operator hsa been used with wrong number or position of its arguments
class InvalidArity : public OperatorException
{
	public:
	InvalidArity (const string &s, const string &t) :
		OperatorException (string ("Operator ") + s + " has no " + t + " form") {}
	~InvalidArity () {}
};

#endif
