/***************************************************************************
                          functor.h  -  description
                             -------------------
    begin                : Thu Jul 4 2002
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

#ifndef FUNCTOR_H
#define FUNCTOR_H

#include <functional>

template <class T>
class PointerComparator
{
	public:
	bool operator() (const T *a, const T *b) const {return *a < *b;}
};

// Extends the STL unary_function with the operator()
template <class ArgumentType, class ResultType>
class UnaryFunction : public unary_function <ArgumentType, ResultType>
{
	public:
	virtual ResultType operator() (ArgumentType arg) = 0;
};

// BinaryCompose (f, x, y) (a) = f(x(a), y(a))
template <class F, class X, class Y>
class BinaryCompose : public UnaryFunction <typename X::argument_type, typename F::result_type>
{
	public:
	result_type operator() (argument_type arg) {return F() (X() (arg), Y() (arg));}
};

#endif
