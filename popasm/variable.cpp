/***************************************************************************
                          variable.cpp  -  description
                             -------------------
    begin                : Fri Feb 7 2003
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

#include "variable.h"

UserDefined::UserDefined (const string &n) throw () : BasicSymbol(n)
{
	Line = CurrentAssembler->GetCurrentLine();
	Offset = CurrentAssembler->GetCurrentOffset();
}

UserDefined::UserDefined (const string &n, Dword off) throw () : BasicSymbol(n)
{
	Line = CurrentAssembler->GetCurrentLine();
	Offset = off;
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
