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

void Immediate::SetSize(unsigned int sz)
{
	Number temp(Value);
	temp.SetSize(sz);
	BasicArgument::SetSize(sz);
}

bool UnsignedByte::IdFunctor::operator() (const BasicArgument *arg)
{
	const Immediate *immed = dynamic_cast<const Immediate *> (arg);
	if (immed == 0) return false;

	if ((arg->GetSize() != 0) && (arg->GetSize() != 8)) return false;

	// Unsigned bytes must be integers
	const RealNumber &n = immed->GetValue();
	if (!n.GetInteger()) return false;

	long int x = static_cast<IntegerNumber> (n).GetValue(true);
	if ((x >= 0) && (x <= 255))
	{
		arg->BasicArgument::SetSize (8);
		return true;
	}

	return false;
}

bool SignedByte::IdFunctor::operator() (const BasicArgument *arg)
{
	const Immediate *immed = dynamic_cast<const Immediate *> (arg);
	if (immed == 0) return false;

	if ((arg->GetSize() != 0) && (arg->GetSize() != 8)) return false;

	// Unsigned bytes must be integers
	const RealNumber &n = immed->GetValue();
	if (!n.GetInteger()) return false;

	long int x = static_cast<IntegerNumber> (n).GetValue(true);
	if ((x >= -128) && (x <= 127))
	{
		arg->BasicArgument::SetSize (8);
		return true;
	}

	return false;
}

bool UnsignedWord::IdFunctor::operator() (const BasicArgument *arg)
{
	const Immediate *immed = dynamic_cast<const Immediate *> (arg);
	if (immed == 0) return false;

	if ((arg->GetSize() != 0) && (arg->GetSize() != 16)) return false;

	// Unsigned words must be integers
	const RealNumber &n = immed->GetValue();
	if (!n.GetInteger()) return false;

	long int x = static_cast<IntegerNumber> (n).GetValue(true);
	if ((x >= 0) && (x <= 65535))
	{
		arg->BasicArgument::SetSize (16);
		return true;
	}

	return false;
}
