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

#include "argument.h"
#include "parser.h"
#include "command.h"
#include "lexsym.h"

EncloserMismatch::EncloserMismatch (const Encloser *op, const Encloser *cl) throw () : Opener(op), Closer(cl)
{
	WhatString = "Encloser mismatch between ";
	WhatString += op->GetName();
	WhatString += " and ";
	WhatString += cl->GetName();
}

Expression *MakeTerm (Token *t)
{
	Number *n;
	Symbol *s;

	// Checks for numbers
	n = dynamic_cast<Number *> (t);
	if (n != 0) return new Expression (n, 0);

	// Checks for symbols
	s = dynamic_cast<Symbol *> (t);
	if (s != 0) return new Expression (0, s);

	throw UnexpectedToken (t);
}

UnexpectedToken::UnexpectedToken (const Token *tt) throw () : t(tt)
{
	WhatString = "Unexpected token: ";
	WhatString += t->Print();
}

CommandExpected::CommandExpected (const Token *tt) throw () : t(tt)
{
	WhatString = "Expected command, got ";
	WhatString += t->Print();
}

const char *UnexpectedEnd::WhatString = "Unexpected end of expression reached.";

enum StopCondition {END_REACHED, TERM_REACHED, ENCLOSER_REACHED};

// Reads all unary operators from i on, and store them in v
StopCondition ReadPrefixOperators (vector<Operator *> &UnaryOps, const vector<Token *> &v, vector<Token *>::const_iterator &i)
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
			if (!enc->Opens()) throw UnexpectedToken (*i);
			return ENCLOSER_REACHED;
		}

		// Store prefix operator
		UnaryOps.push_back (op);
		i++;
	}

	return END_REACHED;
}

Operator *GetBinaryOperator (const vector<Token *> &v, vector<Token *>::const_iterator &i, Encloser *Opener)
{
	Operator *BinaryOp;
	Encloser *enc;

	if (i == v.end())
	{
		if (Opener == 0) return 0;
		throw UnmatchedEncloser (Opener);
	}

	// Checks for a binary operator after the term just read. If not found, we have two consecutive terms
	BinaryOp = dynamic_cast<Operator *> (*i);
	if (BinaryOp == 0) return 0;

	// Checks if the operator is an encloser
	enc = dynamic_cast<Encloser *> (BinaryOp);
	if (enc != 0)
	{
		// If it is, check if enclosers match
		if (Opener->Matches (*enc)) return enc;
		else throw EncloserMismatch (Opener, enc);
	}

	return BinaryOp;
}

Expression *GetExpression (const vector<Token *> &v, vector<Token *>::const_iterator &i, unsigned int PreviousPrec, Encloser *Opener)
{
	vector <Operator *> UnaryOps;
	Operator *op, *BinaryOp;
	Encloser *enc;
	Expression *Term;
	unsigned int NextPrec;

	switch (ReadPrefixOperators(UnaryOps, v, i))
	{
		case END_REACHED:
			throw UnexpectedEnd();

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
			delete enc;
			i++;
			break;

		default:
			throw UnexpectedEnd();
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
				delete op;
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
			delete BinaryOp;
			break;
		}

		Expression *NextExpression = GetExpression (v, ++i, NextPrec, Opener);
		(*BinaryOp)(*Term, *NextExpression);
		delete BinaryOp;
		delete NextExpression;

		BinaryOp = GetBinaryOperator(v, i, Opener);
		NextPrec = (BinaryOp == 0) ? 0 : BinaryOp->Precedence (Operator::BINARY);
	}

	return Term;
}

Expression *Parser::EvaluateExpression (const vector<Token *> &v)
{
	Expression *Result;
	vector<Token *>::const_iterator i = v.begin();

	Result = GetExpression (v, i, 0, 0);
	return Result;
}

void Parser::ReadLine (vector<Token *> &Tokens, InputFile &Input) throw ()
{
	Token *t;
	Symbol *s;
	Token::Context c = Token::COMMAND_EXPECTED;

	// Reads all tokens and place them in a vector
	while (true)
	{
		// Get the next token
		t = Token::GetToken(Input, c);

		// Checks for end-of-file
		if (t == 0) break;

		s = dynamic_cast<Symbol *> (t);
		if (s != 0)
		{
			// Checks for end-of-line
			if (s->GetName() == "\n")
			{
				delete t;
				break;
			}

			// Checks for comment
			if (s->GetName() == ";")
			{
				delete t;
				Input.SkipLine ();
				break;
			}

			// If a command has already been read, switch the lexical analyser to another context,
			// to read its arguments
			if (dynamic_cast<const Command *> (s->GetData()) != 0)
				c = Token::ARGUMENT_EXPECTED;
		}

		Tokens.push_back (t);
	}
}

vector<Byte> Parser::ParseLine ()
{
	// Reads all tokens in the line to Tokens vector
	vector<Token *> Tokens;
	ReadLine (Tokens, Input);
	vector<Token *>::iterator i = Tokens.begin();

	vector<Argument *> Arguments;

	vector<Byte> Encoding;
	enum {INITIAL, LABEL, COMMAND, FINAL} State = INITIAL;

	const Symbol *sym = 0;
	const BasicSymbol *var = 0;
	const Command *cmd = 0;

	while (State != FINAL)
	{
		switch (State)
		{
			// Beginning of line
			case INITIAL:
				// Checks for end-of-line
				if (i == Tokens.end())
				{
					State = FINAL;
					break;
				}

				// The first token MUST be a symbol, otherwise we have a syntax error
				sym = dynamic_cast<Symbol *> (*i);
				if (sym == 0) throw CommandExpected (*i);

				// Checks wheter we got a label or a command
				cmd = dynamic_cast<const Command *> (sym->GetData());
				State = (cmd == 0) ? LABEL : COMMAND;
				break;

			case LABEL:
				// Checks if the user has specified more than one label
				if (var != 0) throw CommandExpected(sym);
				var = sym->GetData();

				State = INITIAL;
				break;

			case COMMAND:
				// Skips command name
				i++;

				// Gets all command arguments
				while (i != Tokens.end())
				{
					vector<Token *>::iterator j = i;

					// Seeks the comma or the rest of the line
					while (j != Tokens.end())
					{
						Operator *op = dynamic_cast <Operator *> (*j);
						if (op != 0)
						{
							// If it is a comma, delete it
							if (op->GetName() == ",")
							{
								delete *j;
								break;
							}
						}

						j++;
					}

					// Converts the tokens into an argument
					Expression *e = EvaluateExpression (vector<Token *> (i, j));
					Arguments.push_back (Argument::MakeArgument (*e));
					delete e;

					// Skips the comma
					if (j != Tokens.end()) j++;
					i = j;
				}

				// Assemble the command
				cmd->Assemble (var, Arguments, Encoding);

				// Delete what was used during parsing. var needs not be deleted because it's within sym
				delete sym;
				for (vector<Argument *>::iterator i = Arguments.begin(); i != Arguments.end(); i++)
					delete *i;

				State = FINAL;
				break;

			case FINAL:
				break;
		}
	}

	return Encoding;
}
