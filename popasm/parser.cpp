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

#include <typeinfo>

#include "argument.h"
#include "parser.h"
#include "command.h"
#include "instruct.h"
#include "directiv.h"
#include "lexsym.h"
#include "asmer.h"
#include "defs.h"

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
	if (n != 0)
		return new Expression (new Number (*n), 0);

	// Checks for symbols
	s = dynamic_cast<Symbol *> (t);
	if (s != 0)
	{
		Expression *exp;
		const UserDefined *ud = dynamic_cast<const UserDefined *> (s->GetData());
		const Constant *c;

		if (ud != 0)
		{
			c = dynamic_cast<const Constant *> (ud);
			if (c != 0)
				exp = c->GetValue()->Clone();
			else
				exp = new Expression (new Number (ud->GetOffset()), 0, *ud);
		}
		else
		{
			// If we have a forward reference, make it an unknown expression
			if (typeid (*s->GetData()) == typeid (BasicSymbol))
			{
				exp = new Expression (0, s->Clone(), Type(0, Type::UNKNOWN));

				// If the symbol has not beed defined in the previous pass then we have an error
				if (s->GetData()->GetDefinitionPass() != CurrentAssembler->GetCurrentPass())
				{
					throw UndefinedSymbol (s->GetName());
				}

				CurrentAssembler->DefineSymbol (new BasicSymbol (s->GetName()));
				CurrentAssembler->RequestNewPass();
			}
			else
				exp = new Expression (0, s->Clone());
		}

		return exp;
	}

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
		// If a closing encloser is issued without its opening counterpart stop parsing here.
		if (Opener == 0)
			return 0;

		// If it is, check if enclosers match
		if (Opener->Matches (*enc))
			return enc;
		else
			throw EncloserMismatch (Opener, enc);
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

		Expression *NextExpression = GetExpression (v, ++i, NextPrec, Opener);
		(*BinaryOp)(*Term, *NextExpression);
		delete NextExpression;

		BinaryOp = GetBinaryOperator(v, i, Opener);
		NextPrec = (BinaryOp == 0) ? 0 : BinaryOp->Precedence (Operator::BINARY);
	}

	return Term;
}

Expression *Parser::EvaluateExpression (const vector<Token *> &v, vector<Token *>::iterator &i)
{
	Expression *Result;

	Result = GetExpression (v, i, 0, 0);
	return Result;
}

void Parser::ReadLine (vector<Token *> &Tokens) throw ()
{
	Token *t;
	Symbol *s;

	// Reads all tokens and place them in a vector
	while (true)
	{
		// Get the next token
		t = Token::GetToken(Input, Token::ARGUMENT_EXPECTED);

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
		}

		Tokens.push_back (t);
	}
}

void Parser::ParseArguments (vector<Argument *> &args, vector<Token *> &Tokens)
{
	Expression *e;
	Operator *op;
	vector<Token *>::iterator i = Tokens.begin();

	while (i != Tokens.end())
	{
		e = EvaluateExpression (Tokens, i);
		args.push_back (Argument::MakeArgument (*e));
		delete e;

		if (i == Tokens.end())
			break;

		op = dynamic_cast <Operator *> (*i);
		if (op != 0)
		{
			// If it is a comma the end of argument has been found
			if (op->GetName() == ",")
				i++;
			else
			{
				cout << "Extra characters on line." << endl;
				return;
			}
		}
		else
		{
			cout << "Extra characters on line." << endl;
			return;
		}
	}
}

void Parser::ParseArguments (vector<Argument *> &args)
{
	vector <Token *> Tokens;
	ReadLine (Tokens);

	ParseArguments (args, Tokens);

	for (vector<Token *>::iterator i = Tokens.begin(); i != Tokens.end(); i++)
		delete *i;
}

vector<Byte> Parser::ParseLine ()
{
	// Reads all tokens in the line to Tokens vector
	vector<Token *> Tokens;
	vector<Byte> Encoding;
	enum {INITIAL, LABEL, COMMAND, FINAL} State = INITIAL;

	const Symbol *sym = 0;
	const Command *cmd = 0;
	const Operator *op;

	const Symbol *var = 0;
	Token *NextToken = Token::GetToken (Input, Token::COMMAND_EXPECTED);

	while (State != FINAL)
	{
		switch (State)
		{
			// Beginning of line
			case INITIAL:
				// The first token MUST be a symbol, otherwise we have a syntax error
				sym = dynamic_cast<Symbol *> (NextToken);
				if (sym == 0)
					throw CommandExpected (NextToken);

				// Checks for end-of-line
				if (sym->GetName() == "\n")
				{
					delete NextToken;
					State = FINAL;
					break;
				}

				// Checks wheter we got a label or a command
				cmd = dynamic_cast<const Command *> (sym->GetData());
				State = (cmd == 0) ? LABEL : COMMAND;
				break;

			case LABEL:
				// Checks if the user has specified more than one label
				if (var != 0)
					throw CommandExpected(sym);
				var = sym;

				// Skips colon (if any)
				NextToken = Token::GetToken (Input, Token::COMMAND_EXPECTED);
				op = dynamic_cast<const Operator *> (NextToken);
				if (op != 0)
					if (op->GetName() == ":")
					{
						delete NextToken;
						NextToken = Token::GetToken (Input, Token::COMMAND_EXPECTED);
					}

				State = INITIAL;
				break;

			case COMMAND:
				// Skips command name and assemble it
				cmd->Assemble (var, *this, Encoding);
				delete NextToken;

				State = FINAL;
				break;

			case FINAL:
				break;
		}
	}

	// Test for orphan labels
	if ((cmd == 0) && (var != 0))
		CurrentAssembler->DefineSymbol (new Label (var->GetName()));

	delete var;
	return Encoding;
}
