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
#include <typeinfo>

#include "defs.h"
#include "number.h"
#include "lexnum.h"
#include "type_exp.h"

class BasicArgument
{
	unsigned int Size;	// Size of the argument, in bits. Zero means: no size constraint

	public:
	BasicArgument (unsigned int sz = 0) throw () : Size(sz) {}
	virtual ~BasicArgument () throw () {}

	unsigned int GetSize () const {return Size;}
	virtual string Print () const throw () = 0;
};

class Immediate : public BasicArgument
{
	public:
	Immediate (unsigned int sz = 0) throw () : BasicArgument (sz) {}
	~Immediate () throw () {}

	virtual void Write (vector<Byte> &Output) const = 0;
	virtual string Print () const throw () = 0;
};

class UnknownImmediate : public Immediate
{
	RealNumber Value;

	public:
	UnknownImmediate (const Number &n) throw () : Immediate(n.GetSize()), Value(n.GetValue()) {}
	~UnknownImmediate () throw () {}

	const RealNumber &GetValue() const throw () {return Value;}

	void Write (vector<Byte> &Output) const {throw 0;}
	string Print() const throw() {return Value.Print();}
};

class UnsignedByte : public Immediate
{
	Byte Value;

	public:
	UnsignedByte (const RealNumber &n);
	~UnsignedByte () throw () {}

	void Write (vector<Byte> &Output) const throw () {Output.push_back (Value);}
	string Print() const throw() {return ::Print(Value);}
};

class SignedByte : public Immediate
{
	Byte Value;

	public:
	SignedByte (const RealNumber &n);
	~SignedByte () throw () {}

	void Write (vector<Byte> &Output) const throw () {Output.push_back (Value);}
	string Print() const throw() {return ::Print(Value);}
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

	bool Match (const type_info &ti);
	string Print () const throw () {return Data->Print();}
};

#endif
