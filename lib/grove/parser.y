/*
** Copyright 2014-2015 Robert Fratto. See the LICENSE.txt file at the top-level
** directory of this distribution.
**
** Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file
** may not be copied, modified, or distributed except according to those terms.
*/

%{
	#include <grove/Module.h>
	#include <grove/ASTNode.h>
	#include <grove/Block.h>
	#include <grove/CondBlock.h>
	#include <grove/Function.h>
	#include <grove/IfStmt.h>
	#include <grove/FunctionCall.h>
	#include <grove/ExternFunction.h>
	#include <grove/Value.h>
	#include <grove/StrValue.h>
	#include <grove/Expression.h>
	#include <grove/ReturnStmt.h>
	#include <grove/BinOpCompare.h>
	#include <grove/BinOpArith.h>
	#include <grove/BinOpAssign.h>
	#include <grove/BinOpAndOr.h>
	#include <grove/Parameter.h>
	#include <grove/IDReference.h>
	#include <grove/NegativeExpr.h>
	#include <grove/VarDecl.h>
	#include <grove/IncrementExpr.h>
	#include <grove/Loop.h>
	#include <grove/LoopTerminator.h>
	#include <grove/DerefExpr.h>
	#include <grove/ReferenceExpr.h>
	#include <grove/CastExpr.h>
	#include <grove/ArrayValue.h>
	#include <grove/ArrayAccessExpr.h>
	#include <grove/TernaryExpr.h>
	#include <grove/AccessExpr.h>
	#include <grove/EnumStmt.h>
	#include <grove/SizeofExpr.h>
	#include <grove/OString.h>

	#include <grove/types/Type.h>
	#include <grove/types/IntType.h>
	#include <grove/types/UIntType.h>
	#include <grove/types/FloatType.h>
	#include <grove/types/DoubleType.h>
	#include <grove/types/VoidType.h>
	#include <grove/types/PointerType.h>
	#include <grove/types/VarType.h>
	#include <grove/types/ArrayType.h>
	#include <grove/types/VariadicArrayType.h>

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
	ASTNode* node;
	Block* block;
	Expression* expr;
	Statement* stmt;
	Value* val;
	OString* str;
	Type* ty;
}

%start start

%token DEF END IF ELIF ELSE TYPE_ID OPEN_PAREN CLOSE_PAREN TYPE COMMA
%token TIMES NUMBER DIVIDE MINUS PLUS NEWLINE SEMICOLON
%token TYPE_INT TYPE_UINT TYPE_FLOAT TYPE_DOUBLE TYPE_INT8 TYPE_UINT8 TYPE_INT16
%token TYPE_UINT16 TYPE_INT32 TYPE_UINT32 TYPE_INT64 TYPE_UINT64 TYPE_CHAR TYPE_VOID TYPE_VAR
%token RETURN CLASS USING PUBLIC SHARED PRIVATE OPEN_BRACE CLOSE_BRACE
%token OPEN_BRACKET CLOSE_BRACKET INCREMENT DECREMENT ASSIGN PLUS_ASSIGN
%token MINUS_ASSIGN TIMES_ASSIGN DIVIDE_ASSIGN MOD_ASSIGN ARROW ARROW_LEFT
%token DOT LEQ GEQ COMP_LT COMP_GT MOD VALUE STRING EXTERN VARARG EQUALS NEQUALS WHEN
%token UNLESS LOGICAL_AND LOGICAL_OR BITWISE_AND BITWISE_OR BITWISE_XOR
%token FOR FOREVER LOOP CONTINUE BREAK DO WHILE
%token CONST_FLAG QUESTION COLON ENUM SIZEOF

