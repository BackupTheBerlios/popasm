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
	void SetSize (unsigned int sz) const;

	class IdFunctor : public BasicArgument::IdFunctor
	{
		public:
		bool operator() (const BasicArgument *arg) {return dynamic_cast<const Immediate *> (arg) != 0;}
	};

	// Cannot write a number if its size is unknown (zero)
	void Write (vector<Byte> &Output) const {if (GetSize() == 0) throw 0; Value.Write(Output, GetSize() / 8);}
	string Print() const throw() {return Value.Print();}
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

template <unsigned int n, Number::NumberType t>
class Immed : public UnaryFunction<const BasicArgument *, bool>
{
	public:
	result_type operator() (argument_type arg)
	{
		// Checks whether we have an integer immediate value
		const Immediate *immed = dynamic_cast<const Immediate *> (arg);
		if (immed == 0) return false;

		try
		{
			immed->SetSize (n);
		}
		catch (...) {return false;}

		return true;
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
