/***************************************************************************
                          hashtab.h  -  description
                             -------------------
    begin                : Sun Jun 2 2002
    copyright            : (C) 2002 by Helcio Mello
    email                : helcio@users.sourceforge.net
 ***************************************************************************/

//--------------------------------------------------------------------------
// Template that implements a hash table
//--------------------------------------------------------------------------

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef HASHTAB_H
#define HASHTAB_H

#include <set>
#include <utility>
#include <functional>
#include <exception>


// Thrown when one attempts to insert an element which is already in the Hash Table
class DuplicatedElement : public exception
{
	static const char WhatString[];

	public:
	DuplicatedElement () {}
	~DuplicatedElement () {}

	const char *what() const {return WhatString;}
};

// Template for Hash Table. T is the type of the element being stored; H is a unary functor that calculates
// a hash value for a given key, Comp is a binary functor that compares two elements and returns true if
// the first is smaller than the second; n is the number of chains (i.e. lists) used to store elements
// that hash to the same value.
template <class T, class H, class Comp = less<T>, unsigned int n = 47>
class HashTable
{
	typedef set<T, Comp> ContainerType;
	ContainerType Chains[n];

	public:
	HashTable () throw () {}
	virtual ~HashTable () throw () {};

	// Inserts elements in the Hash Table
	void Insert (const T &x) throw (DuplicatedElement);
	// Removes elements from the Hash Table
	void Remove (const T &x) throw ();
	// Checks if x is present in the table. Returns a pointer to it or null if not found.
	const T *Find (const T &x) const throw ();
};

template <class T, class H, class Comp = less<T>, unsigned int n = 47>
void HashTable<T, H, Comp, n>::Insert (const T &x) throw (DuplicatedElement)
{
	unsigned int i = H() (x) % n;

	pair<ContainerType::iterator, bool> p = Chains[i].insert(x);
	if (!p.second) throw DuplicatedElement();
}

template <class T, class H, class Comp = less<T>, unsigned int n = 47>
void HashTable<T, H, Comp, n>::Remove (const T &x) throw ()
{
	unsigned int i = H() (x) % n;
	Chains[i].erase(x);
}

template <class T, class H, class Comp = less<T>, unsigned int n = 47>
const T *HashTable<T, H, Comp, n>::Find (const T &x) const throw ()
{
	unsigned int i = H() (x) % n;
	ContainerType::iterator j = Chains[i].find (x);

	return (j == Chains[i].end()) ? 0 : &*j;
}

#endif
