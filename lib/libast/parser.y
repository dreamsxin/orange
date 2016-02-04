/*
** Copyright 2014-2016 Robert Fratto. See the LICENSE.txt file at the top-level
** directory of this distribution.
**
** Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file
** may not be copied, modified, or distributed except according to those terms.
*/

%{
	#include <libast/Module.h>
	#include <libast/ASTNode.h>
	#include <libast/Block.h>
	#include <libast/CondBlock.h>
	#include <libast/Function.h>
	#include <libast/IfStmt.h>
	#include <libast/FunctionCall.h>
	#include <libast/ExternFunction.h>
	#include <libast/Value.h>
	#include <libast/StrValue.h>
	#include <libast/Expression.h>
	#include <libast/ReturnStmt.h>
	#include <libast/BinOpCompare.h>
	#include <libast/BinOpArith.h>
	#include <libast/BinOpAssign.h>
	#include <libast/BinOpAndOr.h>
	#include <libast/Parameter.h>
	#include <libast/IDReference.h>
	#include <libast/NegativeExpr.h>
	#include <libast/VarDecl.h>
	#include <libast/IncrementExpr.h>
	#include <libast/Loop.h>
	#include <libast/LoopTerminator.h>
	#include <libast/DerefExpr.h>
	#include <libast/ReferenceExpr.h>
	#include <libast/CastExpr.h>
	#include <libast/ArrayValue.h>
	#include <libast/ArrayAccessExpr.h>
	#include <libast/TernaryExpr.h>
	#include <libast/AccessExpr.h>
	#include <libast/EnumStmt.h>
	#include <libast/SizeofExpr.h>
	#include <libast/OString.h>
	#include <libast/ClassDecl.h>
	#include <libast/ClassMethod.h>
	#include <libast/MemberVarDecl.h>
	#include <libast/MemberAccess.h>
	#include <libast/CtorCall.h>
	#include <libast/Protectable.h>
	#include <libast/SuperReference.h>
	#include <libast/SuperCall.h>

	#include <libast/types/Type.h>
	#include <libast/types/IntType.h>
	#include <libast/types/UIntType.h>
	#include <libast/types/FloatType.h>
	#include <libast/types/DoubleType.h>
	#include <libast/types/VoidType.h>
	#include <libast/types/PointerType.h>
	#include <libast/types/VarType.h>
	#include <libast/types/ArrayType.h>
	#include <libast/types/VariadicArrayType.h>
	#include <libast/types/ReferenceType.h>

	#include <libast/exceptions/code_error.h>

	#include <util/assertions.h>

	#define SET_LOCATION(node, start, end)\
		node->setLocation(CodeLocation(module->getFile(), start.first_line,\
		end.last_line, start.first_column, end.last_column));

	extern struct YYLTYPE yylloc;
	extern void yyerror(Module* mod, const char *s);

	extern int yylex(Module* module);
%}

%locations
%error-verbose
%lex-param { Module* module }
%parse-param { Module* module }

%union {
	std::vector<ASTNode*>* nodes;
	std::vector<Parameter*>* params;
	std::vector<Expression*>* args;
	std::vector<Block*>* blocks;
	std::vector<Expression*>* exprs;
	std::vector<std::tuple<OString, Expression*>>* pairs;
	std::vector<std::tuple<OString, Value*>>* vpairs;
	std::tuple<OString, ProtectionLevel>* ppair;
	ASTNode* node;
	Block* block;
	Expression* expr;
	Statement* stmt;
	Value* val;
	OString* str;
	const Orange::Type* ty;
}

%start start

%token DEF END IF ELIF ELSE IDENTIFIER OPEN_PAREN CLOSE_PAREN TYPE COMMA
%token TIMES NUMBER DIVIDE MINUS PLUS NEWLINE SEMICOLON
%token TYPE_INT TYPE_UINT TYPE_FLOAT TYPE_DOUBLE TYPE_INT8 TYPE_UINT8 TYPE_INT16
%token TYPE_UINT16 TYPE_INT32 TYPE_UINT32 TYPE_INT64 TYPE_UINT64 TYPE_CHAR TYPE_VOID TYPE_VAR
%token RETURN CLASS USING PUBLIC PROTECTED PRIVATE OPEN_BRACE CLOSE_BRACE
%token OPEN_BRACKET CLOSE_BRACKET INCREMENT DECREMENT ASSIGN PLUS_ASSIGN
%token MINUS_ASSIGN TIMES_ASSIGN DIVIDE_ASSIGN MOD_ASSIGN ARROW ARROW_LEFT
%token DOT LEQ GEQ COMP_LT COMP_GT MOD VALUE STRING EXTERN VARARG EQUALS NEQUALS WHEN
%token UNLESS LOGICAL_AND LOGICAL_OR BITWISE_AND BITWISE_OR BITWISE_XOR
%token FOR FOREVER LOOP CONTINUE BREAK DO WHILE
%token CONST_FLAG QUESTION COLON ENUM SIZEOF TYPE_ID THIS AT STATIC FROM SUPER

