/***************************************************************************
                          immed.h  -  description
                             -------------------
    begin                : Fri Jul 5 2002
    copyright            : (C) 2002 by Helcio Mello
    email                : helcio@users.sf.net
 ***************************************************************************/

//--------------------------------------------------------------------------
// Holds data to model immediate arguments given to instructions
//--------------------------------------------------------------------------

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef IMMED_H
#define IMMED_H

#include "argument.h"
#include "number.h"

class Immediate : public BasicArgument
{
	RealNumber Value;

	public:
	Immediate () {}
	Immediate (const Number &n) throw () : BasicArgument(n.GetSize()), Value(n.GetValue()) {}
	~Immediate () throw () {}

	const RealNumber &GetValue() const throw () {return Value;}
	void SetSize (unsigned int sz);

	class IdFunctor : public BasicArgument::IdFunctor
	{
		public:
		bool operator() (const BasicArgument *arg) {return dynamic_cast<const Immediate *> (arg) != 0;}
	};

	// Cannot write a number if its size is unknown (zero)
	void Write (vector<Byte> &Output) const {if (GetSize() == 0) throw 0; Value.Write(Output, GetSize() / 8);}
	string Print() const throw() {return Value.Print();}
};

class UnsignedByte : public Immediate
{
	public:
	UnsignedByte () {}
	~UnsignedByte () throw () {}

	class IdFunctor : public BasicArgument::IdFunctor
	{
		public:
		bool operator() (const BasicArgument *arg);
	};
};

class SignedByte : public Immediate
{
	public:
	SignedByte () {}
	~SignedByte () throw () {}

	class IdFunctor : public BasicArgument::IdFunctor
	{
		public:
		bool operator() (const BasicArgument *arg);
	};
};

class UnsignedWord : public Immediate
{
	public:
	UnsignedWord () {}
	~UnsignedWord () throw () {}

	class IdFunctor : public BasicArgument::IdFunctor
	{
		public:
		bool operator() (const BasicArgument *arg);
	};
};

#endif
