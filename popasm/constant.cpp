/***************************************************************************
                          constant.cpp  -  description
                             -------------------
    begin                : Sat Feb 1 2003
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

#include "constant.h"

void Constant::SetValue(Expression *NewValue) throw ()
{
	if (Mutable)
	{
		delete Value;
		Value = NewValue;
	}
	else
		throw MultidefinedSymbol (GetName());
}