%type <nodes> compound_statement var_decl valued statement_no_term flagged_compound_statement
%type <nodes> opt_valued
%type <node> statement return controls flagged_statement
%type <exprs> expr_list array_def_list
%type <pairs> var_decl_list
%type <vpairs> enum_members
%type <blocks> else_if_or_end
%type <expr> expression primary comparison arithmetic call increment
%type <expr> opt_expression ternary
%type <stmt> structures function extern_function ifs inline_if unless
%type <stmt> inline_unless for_loop inline_for_loop enum_stmt class_stmt
%type <val> VALUE pos_or_neg_value
%type <str> COMP_LT COMP_GT LEQ GEQ PLUS MINUS IDENTIFIER STRING TIMES DIVIDE ASSIGN
%type <str> EQUALS NEQUALS PLUS_ASSIGN TIMES_ASSIGN MINUS_ASSIGN DIVIDE_ASSIGN
%type <str> MOD MOD_ASSIGN BITWISE_AND BITWISE_OR BITWISE_XOR LOGICAL_AND
%type <str> LOGICAL_OR LOOP CONTINUE BREAK TYPE_ID typename_or_identifier
%type <str> THIS AT PUBLIC PROTECTED PRIVATE STATIC opt_static FROM SUPER
%type <ty> type basic_type type_hint non_agg_type array_type opt_inheritance
%type <params> param_list opt_param_list
%type <args> opt_arg_list arg_list
%type <ppair> opt_protection_level protection_level

/* lowest to highest precedence */
%left IDENTIFIER
%right CONST_FLAG
%left COMMA

%right ASSIGN ARROW_LEFT PLUS_ASSIGN MINUS_ASSIGN TIMES_ASSIGN DIVIDE_ASSIGN MOD_ASSIGN

%right QUESTION COLON

%left LOGICAL_OR
%left LOGICAL_AND

%left EQUALS NEQUALS
%left COMP_LT COMP_GT LEQ GEQ

%left BITWISE_OR
%left BITWISE_XOR
%left BITWISE_AND

%left PLUS MINUS
%left TIMES DIVIDE MOD
%left OPEN_PAREN CLOSE_PAREN
%left INCREMENT DECREMENT OPEN_BRACKET DOT
%right SIZEOF

%right PUBLIC PROTECTED PRIVATE

%%

start
	: statements
	;

statements
	: statements flagged_statement
	{
		if ($2 != nullptr)
		{
			module->getBlock()->addStatement($2);
		}
	}
	| statements flagged_compound_statement
	{
		for (auto stmt : *$2)
		{
			if (stmt == nullptr) continue;
			module->getBlock()->addStatement(stmt);
		}

		delete $2;
	}
	| flagged_statement
	{
		if ($1 != nullptr)
		{
			module->getBlock()->addStatement($1);
		}
	}
	| flagged_compound_statement
	{
		for (auto stmt : *$1)
		{
			if (stmt == nullptr) continue;
			module->getBlock()->addStatement(stmt);
		}

		delete $1;
	}
	;

flagged_statement
	: opt_protection_level opt_static statement {
		$$ = $3;

		if ($1 != nullptr)
		{
			if ($$->is<Protectable*>() == false)
			{
				auto pair = *$1;
				throw code_error($$, [pair]() -> std::string
				{
					std::stringstream ss;
					ss << "Keyword " << std::get<0>(pair).str()
					   << " can't be used here";
					return ss.str();
				});
			}

			$$->as<Protectable*>()->setProtectionLevel(std::get<1>(*$1));
			delete $1;
		}

		if ($2 != nullptr)
		{
			if ($$->is<Staticable*>() == false)
			{
				auto str = *$2;
				throw code_error($$, [str]() -> std::string
				{
					std::stringstream ss;
					ss << "Keyword " << str.str()
					   << " can't be used here";
					return ss.str();
				});
			}

			$$->as<Staticable*>()->setStatic(true);
			delete $2;
		}
	}
	;

