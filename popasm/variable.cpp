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
#include "asmer.h"

UserDefined::UserDefined (const string &n, const Type &t) throw () : BasicSymbol(n), Type(t)
{
	Line = CurrentAssembler->GetCurrentLine();
	Offset = CurrentAssembler->GetCurrentOffset();
}

UserDefined::UserDefined (const string &n, Dword off, const Type &t) throw () : BasicSymbol(n), Type(t)
{
	Line = CurrentAssembler->GetCurrentLine();
	Offset = off;
}

bool UserDefined::Changed (const BasicSymbol *s) throw ()
{
	bool answer = false;

	const UserDefined *ud = dynamic_cast<const UserDefined *> (s);
	if (ud == 0)
	{
		cout << "Internal error. Please send a bug report." << endl;
		return false;
	}

	// Checks for changed in offset
	if (Offset != ud->Offset)
	{
		answer = true;
		Offset = ud->Offset;
	}

	// Checks for changes in typing info
	if (Type::operator!= (*ud))
	{
		answer = true;
		Type::operator= (*ud);
	}

	return answer;
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
