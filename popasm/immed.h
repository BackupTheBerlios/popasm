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

class UnknownImmediateSize : public exception
{
	static const char WhatString[];

	public:
	UnknownImmediateSize () throw () {}
	~UnknownImmediateSize () throw () {}

	const char *what () const throw () {return WhatString;}
};

class Immediate : public BasicArgument
{
	RealNumber Value;

	public:
	Immediate () {}
	Immediate (const Number &n, Distance dist) throw () : BasicArgument(n.GetSize(), dist), Value(n.GetValue()) {}
	~Immediate () throw () {}

	const RealNumber &GetValue() const throw () {return Value;}
	void SetSize (unsigned int sz, Number::NumberType t = Number::ANY) const throw (InvalidSize, CastFailed);

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

template <long int n>
class ImmedEqual : public UnaryFunction<const BasicArgument *, bool>
{
	public:
	result_type operator() (argument_type arg)
	{
		const Immediate *immed = dynamic_cast<const Immediate *> (arg);
		if (immed == 0) return false;
		if ((arg->GetSize() != 0) || (!immed->GetValue().GetInteger())) return false;
		return immed->GetValue() == RealNumber (n);
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
		if (immed->GetSize() != 0)
		{
			// If the size is the same but the type is different, throw exception.
			if (immed->GetSize() != n)
				return 0;
			else
				immed->SetSize (n, t);

			return true;
		}

		try
		{
			immed->SetSize (n, t);
		}
		catch (...)
		{
			if (ThrowExceptions) throw;
			return false;
		}

		return true;
	}
};

class RelativeArgument : public Immediate::IdFunctor
{
	Type::Distance RelativeDistance;

	public:
	RelativeArgument (Type::Distance dist) throw () : RelativeDistance(dist) {}
	~RelativeArgument () throw () {}

	Type::Distance GetRelativeDistance () const throw () {return RelativeDistance;}

	result_type operator() (argument_type arg)
	{
		// Checks whether we have an integer immediate value
		const Immediate *immed = dynamic_cast<const Immediate *> (arg);
		if (immed == 0) return false;

		return (arg->GetDistanceType() == Type::NONE) || (arg->GetDistanceType() == RelativeDistance);
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