flagged_compound_statement
	: opt_protection_level opt_static compound_statement {
		$$ = $3;

		if ($1 != nullptr)
		{
			for (auto stmt : *$$)
			{
				if (stmt->is<Protectable*>() == false)
				{
					auto pair = *$1;
					throw code_error(stmt, [pair]() -> std::string
					{
						std::stringstream ss;
						ss << "Keyword " << std::get<0>(pair).str()
						   << " can't be used here";
						return ss.str();
					});
				}

				stmt->as<Protectable*>()->setProtectionLevel(std::get<1>(*$1));
			}
			delete $1;
		}

		if ($2 != nullptr)
		{
			for (auto stmt : *$$)
			{
				if (stmt->is<Staticable*>() == false)
				{
					auto str = *$2;
					throw code_error(stmt, [str]() -> std::string
					{
						std::stringstream ss;
						ss << "Keyword " << str.str()
						   << " can't be used here";
						return ss.str();
					});
				}

				stmt->as<Staticable*>()->setStatic(true);
			}

			delete $2;
		}
	}
	;

opt_statements
	: statements
	|
	;

statement_no_term
	: structures { $$ = new std::vector<ASTNode*>(); $$->push_back($1); }
	| controls { $$ = new std::vector<ASTNode*>(); $$->push_back($1); }
	| expression { $$ = new std::vector<ASTNode*>(); $$->push_back($1); }
	| var_decl { $$ = $1; }
	;

statement
	: structures term { $$ = $1; } /* structures: if, loops, functions, etc */
	| controls term { $$ = $1; } /* controls: return, break, continue */
	| expression term { $$ = $1; }
	| term { $$ = nullptr; }
	;

compound_statement
	: var_decl term
	{
		$$ = $1;
	}
	;

valued
	: var_decl
	{
		$$ = $1;
	}
	| expression
	{
		$$ = new std::vector<ASTNode *>();
		$$->push_back($1);
	}

structures
	: function { $$ = $1; }
	| extern_function { $$ = $1; }
	| class_stmt { $$ = $1; }
	| ifs { $$ = $1; }
	| unless { $$ = $1; }
	| inline_if { $$ = $1; }
	| inline_unless { $$ = $1; }
	| for_loop { $$ = $1; }
	| inline_for_loop { $$ = $1; }
	| enum_stmt { $$ = $1; }
	;

function
	: DEF typename_or_identifier OPEN_PAREN opt_param_list CLOSE_PAREN type_hint term
	{
		Function* func = nullptr;

		if (module->getBlock()->is<ClassDecl *>())
		{
			func = new ClassMethod(*$2, module->getBlock()->as<ClassDecl *>(),
								   *$4);
		}
		else
		{
			func = new Function(*$2, *$4);
		}

		func->setReturnType($6);

		module->pushBlock(func);
		$<stmt>$ = func;
	}
	opt_statements END
	{
		auto func = (Function *)$<stmt>8;

		$$ = func;
        SET_LOCATION($$, @1, @10);

		module->popBlock();

		delete $2;
		delete $4;
	}
	| DEF typename_or_identifier OPEN_PAREN opt_param_list CLOSE_PAREN type_hint COLON
	{
		Function* func = nullptr;

		if (module->getBlock()->is<ClassDecl *>())
		{
			func = new ClassMethod(*$2, module->getBlock()->as<ClassDecl *>(),
								   *$4);
		}
		else
		{
			func = new Function(*$2, *$4);
		}

		func->setReturnType($6);

		module->pushBlock(func);
		$<stmt>$ = func;
	}
	statement_no_term
	{
		auto func = (Function *)$<stmt>8;

		for (auto statement : *$9)
		{
			func->addStatement(statement);
		}

		$$ = func;
        SET_LOCATION($$, @1, @9);

		module->popBlock();

		delete $2;
		delete $4;
		delete $9;
	}
	;

type_hint
	: ARROW type { $$ = $2; }
	| { $$ = nullptr; }
	;

extern_function
	: EXTERN IDENTIFIER OPEN_PAREN opt_param_list CLOSE_PAREN ARROW type
	{
		$$ = new ExternFunction(*$2, *$4, $7);
		SET_LOCATION($$, @1, @7);

		delete $2;
		delete $4;
	}
	| EXTERN IDENTIFIER OPEN_PAREN param_list COMMA VARARG CLOSE_PAREN ARROW type
	{
		$$ = new ExternFunction(*$2, *$4, $9, true);
		SET_LOCATION($$, @1, @9);

		delete $2;
		delete $4;
	}
	;

class_stmt
	: CLASS IDENTIFIER opt_inheritance term
	{
		$<stmt>$ = new ClassDecl(*$2, (ReferenceType*)$3);
		$<stmt>$->setModule(module);
		module->pushBlock((Block *)$<stmt>$);
		delete $2;
	} opt_statements END
	{
		ClassDecl* inst = (ClassDecl *)$<stmt>5;

		$$ = inst;
		SET_LOCATION($$, @1, @7);

		module->popBlock();
	}

