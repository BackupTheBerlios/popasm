//--------------------------------------------------------------------------
//                          defs.h  -  description
//                             -------------------
//    begin                : Mon Apr 23 2001
//    copyright            : (C) 2001 by Helcio Mello
//    email                : helcio@users.sourceforge.net
//--------------------------------------------------------------------------

//---------------------------------------------------------------------------------
// This file contains definitions used throughout the project
//---------------------------------------------------------------------------------

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DEFS_H
#define DEFS_H

// typedefs used here just to save some typing
typedef unsigned char Byte;
typedef unsigned short int Word;
typedef unsigned long int Dword;

Dword ZeroExtend (Byte b);
Dword ZeroExtend (Word w);

Dword SignExtend (Byte b);
Dword SignExtend (Word w);

// SHR and SAR stand for SHift Right and Shift Arithmetic Right, respectivelly
Dword SHR (Dword d, int n);
Dword SAR (Dword d, int n);

#endif
