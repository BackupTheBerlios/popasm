/***************************************************************************
                          parser.cpp  -  description
                             -------------------
    begin                : Sun May 26 2002
    copyright            : (C) 2002 by Helcio Mello
    email                : helcio@users.sourceforge.net
 ***************************************************************************/

//---------------------------------------------------------------------------
// Checks syntax and evaluates expressions respecting precedence
//---------------------------------------------------------------------------

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "parser.h"

Expression *MakeTerm (Token *t)
{
	Number *n;
	Register *r;

	// Checks for numbers
	n = dynamic_cast<Number *> (t);
	if (n != 0) return new Expression (n, 0);

	// Checks for registers
	r = dynamic_cast<Register *> (t);
	if (r != 0) return new Expression (new Number (1), r);

	throw 0;
}

enum StopCondition {END_REACHED, TERM_REACHED, ENCLOSER_REACHED};

// Reads all unary operators from i on, and store them in v
StopCondition ReadPrefixOperators (vector<Operator *> &UnaryOps, vector<Token *> &v, vector<Token *>::iterator &i)
{
	Operator *op;
	Encloser *enc;

	// Gets all prefix operators
	while (i != v.end())
	{
		// Checks if the token is an operator
		op = dynamic_cast<Operator *> (*i);

		// If not, all unary operators have been entered
		if (op == 0) return TERM_REACHED;

		// Checks if the operator is an encloser
		enc = dynamic_cast<Encloser *> (*i);
		if (enc != 0)
		{
			// If it is and is not an opening one, throw exception
			if (!enc->Opens()) throw 0;
			return ENCLOSER_REACHED;
		}

		// Store prefix operator
		UnaryOps.push_back (op);
		i++;
	}

	return END_REACHED;
}

Operator *GetBinaryOperator (vector<Token *> &v, vector<Token *>::iterator &i, Encloser *Opener)
{
	Operator *BinaryOp;
	Encloser *enc;

	if (i == v.end()) return 0;

	// Checks for a binary operator after the term just read. If not found, we have two consecutive terms
	BinaryOp = dynamic_cast<Operator *> (*i);
	if (BinaryOp == 0) throw 0;

	// Checks if the operator is an encloser
	enc = dynamic_cast<Encloser *> (BinaryOp);
	if (enc != 0)
	{
		// If it is, check if enclosers match
		if (Opener->Matches (*enc)) return enc;
		else throw 0;
	}

	return BinaryOp;
}

Expression *GetExpression (vector<Token *> &v, vector<Token *>::iterator &i, unsigned int PreviousPrec, Encloser *Opener)
{
	vector <Operator *> UnaryOps;
	Operator *op, *BinaryOp;
	Encloser *enc;
	Expression *Term;
	unsigned int NextPrec;

	switch (ReadPrefixOperators(UnaryOps, v, i))
	{
		case END_REACHED:
			throw 0;

		case TERM_REACHED:
			// Gets the term that follows the unary operators
			Term = MakeTerm (*i);
			i++;
			break;

		case ENCLOSER_REACHED:
			// Get what comes after the opening encloser
			enc = dynamic_cast<Encloser *> (*i);
			i++;
			Term = GetExpression (v, i, 0, enc);
			(*enc)(*Term);
			i++;
			break;

		default:
			throw 0;
	}

	BinaryOp = GetBinaryOperator (v, i, Opener);
	NextPrec = (BinaryOp == 0) ? 0 : BinaryOp->Precedence (Operator::BINARY);

	while ((!UnaryOps.empty()) || (BinaryOp != 0))
	{
		if (!UnaryOps.empty())
		{
			op = UnaryOps.back();

			// Who has the greatest precedence? The last unary operator or the binary one?
			if (op->Precedence (Operator::PREFIX) >= NextPrec)
			{
				(*op)(*Term);
				UnaryOps.pop_back();
				continue;
			}
		}
		else
		{
			// Performs the next binary operator only if it has greater precedence than the previous one
			if (PreviousPrec >= NextPrec) return Term;
		}

		// If the closing encloser was reached, stop; otherwise apply the binary operator and get next one
		if (dynamic_cast<Encloser *> (BinaryOp) != 0)
		{
			i++;
			break;
		}

		(*BinaryOp)(*Term, *GetExpression (v, ++i, NextPrec, Opener));
		BinaryOp = GetBinaryOperator(v, i, Opener);
		NextPrec = (BinaryOp == 0) ? 0 : BinaryOp->Precedence (Operator::BINARY);
	}

	return Term;
}

Expression *Parser::EvaluateExpression (vector<Token *> &v)
{
	Expression *Result;
	vector<Token *>::iterator i = v.begin();

	Result = GetExpression (v, i, 0, 0);
	return Result;
}
