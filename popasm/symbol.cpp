/***************************************************************************
                          symbol.cpp  -  description
                             -------------------
    begin                : Tue Jun 4 2002
    copyright            : (C) 2002 by Helcio Mello
    email                : helcio@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "symbol.h"

unsigned int HashFunctor::operator() (const BasicSymbol * const &sd)
{
	unsigned int x = 0;

	for (string::const_iterator i = (sd->GetName()).begin(); i != (sd->GetName()).end(); i++)
		x += *i;

	return x;
}
