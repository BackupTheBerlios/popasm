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
#include "asmer.h"
#include "type_exp.h"

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
	Immediate (const Number &n, int dist = UNDEFINED) throw ()
		: BasicArgument(Type (n.GetSize(), SCALAR, dist, n.IsInteger() ? INTEGER : FLOAT)), Value(n.GetValue()) {}
	~Immediate () throw () {}

	const RealNumber &GetValue() const throw () {return Value;}
	void SetSize (unsigned int sz, Number::NumberType t = Number::ANY) const throw (InvalidSize, CastFailed);

	// Returns the value of the number as an unsigned long int
	unsigned long int GetUnsignedLong () const throw (Overflow, IntegerExpected) {return Value.GetUnsignedLong();}
	// Returns the value of the number as a long int
	long int GetLong () const throw (Overflow, IntegerExpected) {return Value.GetLong();}

	static Argument *MakeArgument (const Expression &e) throw (InvalidArgument, exception);

	class IdFunctor : public BasicArgument::IdFunctor
	{
		public:
		bool operator() (Argument &arg);
	};

	// Cannot write a number if its size is unknown (zero)
	void Write (vector<Byte> &Output) const throw (UnknownImmediateSize, Overflow)
	{
		if (GetSize() == 0) throw UnknownImmediateSize();
		Value.Write(Output, GetSize() / 8);
	}

	string Print() const throw() {return Type::PrintSize(GetSize()) + " " + Value.Print();}
};

template <int sz, long int n>
class ImmedEqual : public BasicArgument::IdFunctor
{
	public:
	bool operator() (Argument &arg)
	{
		const Immediate *immed = dynamic_cast<const Immediate *> (arg.GetData());
		if (immed == 0)
		{
			arg.SetData (new Immediate (Number (n, GetFirstSize(sz))));
			return true;
		}

		if (!MatchSize (sz, immed->GetSize())) return false;
		if (immed->GetNumericalType() != INTEGER) return false;
		return immed->GetLong() == n;
	}
};

template <unsigned int n, long int a, long int b, Number::NumberType t = Number::ANY>
class ImmedRange : public BasicArgument::IdFunctor
{
	public:
	bool operator() (Argument &arg)
	{
		const Immediate *immed = dynamic_cast<const Immediate *> (arg.GetData());
		if (immed == 0)
		{
			arg.SetData (new Immediate (Number (a, n)));
			return true;
		}

		if (immed->GetNumericalType() != INTEGER)
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
class Immed : public BasicArgument::IdFunctor
{
	public:
	bool operator() (Argument &arg)
	{
		// Checks whether we have an integer immediate value
		const Immediate *immed = dynamic_cast<const Immediate *> (arg.GetData());
		if (immed == 0)
		{
			if (arg.IsUndefined())
			{
				arg.SetData (new Immediate (Number (0, n)));
				return true;
			}

			return false;
		}

		if (immed->GetNumericalType() != INTEGER) throw IntegerExpected (immed->GetValue());

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

class RelativeArgument : public BasicArgument::IdFunctor
{
	int RelativeDistance;

	public:
	RelativeArgument (int dist) throw () : RelativeDistance(dist) {}
	virtual ~RelativeArgument () throw () {}

	int GetRelativeDistance () const throw () {return RelativeDistance;}

	bool operator() (Argument &arg)
	{
		// Checks whether we have an integer immediate value
		const Immediate *immed = dynamic_cast<const Immediate *> (arg.GetData());
		if (immed == 0)
		{
			if (arg.IsUndefined())
			{
				arg.SetData (new Immediate (Number(), RelativeDistance));
				return true;
			}

			return false;
		}

		if (immed->GetDistanceType() != RelativeDistance)
			if (immed->GetDistanceType() == UNDEFINED)
				return Type::CombineSD (immed->GetSize(), RelativeDistance);
			else
				return false;

		return true;
	}
};

class DefaultImmed : public BasicArgument::IdFunctor
{
	public:
	bool operator() (Argument &arg)
	{
		switch (arg.GetData()->GetSize())
		{
			case 0:
				if (CurrentAssembler->GetCurrentMode() == 16)
					return Immed<16, Number::ANY>() (arg);
				else
					return Immed<32, Number::ANY>() (arg);

			case 16:
			case 32:
			{
				const Immediate *immed = dynamic_cast<const Immediate *> (arg.GetData());
				if (immed != 0)
					return true;

				if (arg.IsUndefined())
				{
					arg.SetData (new Immediate (Number (0, arg.GetData()->GetSize())));
					return true;
				}

				return false;
			}

			default:
				return false;
		}

		return true;
	}
};

#endif