opt_inheritance
	: FROM TYPE_ID
	{
		$$ = new ReferenceType(*$2);
		SET_LOCATION($$->as<ASTNode *>(), @2, @2)
		delete $2;
	}
	| { $$ = nullptr; }

ifs
	: IF expression
	{
		auto block = new CondBlock($2);
		$<stmt>$ = block;
		module->pushBlock(block);
	}
	term statements else_if_or_end
	{
		auto blocks = $6;

		auto block = (CondBlock *)$<stmt>3;

		blocks->insert(blocks->begin(), block);

		auto if_stmt = new IfStmt();
		for (auto block : *blocks)
		{
			if_stmt->addBlock(block);
		}

		$$ = if_stmt;
		SET_LOCATION($$, @1, @6);

		module->popBlock();

		delete blocks;
	}
	;

else_if_or_end
	: ELIF expression
	{
		auto block = new CondBlock($2);
		module->pushBlock(block);
		$<stmt>$ = block;
	}
	term statements else_if_or_end
	{
		$$ = $6;

		auto block = (CondBlock *)$<stmt>3;

		$$->insert($$->begin(), block);

		SET_LOCATION(block, @1, @6);
		module->popBlock();
	}
	| ELSE
	{
		auto block = new Block();
		module->pushBlock(block);
		$<stmt>$ = block;
	}
	term statements END
	{
		$$ = new std::vector<Block *>();

		auto block = (Block *)$<stmt>2;

		$$->insert($$->begin(), block);

		SET_LOCATION(block, @1, @5);
		module->popBlock();
	}
	| END
	{
		$$ = new std::vector<Block *>();
	}
	;

unless
	: UNLESS expression
	{
		auto block = new CondBlock($2, true);
		module->pushBlock(block);
		$<stmt>$ = block;
	}
	term statements END
	{
		auto block = (CondBlock *)$<stmt>3;

		auto if_stmt = new IfStmt();
		if_stmt->addBlock(block);

		$$ = if_stmt;
		SET_LOCATION($$, @1, @6);
		module->popBlock();
	}

inline_if
	: controls IF expression
	{
		auto block = new CondBlock($3);
		block->addStatement($1);

		auto if_stmt = new IfStmt();
		if_stmt->addBlock(block);

		$$ = if_stmt;
		SET_LOCATION($$, @1, @3);
	}
	| expression IF expression
	{
		auto block = new CondBlock($3);
		block->addStatement($1);

		auto if_stmt = new IfStmt();
		if_stmt->addBlock(block);

		$$ = if_stmt;
		SET_LOCATION($$, @1, @3);
	}
	;

inline_unless
	: controls UNLESS expression
	{
		auto block = new CondBlock($3, true);
		block->addStatement($1);

		auto if_stmt = new IfStmt();
		if_stmt->addBlock(block);

		$$ = if_stmt;
		SET_LOCATION($$, @1, @3);
	}
	| expression UNLESS expression
	{
		auto block = new CondBlock($3, true);
		block->addStatement($1);

		auto if_stmt = new IfStmt();
		if_stmt->addBlock(block);

		$$ = if_stmt;
		SET_LOCATION($$, @1, @3);
	}
	;

for_loop
	: FOR OPEN_PAREN opt_valued SEMICOLON opt_expression SEMICOLON opt_expression
	  CLOSE_PAREN term
	{
		auto loop = new Loop(*$3, $5, $7, false);
		module->pushBlock(loop);
		$<stmt>$ = loop;
	}
	statements END
	{
		auto loop = (Loop *)$<stmt>10;

		$$ = loop;
		SET_LOCATION($$, @1, @12);
		module->popBlock();

		delete $3;
	}
	| WHILE expression term
	{
		auto loop = new Loop(std::vector<ASTNode*>(), $2, nullptr, false);
		module->pushBlock(loop);
		$<stmt>$ = loop;
	}
	statements END
	{
		auto loop = (Loop *)$<stmt>4;

		$$ = loop;
		SET_LOCATION($$, @1, @6);
		module->popBlock();
	}
	| FOREVER DO term
	{
		auto loop = new Loop(std::vector<ASTNode*>(), nullptr, nullptr, false);
		module->pushBlock(loop);
		$<stmt>$ = loop;
	}
	statements END
	{
		auto loop = (Loop *)$<stmt>4;

		$$ = loop;
		SET_LOCATION($$, @1, @6);
		module->popBlock();
	}
	| DO term
	{
		auto loop = new Loop(std::vector<ASTNode*>(), nullptr, nullptr, true);
		module->pushBlock(loop);
		$<stmt>$ = loop;
	}
	statements END WHILE expression
	{
		auto loop = (Loop *)$<stmt>3;
		loop->setCondition($7);

		$$ = loop;
		SET_LOCATION($$, @1, @7);
		module->popBlock();
	}
	;