%type <nodes> opt_statements statements compound_statement var_decl valued
%type <nodes> opt_valued
%type <node> statement return controls
%type <exprs> expr_list array_def_list
%type <pairs> var_decl_list
%type <vpairs> enum_members
%type <blocks> else_if_or_end
%type <expr> expression primary comparison arithmetic call increment
%type <expr> opt_expression ternary
%type <stmt> structures function extern_function ifs inline_if unless
%type <stmt> inline_unless for_loop inline_for_loop enum_stmt
%type <val> VALUE pos_or_neg_value
%type <str> COMP_LT COMP_GT LEQ GEQ PLUS MINUS TYPE_ID STRING TIMES DIVIDE ASSIGN
%type <str> EQUALS NEQUALS PLUS_ASSIGN TIMES_ASSIGN MINUS_ASSIGN DIVIDE_ASSIGN
%type <str> MOD MOD_ASSIGN BITWISE_AND BITWISE_OR BITWISE_XOR LOGICAL_AND
%type <str> LOGICAL_OR LOOP CONTINUE BREAK
%type <ty> type basic_type type_hint non_agg_type array_type
%type <params> param_list
%type <args> arg_list

/* lowest to highest precedence */
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

%%

start
  : statements
	{
		for (auto stmt : *$1)
		{
			module->getMain()->addStatement(stmt);
		}

		delete $1;
	}
	;

statements
	: statements statement
	{
		$$ = $1;

		if ($2 != nullptr)
		{
    		$$->push_back($2);
		}
	}
	| statements compound_statement
	{
		$$ = $1;

		for (auto stmt : *$2)
		{
			if (stmt == nullptr) continue;
			$$->push_back(stmt);
		}

		delete $2;
	}
	| statement
	{
		$$ = new std::vector<ASTNode *>();

		if ($1 != nullptr)
		{
			$$->push_back($1);
		}
	}
	| compound_statement
	{
		$$ = new std::vector<ASTNode *>();

		for (auto stmt : *$1)
		{
			if (stmt == nullptr) continue;
			$$->push_back(stmt);
		}

		delete $1;
	}
	;

opt_statements
	: statements { $$ = $1; }
	| { $$ = new std::vector<ASTNode *>(); }
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
	| ifs { $$ = $1; }
	| unless { $$ = $1; }
	| inline_if { $$ = $1; }
	| inline_unless { $$ = $1; }
	| for_loop { $$ = $1; }
	| inline_for_loop { $$ = $1; }
	| enum_stmt { $$ = $1; }
	;

function
 	: DEF TYPE_ID OPEN_PAREN CLOSE_PAREN type_hint term opt_statements END
	{
		auto func = new Function(*$2, std::vector<Parameter *>());
		func->setReturnType($5);

		for (auto stmt : *$7)
		{
			func->addStatement(stmt);
		}

		$$ = func;
        SET_LOCATION($$, @1, @8);

		delete $2;
		delete $7;
	}
	| DEF TYPE_ID OPEN_PAREN param_list CLOSE_PAREN type_hint term opt_statements END
	{
		auto func = new Function(*$2, *$4);
		func->setReturnType($6);

		for (auto stmt : *$8)
		{
			func->addStatement(stmt);
		}

		$$ = func;
        SET_LOCATION($$, @1, @9);

		delete $2;
		delete $4;
		delete $8;
	}
	;

type_hint
	: ARROW type { $$ = $2; }
	| { $$ = nullptr; }
	;

extern_function
	: EXTERN TYPE_ID OPEN_PAREN CLOSE_PAREN ARROW type
	{
		std::vector<Parameter *> params;
		$$ = new ExternFunction(*$2, params, $6);
		SET_LOCATION($$, @1, @6);

		delete $2;
	}
	| EXTERN TYPE_ID OPEN_PAREN param_list CLOSE_PAREN ARROW type
	{
		$$ = new ExternFunction(*$2, *$4, $7);
		SET_LOCATION($$, @1, @7);

		delete $2;
		delete $4;
	}
	| EXTERN TYPE_ID OPEN_PAREN param_list COMMA VARARG CLOSE_PAREN ARROW type
	{
		$$ = new ExternFunction(*$2, *$4, $9, true);
		SET_LOCATION($$, @1, @9);

		delete $2;
		delete $4;
	}
	;

