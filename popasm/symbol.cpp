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

unsigned int HashFunctor::operator() (BasicSymbol * const &sd)
{
	unsigned int x = 0;

	for (string::const_iterator i = (sd->GetName()).begin(); i != (sd->GetName()).end(); i++)
		x += *i;

	return x;
}

const Variable *Aggregate::FindMember (const string &v) const throw ()
{
	// Searches all members for the one named s
	for (vector<const Variable *>::const_iterator i = Members.begin(); i != Members.end(); i++)
		if (v == (*i)->GetName()) return *i;

	return 0;
}

Aggregate::~Aggregate () throw ()
{
	for (vector<const Variable *>::const_iterator i = Members.begin(); i != Members.end(); i++)
		delete *i;
}

void Structure::AddMember (const Variable *v) throw ()
{
	// New members in a structure always increase its size
	Aggregate::AddMember (v);
	Size += v->GetSize() * v->GetLength();
}

void Union::AddMember (const Variable *v) throw ()
{
	unsigned int NewSize = v->GetSize() * v->GetLength();

	// The size of an union is always the size of the greatest member
	Aggregate::AddMember (v);
	if (NewSize > Size) Size = NewSize;
}