inline_for_loop
	: controls FOR OPEN_PAREN opt_valued SEMICOLON opt_expression SEMICOLON opt_expression
	  CLOSE_PAREN
	{
		auto loop = new Loop(*$4, $6, $8, false);
		loop->addStatement($1);
		$$ = loop;
		SET_LOCATION($$, @1, @9);

		delete $4;
	}
	| expression FOR OPEN_PAREN opt_valued SEMICOLON opt_expression SEMICOLON opt_expression
	  CLOSE_PAREN
	{
		auto loop = new Loop(*$4, $6, $8, false);
		loop->addStatement($1);
		$$ = loop;
		SET_LOCATION($$, @1, @9);

		delete $4;
	}
	| controls WHILE expression
	{
		auto loop = new Loop(std::vector<ASTNode*>(), $3, nullptr, false);
		loop->addStatement($1);
		$$ = loop;
		SET_LOCATION($$, @1, @3);
	}
	| expression WHILE expression
	{
		auto loop = new Loop(std::vector<ASTNode*>(), $3, nullptr, false);
		loop->addStatement($1);
		$$ = loop;
		SET_LOCATION($$, @1, @3);
	}
	| controls FOREVER
	{
		auto loop = new Loop(std::vector<ASTNode*>(), nullptr, nullptr, false);
		loop->addStatement($1);
		$$ = loop;
		SET_LOCATION($$, @1, @2);
	}
	| expression FOREVER
	{
		auto loop = new Loop(std::vector<ASTNode*>(), nullptr, nullptr, false);
		loop->addStatement($1);
		$$ = loop;
		SET_LOCATION($$, @1, @2);
	}
	;

opt_valued
	: valued { $$ = $1; }
	| { $$ = new std::vector<ASTNode*>(); }
	;

opt_expression
	: expression { $$ = $1; }
	| { $$ = nullptr; }
	;

param_list
	: param_list COMMA type IDENTIFIER
	{
		$$ = $1;
		auto param = new Parameter($3, *$4);
		$$->push_back(param);
		SET_LOCATION(param, @1, @4);

		delete $4;
	}
	| type IDENTIFIER
	{
		$$ = new std::vector<Parameter *>();
		auto param = new Parameter($1, *$2);
		$$->push_back(param);
		SET_LOCATION(param, @1, @2);

		delete $2;
	}
	;

opt_param_list
	: param_list { $$ = $1; }
	| { $$ = new std::vector<Parameter *>(); }
	;

opt_arg_list
	: arg_list { $$ = $1; }
	| { $$ = new std::vector<Expression *>(); }
	;

arg_list
	: arg_list COMMA expression
	{
		$$ = $1;
		$$->push_back($3);
	}
	| expression
	{
		$$ = new std::vector<Expression *>();
		$$->push_back($1);
	}

controls
	: return { $$ = $1; }
	| CONTINUE { $$ = new LoopTerminator(*$1); SET_LOCATION($$, @1, @1); delete $1; }
	| BREAK { $$ = new LoopTerminator(*$1); SET_LOCATION($$, @1, @1); delete $1; }
	| LOOP { $$ = new LoopTerminator(*$1); SET_LOCATION($$, @1, @1); delete $1; }
	;

expression
	: primary { $$ = $1; }
	| comparison { $$ = $1; }
	| arithmetic { $$ = $1; }
	| call { $$ = $1; }
	| increment { $$ = $1; }
	| ternary { $$ = $1; }
	;

comparison
	: expression COMP_LT expression { $$ = new BinOpCompare($1, *$2, $3); SET_LOCATION($$, @1, @3); delete $2; }
	| expression COMP_GT expression { $$ = new BinOpCompare($1, *$2, $3); SET_LOCATION($$, @1, @3); delete $2; }
	| expression LEQ expression { $$ = new BinOpCompare($1, *$2, $3); SET_LOCATION($$, @1, @3); delete $2; }
	| expression GEQ expression { $$ = new BinOpCompare($1, *$2, $3); SET_LOCATION($$, @1, @3); delete $2; }
	| expression EQUALS expression { $$ = new BinOpCompare($1, *$2, $3); SET_LOCATION($$, @1, @3); delete $2; }
	| expression NEQUALS expression { $$ = new BinOpCompare($1, *$2, $3); SET_LOCATION($$, @1, @3); delete $2; }

    | expression LOGICAL_AND expression { $$ = new BinOpAndOr($1, *$2, $3); SET_LOCATION($$, @1, @3); delete $2; }
    | expression LOGICAL_OR expression { $$ = new BinOpAndOr($1, *$2, $3); SET_LOCATION($$, @1, @3); delete $2; }
	;

