/***************************************************************************
                          express.cpp  -  description
                             -------------------
    copyright            : (C) 2001 by Helcio Mello
    email                : helcio@users.sourceforge.net
 ***************************************************************************/

//--------------------------------------------------------------------------
// Performs arithmetics on first degree polynomials
//--------------------------------------------------------------------------

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "express.h"

const char IncompatibleTerms::WhatString[] = "Incompatible terms.";
const char InvalidTerm::WhatString[] = "Invalid term. Numeric pointer must not be null.";
const char ConstantExpected::WhatString[] = "Operation expected a constant term.";
const char DivisionException::WhatString[] = "Division by zero or non-constant expression.";
