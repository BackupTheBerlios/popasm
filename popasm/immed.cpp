/***************************************************************************
                          immed.cpp  -  description
                             -------------------
    begin                : Fri Jul 5 2002
    copyright            : (C) 2002 by Helcio Mello
    email                : helcio@users.sf.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "immed.h"

OutOfRange::OutOfRange (unsigned long int a, unsigned long int b) throw ()
	: WhatString ("Value out of range. Must be between ")
{
	WhatString += Print(a) + " and " + Print (b);
}

const char UnknownImmediateSize::WhatString[] = "Undefined size for immediate argument.";

void Immediate::SetSize (unsigned int sz, Number::NumberType t = Number::ANY) const throw (InvalidSize, CastFailed)
{
	Number temp(Value);
	temp.SetSize(sz, t);
	BasicArgument::SetSize(sz);
}

Argument *Immediate::MakeArgument (const Expression &e) throw (InvalidArgument, exception)
{
	// Only scalars can be immediates
	if (e.GetConstData().GetCurrentType() != Type::SCALAR)
		return 0;

	const SimpleExpression *pref;
	const Number *num = e.GetNumber(pref);

	// Immediate values cannot have prefixes
	if (pref != 0)
		return 0;

	return new Argument (new Immediate (*num, e.GetConstData().GetDistanceType()));
}

bool Immediate::IdFunctor::operator() (Argument &arg)
{
	const Immediate *immed = dynamic_cast<const Immediate *> (arg.GetData());

	if (immed != 0)
		return true;

	if (arg.IsUndefined())
	{
		arg.SetData (new Immediate ());
		return true;
	}

	return false;
}