arithmetic
	: expression PLUS expression { $$ = new BinOpArith($1, *$2, $3); SET_LOCATION($$, @1, @3); delete $2; }
	| expression MINUS expression { $$ = new BinOpArith($1, *$2, $3); SET_LOCATION($$, @1, @3); delete $2; }
	| expression TIMES expression { $$ = new BinOpArith($1, *$2, $3); SET_LOCATION($$, @1, @3); delete $2; }
	| expression DIVIDE expression { $$ = new BinOpArith($1, *$2, $3); SET_LOCATION($$, @1, @3); delete $2; }
	| expression MOD expression { $$ = new BinOpArith($1, *$2, $3); SET_LOCATION($$, @1, @3); delete $2; }

	| expression BITWISE_AND expression { $$ = new BinOpArith($1, *$2, $3); SET_LOCATION($$, @1, @3); delete $2; }
	| expression BITWISE_OR expression { $$ = new BinOpArith($1, *$2, $3); SET_LOCATION($$, @1, @3); delete $2; }
	| expression BITWISE_XOR expression { $$ = new BinOpArith($1, *$2, $3); SET_LOCATION($$, @1, @3); delete $2; }

	| expression ASSIGN expression { $$ = new BinOpAssign($1, *$2, $3); SET_LOCATION($$, @1, @3); delete $2; }
	| expression PLUS_ASSIGN expression { $$ = new BinOpAssign($1, *$2, $3); SET_LOCATION($$, @1, @3); delete $2; }
	| expression MINUS_ASSIGN expression { $$ = new BinOpAssign($1, *$2, $3); SET_LOCATION($$, @1, @3); delete $2; }
	| expression TIMES_ASSIGN expression { $$ = new BinOpAssign($1, *$2, $3); SET_LOCATION($$, @1, @3); delete $2; }
	| expression DIVIDE_ASSIGN expression { $$ = new BinOpAssign($1, *$2, $3); SET_LOCATION($$, @1, @3); delete $2; }
	| expression MOD_ASSIGN expression { $$ = new BinOpAssign($1, *$2, $3); SET_LOCATION($$, @1, @3); delete $2; }
	;

increment
	: expression INCREMENT { $$ = new IncrementExpr($1,  1, false); SET_LOCATION($$, @1, @2); }
	| expression DECREMENT { $$ = new IncrementExpr($1, -1, false); SET_LOCATION($$, @1, @2); }
	| INCREMENT expression { $$ = new IncrementExpr($2,  1, true); SET_LOCATION($$, @1, @2); }
	| DECREMENT expression { $$ = new IncrementExpr($2, -1, true); SET_LOCATION($$, @1, @2); }
	;

ternary
	: expression QUESTION expression COLON expression
	{
		$$ = new TernaryExpr($1, $3, $5);
		SET_LOCATION($$, @1, @5);
	}
	;

call
	: IDENTIFIER OPEN_PAREN opt_arg_list CLOSE_PAREN
	{
		$$ = new FunctionCall(*$1, *$3);
		SET_LOCATION($$, @1, @4);

		delete $1;
		delete $3;
	}
	| TYPE_ID OPEN_PAREN opt_arg_list CLOSE_PAREN
	{
		$$ = new CtorCall(*$1, *$3);
		SET_LOCATION($$, @1, @4);

		delete $1;
		delete $3;
	}
	| expression OPEN_PAREN opt_arg_list CLOSE_PAREN
	{
		$$ = new ExpressionCall($1, *$3, true);
		SET_LOCATION($$, @1, @4);

		delete $3;
	}
	| SUPER OPEN_PAREN opt_arg_list CLOSE_PAREN
	{
		$$ = new SuperCall(*$3);
		SET_LOCATION($$, @1, @4);

		delete $3;
	}
	;

typename_or_identifier
	: IDENTIFIER { $$ = $1; }
	| TYPE_ID { $$ = $1; }
	;

