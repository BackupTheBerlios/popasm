/***************************************************************************
                          argument.h  -  description
                             -------------------
    begin                : Tue Jun 4 2002
    copyright            : (C) 2002 by Helcio Mello
    email                : helcio@users.sourceforge.net
 ***************************************************************************/

//--------------------------------------------------------------------------
// Holds information about arguments passed for machine instructions
//--------------------------------------------------------------------------

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ARGUMENT_H
#define ARGUMENT_H

#include "lexnum.h"
#include "number.h"
#include "lexop.h"

namespace Argument
{
	class BasicArgument
	{
		public:
		BasicArgument () {}
		virtual ~BasicArgument () {}
	};

	class Immediate : public BasicArgument
	{
		public:
		Immediate () {}
		~Immediate () {}
	};

	class Memory : public BasicArgument
	{
		public:
		Memory () {}
		~Memory () {}
	};

	class Register : public BasicArgument
	{
		public:
		Register () {}
		~Register () {}
	};
}

#endif
