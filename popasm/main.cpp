/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Fri Sep 7 2001
    copyright            : (C) 2001 by Helcio Mello
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

#include <iostream>
#include "number.h"

int main ()
{
	try
	{
		NaturalNumber a(2);
		NaturalNumber b("10_000");

		cout << a.Power(b).Print () << endl;
	}
	catch (NumberException ne)
	{
		cout << ne.what() << endl;
	}

	return 0;
}
