/***************************************************************************
                          dup_arg.cpp  -  description
                             -------------------
    begin                : Sun Mar 9 2003
    copyright            : (C) 2003 by Helcio Mello
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

#include "dup_arg.h"

Argument *DupArgument::MakeArgument (const Expression &e) throw (InvalidArgument, exception)
{
	return 0;
}

void DupArgument::Write (vector<Byte> &Output) const throw (UnknownImmediateSize, Overflow)
{
	if (DupExpression.GetConstData().GetSize() == 0)
		DupExpression.GetConstData().SetSize(GetSize());

	DupExpression.Write (Output);
}

string DupArgument::Print () const throw ()
{
	// Preliminary version
	return string();
}