primary
	: OPEN_PAREN expression CLOSE_PAREN { $$ = $2; }
	| VALUE { $$ = $1; SET_LOCATION($$, @1, @1); }
	| MINUS expression { $$ = new NegativeExpr($2); SET_LOCATION($$, @1, @2); }
	| STRING { $$ = new StrValue(*$1); SET_LOCATION($$, @1, @1); delete $1; }
	| IDENTIFIER { $$ = new IDReference(*$1); SET_LOCATION($$, @1, @1); delete $1; }
	| TIMES expression { $$ = new DerefExpr($2); SET_LOCATION($$, @1, @2); }
	| BITWISE_AND expression { $$ = new ReferenceExpr($2); SET_LOCATION($$, @1, @2); }
	| OPEN_PAREN type CLOSE_PAREN expression { $$ = new CastExpr($2, $4); SET_LOCATION($$, @1, @4); }
	| OPEN_BRACKET expr_list CLOSE_BRACKET { $$ = new ArrayValue(*$2); SET_LOCATION($$, @1, @3); delete $2; }
	| expression OPEN_BRACKET expression CLOSE_BRACKET { $$ = new ArrayAccessExpr($1, $3); SET_LOCATION($$, @1, @4); }
	| expression DOT IDENTIFIER { $$ = new AccessExpr($1, *$3); SET_LOCATION($$, @1, @3); delete $3; }
	| TYPE_ID DOT IDENTIFIER {
		auto ref = new ReferenceType(*$1);
		SET_LOCATION(ref, @1, @1);

		$$ = new AccessExpr(ref, *$3);
		SET_LOCATION($$, @1, @3);

		delete $1;
		delete $3;
	}
	| THIS DOT IDENTIFIER {
		auto lhs = new IDReference("this");
		SET_LOCATION(lhs, @1, @1);

		$$ = new AccessExpr(lhs, *$3);
		SET_LOCATION($$, @1, @3);

		delete $1; delete $3;
	}
	| SUPER DOT IDENTIFIER {
		auto lhs = new SuperReference();
		SET_LOCATION(lhs, @1, @1);

		$$ = new AccessExpr(lhs, *$3);
		SET_LOCATION($$, @1, @3);

		delete $1; delete $3;
	}
	| AT IDENTIFIER {
		auto lhs = new IDReference("this");
		SET_LOCATION(lhs, @1, @1);

		$$ = new AccessExpr(lhs, *$2);
		SET_LOCATION($$, @1, @2);

		delete $1; delete $2;
	}
	| SIZEOF OPEN_PAREN expression CLOSE_PAREN { $$ = new SizeofExpr($3); SET_LOCATION($$, @1, @4); }
	| SIZEOF OPEN_PAREN type CLOSE_PAREN { $$ = new SizeofExpr($3); SET_LOCATION($$, @1, @4); }
	;

expr_list
	: expr_list COMMA expression
	{
		$$ = $1;
		$$->push_back($3);
	}
	| expression
	{
		$$ = new std::vector<Expression *>();
		$$->push_back($1);
	}
	;


return
	: RETURN
	{
		$$ = new ReturnStmt(nullptr);
		SET_LOCATION($$, @1, @1);
	}
	| RETURN expression
	{
		$$ = new ReturnStmt($2);
		SET_LOCATION($$, @1, @2);
	}
	;

var_decl
	: type var_decl_list
	{
		$$ = new std::vector<ASTNode*>();

		for (auto tupl : *$2)
		{
			VarDecl* decl = nullptr;

			if (module->getBlock()->is<ClassDecl*>())
			{
				decl = new MemberVarDecl($1, std::get<0>(tupl), std::get<1>(tupl));
			}
			else
			{
				decl = new VarDecl($1, std::get<0>(tupl), std::get<1>(tupl));
			}

			$$->push_back(decl);
    		SET_LOCATION(decl, @1, @2);
		}

		delete $2;
	}
	;

var_decl_list
	: var_decl_list COMMA IDENTIFIER
	{
		$$ = $1;
		$$->push_back(std::make_tuple(*$3, nullptr));

		delete $3;
	}
	| var_decl_list COMMA IDENTIFIER ASSIGN expression
	{
		$$ = $1;
		$$->push_back(std::make_tuple(*$3, $5));

		delete $3;
	}
	| IDENTIFIER
	{
		$$ = new std::vector<std::tuple<OString, Expression*>>();
		$$->push_back(std::make_tuple(*$1, nullptr));

		delete $1;
	}
	| IDENTIFIER ASSIGN expression
	{
		$$ = new std::vector<std::tuple<OString, Expression*>>();
		$$->push_back(std::make_tuple(*$1, $3));

		delete $1;
	}

enum_stmt
	: ENUM IDENTIFIER term enum_members END
	{
		auto estmt = new EnumStmt(*$2, module, Orange::IntType::get(module, 64));
		for (auto pair : *$4)
		{
			estmt->addMember(std::get<0>(pair), std::get<1>(pair));
		}

		$$ = estmt;
		SET_LOCATION($$, @1, @5);

		delete $2;
		delete $4;
	}

