/***************************************************************************
                          argument.h  -  description
                             -------------------
    begin                : Tue Jun 4 2002
    copyright            : (C) 2002 by Helcio Mello
    email                : helcio@users.sourceforge.net
 ***************************************************************************/

//--------------------------------------------------------------------------
// Holds information about arguments passed for machine instructions
//--------------------------------------------------------------------------

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ARGUMENT_H
#define ARGUMENT_H

#include <exception>
#include <vector>

#include "defs.h"
#include "lexnum.h"
#include "type.h"
#include "type_exp.h"
#include "functors.h"

class BasicArgument;
typedef UnaryFunction<const BasicArgument *, bool> BasicIdFunctor;

class BasicArgument : public Type
{
	public:
	BasicArgument (unsigned int sz = 0, Distance dist = NONE) throw () : Type (sz, SCALAR, dist) {}
	BasicArgument (const Type &t) throw () : Type (t) {}
	virtual ~BasicArgument () throw () {}

	void SetSize (unsigned int sz) const throw () {const_cast<BasicArgument *> (this)->SetSize (sz);}

	virtual string Print () const throw () = 0;

	class IdFunctor : public BasicIdFunctor
	{
		public:
		bool operator() (const BasicArgument *arg) = 0;
		virtual ~IdFunctor() throw () {}
	};
};

class UndefinedSize : public exception
{
	static const char WhatString[];

	public:
	UndefinedSize () throw () {}
	~UndefinedSize () throw () {}

	const char *what() const throw() {return WhatString;}
};

class InvalidFullPointer : public exception
{
	static const char WhatString[];

	public:
	InvalidFullPointer () throw () {}
	~InvalidFullPointer () throw () {}

	const char *what() const throw() {return WhatString;}
};

class Argument
{
	const BasicArgument *Data;
	bool Owner;

	static Argument *MakeMemory (const Expression &e);

	public:
	Argument (const BasicArgument *d, bool own) : Data(d), Owner(own) {}
	~Argument () {if (Owner) delete Data;}

	const BasicArgument *GetData () const throw () {return Data;}
	static Argument *MakeArgument (const Expression &e) throw (InvalidArgument, InvalidFullPointer);

	bool Match (BasicIdFunctor *arg) const {return (*arg)(Data);}

	enum CheckType {NONE, EQUAL, GREATER, HALF, MINUS_16BITS};
	bool TypeCheck (Argument &arg, CheckType ct) const throw (UndefinedSize);
	unsigned int GetSize () const throw () {return Data->GetSize();}
	void SetSize (unsigned int sz) const {Data->SetSize(sz);}

	string Print () const throw () {return Data->Print();}
};

#endif
