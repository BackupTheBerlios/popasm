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
#include "number.h"
#include "lexnum.h"
#include "type_exp.h"

class BasicArgument
{
	mutable unsigned int Size;		// Size of the argument, in bits. Zero means: no size constraint

	public:
	BasicArgument (unsigned int sz = 0) throw () : Size(sz) {}
	virtual ~BasicArgument () throw () {}

	// Checks whether ptr is a pointer to an instance of this class.
	virtual bool Identify (const BasicArgument * const ptr) const throw () = 0;

	bool Match (const BasicArgument *arg) const {return arg->Identify(this);}

	virtual void SetSize (unsigned int sz) const {Size = sz;}
	unsigned int GetSize () const throw () {return Size;}
	virtual string Print () const throw () = 0;
};

class Immediate : public BasicArgument
{
	RealNumber Value;

	public:
	Immediate () {}
	Immediate (const Number &n) throw () : BasicArgument(n.GetSize()), Value(n.GetValue()) {}
	~Immediate () throw () {}

	const RealNumber &GetValue() const throw () {return Value;}
	void SetSize (unsigned int sz);

	// Checks whether ptr is a pointer to an instance of this class.
	static const Immediate * const ClassInstance;
	bool Identify (const BasicArgument * const ptr) const throw ()
		{return dynamic_cast<const Immediate * const> (ptr) != 0;}

	// Cannot write a number if its size is unknown (zero)
	void Write (vector<Byte> &Output) const {if (GetSize() == 0) throw 0; Value.Write(Output, GetSize() / 8);}
	string Print() const throw() {return Value.Print();}
};

class UnsignedByte : public Immediate
{
	public:
	UnsignedByte () {}
	~UnsignedByte () throw () {}

	// Checks whether ptr is a pointer to an instance of this class.
	static const UnsignedByte * const ClassInstance;
	bool Identify (const BasicArgument * const ptr) const;
};

class SignedByte : public Immediate
{
	public:
	SignedByte () {}
	~SignedByte () throw () {}

	// Checks whether ptr is a pointer to an instance of this class.
	static const SignedByte * const ClassInstance;
	bool Identify (const BasicArgument * const ptr) const;
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

	bool Match (const BasicArgument *arg) const {return Data->Match(arg);}

	enum CheckType {NONE, EQUAL, GREATER};
	bool TypeCheck (Argument &arg, CheckType ct);
	unsigned int GetSize () const throw () {return Data->GetSize();}
	void SetSize (unsigned int sz) const {Data->SetSize(sz);}

	string Print () const throw () {return Data->Print();}
};

#endif
