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
class Argument;

typedef UnaryFunction<const BasicArgument *, bool> BasicIdFunctor;

class BasicArgument : public Type
{
	public:
	BasicArgument (unsigned int sz = 0, int dist = UNDEFINED) throw () : Type (sz, SCALAR, dist) {}
	BasicArgument (const Type &t) throw () : Type (t) {}
	virtual ~BasicArgument () throw () {}

	void SetSize (unsigned int sz) const throw () {const_cast<BasicArgument *> (this)->Type::SetSize (sz);}

	virtual string Print () const throw () = 0;

	class IdFunctor : public BasicIdFunctor
	{
		public:
		virtual bool operator() (const BasicArgument *arg) = 0;
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

class OneUndefinedSize : public exception
{
	static const char WhatString[];

	public:
	OneUndefinedSize () throw () {}
	~OneUndefinedSize () throw () {}

	const char *what() const throw() {return WhatString;}
};

class TypeMismatch : public exception
{
	string WhatString;

	public:
	TypeMismatch (const vector<Argument *> v) throw ()
		: WhatString ("Type mismatch between ") {}
	~TypeMismatch () throw () {}

	const char *what() const throw () {return WhatString.c_str();}
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
	static Argument *MakeArgument (const Expression &e) throw (InvalidArgument, exception);

	bool Match (BasicIdFunctor *arg) const {return (*arg)(Data);}

	enum CheckType {NONE, EQUAL, GREATER, HALF, MINUS_16BITS};
	static void TypeCheck (const vector<Argument *> &args, CheckType ct) throw (TypeMismatch);

	unsigned int GetSize () const throw () {return Data->GetSize();}
	void SetSize (unsigned int sz) const {Data->SetSize(sz);}

	string Print () const throw () {return Data->Print();}
};

#endif