enum_members
	: enum_members IDENTIFIER term
	{
		$$->push_back(std::make_tuple(*$2, (Value *)nullptr));
		delete $2;
	}
	| enum_members IDENTIFIER ASSIGN pos_or_neg_value term
	{
		$$->push_back(std::make_tuple(*$2, $4));
		delete $2;
	}
	| IDENTIFIER term
	{
		$$ = new std::vector<std::tuple<OString, Value*>>();
		$$->push_back(std::make_tuple(*$1, (Value *)nullptr));
		delete $1;
	}
	| IDENTIFIER ASSIGN pos_or_neg_value term
	{
		$$ = new std::vector<std::tuple<OString, Value*>>();
		$$->push_back(std::make_tuple(*$1, $3));
		delete $1;
	}

pos_or_neg_value
	: VALUE
	{
		$$ = $1;
		SET_LOCATION($$, @1, @1);
	}
	| MINUS VALUE
	{
		$$ = $2;
		$2->negate();
		SET_LOCATION($$, @1, @2);
	}


term
	: NEWLINE
	| SEMICOLON
	;

non_agg_type
	: non_agg_type OPEN_BRACKET CLOSE_BRACKET
	{
		$$ = Orange::PointerType::get(module, $1);
	}
	| type TIMES
	{
		$$ = Orange::PointerType::get(module, $1);
	}
	| basic_type
	{
		$$ = $1;
	}
	;

type
	: CONST_FLAG type
	{
		$$ = $2->getConst();
	}
	| array_type
	{
		$$ = $1;
	}
	| non_agg_type
	{
		$$ = $1;
	}
	;

array_type
	: non_agg_type array_def_list
	{
		$$ = $1;

		bool is_const = true;
		for (unsigned int i = 0; i < $2->size(); i++)
		{
			auto def = $2->at(i);

			if (Orange::Type::exprValidForArrSize(def) == false)
			{
				is_const = false;
				break;
			}
		}

		int sz = (int)$2->size();
		for (int i = sz - 1; i >= 0; i--)
		{
			auto def = $2->at(i);

			if (is_const)
			{
				auto arr_sz = Orange::Type::exprAsArrSize(def);
				$$ = Orange::ArrayType::get(module, $$, arr_sz, false);
			}
			else
			{
				$$ = Orange::VariadicArrayType::get(module, $$, def, false);
			}
		}
	}
	;

array_def_list
	: array_def_list OPEN_BRACKET expression CLOSE_BRACKET
	{
		$$ = $1;
		$$->push_back($3);
	}
	| OPEN_BRACKET expression CLOSE_BRACKET
	{
		$$ = new std::vector<Expression *>();
		$$->push_back($2);
	}
	;

opt_protection_level
	: protection_level { $$ = $1; }
	| { $$ = nullptr; }
	;

protection_level
	: PRIVATE { $$ = new std::tuple<OString,ProtectionLevel>(std::make_tuple(*$1, ProtectionLevel::PROTECTION_PRIVATE)); delete $1;}
	| PROTECTED { $$ = new std::tuple<OString,ProtectionLevel>(std::make_tuple(*$1, ProtectionLevel::PROTECTION_PROTECTED)); delete $1; }
	| PUBLIC { $$ = new std::tuple<OString,ProtectionLevel>(std::make_tuple(*$1, ProtectionLevel::PROTECTION_PUBLIC)); delete $1; }
	;

opt_static
	: STATIC { $$ = $1; }
	| { $$ = nullptr; }
	;

basic_type
	: TYPE_INT { $$ = Orange::IntType::get(module, 64); }
	| TYPE_UINT { $$ = Orange::UIntType::get(module, 64); }
	| TYPE_FLOAT { $$ = Orange::FloatType::get(module); }
	| TYPE_DOUBLE { $$ = Orange::DoubleType::get(module); }
	| TYPE_INT8 { $$ = Orange::IntType::get(module, 8); }
	| TYPE_INT16 { $$ = Orange::IntType::get(module, 16); }
	| TYPE_INT32 { $$ = Orange::IntType::get(module, 32); }
	| TYPE_INT64 { $$ = Orange::IntType::get(module, 64); }
	| TYPE_UINT8 { $$ = Orange::UIntType::get(module, 8); }
	| TYPE_UINT16 { $$ = Orange::UIntType::get(module, 16); }
	| TYPE_UINT32 { $$ = Orange::UIntType::get(module, 32); }
	| TYPE_UINT64 { $$ = Orange::UIntType::get(module, 64); }
	| TYPE_CHAR { $$ = Orange::IntType::get(module, 8); }
	| TYPE_VOID { $$ = Orange::VoidType::get(module); }
	| TYPE_VAR { $$ = Orange::VarType::get(module); }
	| TYPE_ID {
		auto ty = new ReferenceType(*$1);
		delete $1;

		$$ = ty;
		SET_LOCATION(ty, @1, @1);
	}
	;

%%