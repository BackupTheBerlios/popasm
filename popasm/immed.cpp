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

const char UnknownImmediateSize::WhatString[] = "Undefined size for immediate argument.";

void Immediate::SetSize(unsigned int sz) const
{
	Number temp(Value);
	temp.SetSize(sz);
	BasicArgument::SetSize(sz);
}
