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

#include <exception>
#include "argument.h"
#include "number.h"
#include "defs.h"

class UnknownImmediateSize : public exception
{
	static const char WhatString[];

	public:
	UnknownImmediateSize () throw () {}
	~UnknownImmediateSize () throw () {}

	const char *what () const throw () {return WhatString;}
};

class UnexpectedSize : public exception
{
	string WhatString;

	public:
	UnexpectedSize (unsigned long int expected, unsigned long int got) throw () : WhatString ("Unexpected immediate size. Expected ")
	{
		WhatString += Print(expected) + " bits got " + Print(got) + " bits.";
	}
	~UnexpectedSize () throw () {}

	const char *what () const throw () {return WhatString.c_str();}
};

class OutOfRange : public exception
{
	string WhatString;

	public:
	OutOfRange (unsigned long int a, unsigned long int b) throw ();
	~OutOfRange () throw () {}

	const char *what () const throw () {return WhatString.c_str();}
};

class Immediate : public BasicArgument
{
	RealNumber Value;

	public:
	Immediate () {}
	Immediate (const Number &n, int dist) throw () : BasicArgument(n.GetSize(), dist), Value(n.GetValue()) {}
	~Immediate () throw () {}

	const RealNumber &GetValue() const throw () {return Value;}
	bool IsInteger () const throw () {return Value.IsInteger();}
	void SetSize (unsigned int sz, Number::NumberType t = Number::ANY) const throw (InvalidSize, CastFailed);

	// Returns the value of the number as an unsigned long int
	unsigned long int GetUnsignedLong () const throw (Overflow, IntegerExpected) {return Value.GetUnsignedLong();}
	// Returns the value of the number as a long int
	long int GetLong () const throw (Overflow, IntegerExpected) {return Value.GetLong();}

	class IdFunctor : public BasicArgument::IdFunctor
	{
		public:
		bool operator() (const BasicArgument *arg) {return dynamic_cast<const Immediate *> (arg) != 0;}
	};

	// Cannot write a number if its size is unknown (zero)
	void Write (vector<Byte> &Output) const throw (UnknownImmediateSize)
	{
		if (GetSize() == 0) throw UnknownImmediateSize();
		Value.Write(Output, GetSize() / 8);
	}

	string Print() const throw() {return Number::PrintSize(GetSize()) + " " + Value.Print();}
};

template <int sz, long int n>
class ImmedEqual : public UnaryFunction<const BasicArgument *, bool>
{
	public:
	result_type operator() (argument_type arg)
	{
		const Immediate *immed = dynamic_cast<const Immediate *> (arg);
		if (immed == 0) return false;
		if (!MatchSize (sz, immed->GetSize())) return false;
		if (!immed->IsInteger()) return false;
		return immed->GetLong() == n;
	}
};

template <unsigned int n, long int a, long int b, Number::NumberType t = Number::ANY>
class ImmedRange : public UnaryFunction<const BasicArgument *, bool>
{
	public:
	result_type operator() (argument_type arg)
	{
		const Immediate *immed = dynamic_cast<const Immediate *> (arg);
		if (immed == 0) return false;

		if (!immed->IsInteger())
			throw IntegerExpected (immed->GetValue());

		if ((immed->GetSize() != n) && (immed->GetSize() != 0))
			throw UnexpectedSize (n, immed->GetSize());

		immed->SetSize (n, t);

		long int x = immed->GetLong();
		if ((x < a) || (x > b)) throw OutOfRange(a, b);

		return true;
	}
};

template <unsigned int n, Number::NumberType t, bool ThrowExceptions = true>
class Immed : public UnaryFunction<const BasicArgument *, bool>
{
	public:
	result_type operator() (argument_type arg)
	{
		// Checks whether we have an integer immediate value
		const Immediate *immed = dynamic_cast<const Immediate *> (arg);
		if (immed == 0) return false;
		if (!immed->IsInteger()) throw IntegerExpected (immed->GetValue());

		// If the size is the same but the type is different, throw exception.
		if ((immed->GetSize() != n) && (immed->GetSize() != 0))
		{
			if (ThrowExceptions)
				throw UnexpectedSize (n, immed->GetSize());
			else
				return false;
		}

		// Attempts to convert argument
		try
		{
			immed->SetSize (n, t);
		}
		catch (...)
		{
			// Rethrows exception or return false depending on the last template argument
			if (ThrowExceptions || (immed->GetSize() == n)) throw;
			return false;
		}

		return true;
	}
};

class RelativeArgument : public Immediate::IdFunctor
{
	int RelativeDistance;

	public:
	RelativeArgument (int dist) throw () : RelativeDistance(dist) {}
	virtual ~RelativeArgument () throw () {}

	int GetRelativeDistance () const throw () {return RelativeDistance;}

	result_type operator() (argument_type arg)
	{
		// Checks whether we have an integer immediate value
		const Immediate *immed = dynamic_cast<const Immediate *> (arg);
		if (immed == 0) return false;

		return (arg->GetDistanceType() == UNDEFINED) || (arg->GetDistanceType() == RelativeDistance);
	}
};

class DefaultImmed : public UnaryFunction<const BasicArgument *, bool>
{
	public:
	result_type operator() (argument_type arg)
	{
		// Checks whether we have an integer immediate value
		const Immediate *immed = dynamic_cast<const Immediate *> (arg);
		if (immed == 0) return false;

		switch (immed->GetSize())
		{
			case 0:
				if (CurrentAssembler->GetCurrentMode() == 16)
					return Immed<16, Number::ANY>() (arg);
				else
					return Immed<32, Number::ANY>() (arg);

			case 16:
			case 32:
				break;

			default:
				return false;
		}

		return true;
	}
};

#endif