ifs
	: IF expression term statements else_if_or_end
	{
		auto blocks = $5;

		auto block = new CondBlock($2);
		for (auto stmt : *$4)
		{
			block->addStatement(stmt);
		}

		blocks->insert(blocks->begin(), block);

		auto if_stmt = new IfStmt();
		for (auto block : *blocks)
		{
			if_stmt->addBlock(block);
		}

		$$ = if_stmt;
		SET_LOCATION($$, @1, @5);

		delete $4;
		delete blocks;
	}
	;

else_if_or_end
	: ELIF expression term statements else_if_or_end
	{
		$$ = $5;

		auto block = new CondBlock($2);
		for (auto stmt : *$4)
		{
			block->addStatement(stmt);
		}

		$$->insert($$->begin(), block);

		SET_LOCATION(block, @1, @5);

		delete $4;
	}
	| ELSE term statements END
	{
		$$ = new std::vector<Block *>();

		auto block = new Block();
		for (auto stmt : *$3)
		{
			block->addStatement(stmt);
		}

		$$->insert($$->begin(), block);

		SET_LOCATION(block, @1, @4);

		delete $3;
	}
	| END
	{
		$$ = new std::vector<Block *>();
	}
	;

unless
	: UNLESS expression term statements END
	{
		auto block = new CondBlock($2, true);
		for (auto stmt : *$4)
		{
			block->addStatement(stmt);
		}

		auto if_stmt = new IfStmt();
		if_stmt->addBlock(block);

		$$ = if_stmt;
		SET_LOCATION($$, @1, @5);

		delete $4;
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
	  CLOSE_PAREN term statements END
	{
		auto loop = new Loop(*$3, $5, $7, false);

	  	for (auto stmt : *$10)
		{
			loop->addStatement(stmt);
		}

		$$ = loop;
		SET_LOCATION($$, @1, @11);

		delete $3;
		delete $10;
	}
	| WHILE expression term statements END
	{
		auto loop = new Loop(std::vector<ASTNode*>(), $2, nullptr, false);

		for (auto stmt : *$4)
		{
			loop->addStatement(stmt);
		}

		$$ = loop;
		SET_LOCATION($$, @1, @5);

		delete $4;
	}
	| FOREVER DO term statements END
	{
		auto loop = new Loop(std::vector<ASTNode*>(), nullptr, nullptr, false);

		for (auto stmt : *$4)
		{
			loop->addStatement(stmt);
		}

		$$ = loop;
		SET_LOCATION($$, @1, @5);

		delete $4;
	}
	| DO term statements END WHILE expression
	{
		auto loop = new Loop(std::vector<ASTNode*>(), $6, nullptr, true);

		for (auto stmt : *$3)
		{
			loop->addStatement(stmt);
		}

		$$ = loop;
		SET_LOCATION($$, @1, @6);

		delete $3;
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
	: param_list COMMA type TYPE_ID
	{
		$$ = $1;
		auto param = new Parameter($3, *$4);
		$$->push_back(param);
		SET_LOCATION(param, @1, @4);

		delete $4;
	}
	| type TYPE_ID
	{
		$$ = new std::vector<Parameter *>();
		auto param = new Parameter($1, *$2);
		$$->push_back(param);
		SET_LOCATION(param, @1, @2);

		delete $2;
	}

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
	: TYPE_ID OPEN_PAREN CLOSE_PAREN
	{
		std::vector<Expression *> params;
		$$ = new FunctionCall(*$1, params);
		SET_LOCATION($$, @1, @3);

		delete $1;
	}
	| TYPE_ID OPEN_PAREN arg_list CLOSE_PAREN
	{
		$$ = new FunctionCall(*$1, *$3);
		SET_LOCATION($$, @1, @4);

		delete $1;
		delete $3;
	}
	;

primary
	: OPEN_PAREN expression CLOSE_PAREN { $$ = $2; }
	| VALUE { $$ = $1; SET_LOCATION($$, @1, @1); }
	| MINUS expression { $$ = new NegativeExpr($2); SET_LOCATION($$, @1, @2); }
	| STRING { $$ = new StrValue(*$1); SET_LOCATION($$, @1, @1); delete $1; }
	| TYPE_ID { $$ = new IDReference(*$1); SET_LOCATION($$, @1, @1); delete $1; }
	| TIMES expression { $$ = new DerefExpr($2); SET_LOCATION($$, @1, @2); }
	| BITWISE_AND expression { $$ = new ReferenceExpr($2); SET_LOCATION($$, @1, @2); }
	| OPEN_PAREN type CLOSE_PAREN expression { $$ = new CastExpr($2, $4); SET_LOCATION($$, @1, @4); }
	| OPEN_BRACKET expr_list CLOSE_BRACKET { $$ = new ArrayValue(*$2); SET_LOCATION($$, @1, @3); delete $2; }
	| expression OPEN_BRACKET expression CLOSE_BRACKET { $$ = new ArrayAccessExpr($1, $3); SET_LOCATION($$, @1, @4); }
	| expression DOT TYPE_ID { $$ = new AccessExpr($1, *$3); SET_LOCATION($$, @1, @3); delete $3; }
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
			auto decl = new VarDecl($1, std::get<0>(tupl), std::get<1>(tupl));
			$$->push_back(decl);
    		SET_LOCATION(decl, @1, @2);
		}

		delete $2;
	}
	;

