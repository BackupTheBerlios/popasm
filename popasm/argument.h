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
#include "type_exp.h"
#include "functors.h"

class BasicArgument;
typedef UnaryFunction<const BasicArgument *, bool> BasicIdFunctor;

class BasicArgument
{
	mutable unsigned int Size;		// Size of the argument, in bits. Zero means: no size constraint

	public:
	BasicArgument (unsigned int sz = 0) throw () : Size(sz) {}
	virtual ~BasicArgument () throw () {}

	class IdFunctor : public BasicIdFunctor
	{
		public:
		bool operator() (const BasicArgument *arg) = 0;
	};

	virtual void SetSize (unsigned int sz) const {Size = sz;}
	unsigned int GetSize () const throw () {return Size;}
	virtual string Print () const throw () = 0;
};

class Argument
{
	const BasicArgument *Data;
	bool Owner;

	static Argument *MakeMemory (const Expression &e, unsigned int CurrentAddressSize);

	public:
	Argument (const BasicArgument *d, bool own) : Data(d), Owner(own) {}
	~Argument () {if (Owner) delete Data;}

	const BasicArgument *GetData () const throw () {return Data;}
	static Argument *MakeArgument (const Expression &e, unsigned int CurrentAddressSize);

	bool Match (BasicIdFunctor *arg) const {return (*arg)(Data);}

	enum CheckType {NONE, EQUAL, GREATER, HALF};
	bool TypeCheck (Argument &arg, CheckType ct);
	unsigned int GetSize () const throw () {return Data->GetSize();}
	void SetSize (unsigned int sz) const {Data->SetSize(sz);}

	string Print () const throw () {return Data->Print();}
};

#endif