var_decl_list
	: var_decl_list COMMA TYPE_ID
	{
		$$ = $1;
		$$->push_back(std::make_tuple(*$3, nullptr));

		delete $3;
	}
	| var_decl_list COMMA TYPE_ID ASSIGN expression
	{
		$$ = $1;
		$$->push_back(std::make_tuple(*$3, $5));

		delete $3;
	}
	| TYPE_ID
	{
		$$ = new std::vector<std::tuple<OString, Expression*>>();
		$$->push_back(std::make_tuple(*$1, nullptr));

		delete $1;
	}
	| TYPE_ID ASSIGN expression
	{
		$$ = new std::vector<std::tuple<OString, Expression*>>();
		$$->push_back(std::make_tuple(*$1, $3));

		delete $1;
	}

enum_stmt
	: ENUM TYPE_ID term enum_members END
	{
		auto estmt = new EnumStmt(*$2, IntType::get(64));
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
	: enum_members TYPE_ID term
	{
		$$->push_back(std::make_tuple(*$2, (Value *)nullptr));
		delete $2;
	}
	| enum_members TYPE_ID ASSIGN pos_or_neg_value term
	{
		$$->push_back(std::make_tuple(*$2, $4));
		delete $2;
	}
	| TYPE_ID term
	{
		$$ = new std::vector<std::tuple<OString, Value*>>();
		$$->push_back(std::make_tuple(*$1, (Value *)nullptr));
		delete $1;
	}
	| TYPE_ID ASSIGN pos_or_neg_value term
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
		$$ = PointerType::get($1);
	}
	| type TIMES
	{
		$$ = PointerType::get($1);
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

			if (Type::exprValidForArrSize(def) == false)
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
				auto arr_sz = Type::exprAsArrSize(def);
				$$ = ArrayType::get($$, arr_sz, false);
			}
			else
			{
				$$ = VariadicArrayType::get($$, def, false);
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

basic_type
	: TYPE_INT { $$ = IntType::get(64); }
	| TYPE_UINT { $$ = UIntType::get(64); }
	| TYPE_FLOAT { $$ = FloatType::get(); }
	| TYPE_DOUBLE { $$ = DoubleType::get(); }
	| TYPE_INT8 { $$ = IntType::get(8); }
	| TYPE_INT16 { $$ = IntType::get(16); }
	| TYPE_INT32 { $$ = IntType::get(32); }
	| TYPE_INT64 { $$ = IntType::get(64); }
	| TYPE_UINT8 { $$ = UIntType::get(8); }
	| TYPE_UINT16 { $$ = UIntType::get(16); }
	| TYPE_UINT32 { $$ = UIntType::get(32); }
	| TYPE_UINT64 { $$ = UIntType::get(64); }
	| TYPE_CHAR { $$ = IntType::get(8); }
	| TYPE_VOID { $$ = VoidType::get(); }
	| TYPE_VAR { $$ = VarType::get(); }
	;

%%
