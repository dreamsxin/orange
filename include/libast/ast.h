/*
** Copyright 2014-2016 Robert Fratto. See the LICENSE.txt file at the top-level
** directory of this distribution.
**
** Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file
** may not be copied, modified, or distributed except according to those terms.
*/

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <utility>

namespace orange { namespace ast {
	class Type;
	
	enum PrivacyFlag { PRIVATE, PROTECTED, PUBLIC, PACKAGE };
	enum ConstFlag { MUTABLE, CONST };
	enum ModifierFlag { READ_WRITE, READONLY };
	enum LoopCheckTime { BEFORE_BODY, AFTER_BODY };
	
	enum UnaryOp {
		NEGATE, PREINCREMENT, POSTINCREMENT, NOT,
		BITWISE_NOT, PREDECREMENT, POSTDECREMENT,
		REFERENCE, DEFERENCE
	};
	
	enum BinaryOp {
		PLUS, MINUS, TIMES, DIVIDE, MODULO, LT, GT, LEQ, GEQ,
		EQ, NEQ, BIT_OR, BIT_AND, BIT_XOR, OR, AND, SHIFT_LEFT,
		SHIFT_RIGHT,
		
		PLUS_EQ, MINUS_EQ, TIMES_EQ, DIVIDE_EQ, MODULO_EQ,
		BIT_OR_EQ, BIT_AND_EQ, BIT_XOR_EQ, SHIFT_LEFT_EQ,
		SHIFT_RIGHT_EQ
	};

	struct Node {
	protected:
		Node();
	public:
		uintmax_t id; 

		virtual ~Node();
	};
	
	struct VarDeclExpr;
	
	struct Statement  : Node {
	protected:
		Statement() { }
	};
	
	struct Expression : Node {
	protected:
		Expression() { }
	};
	
	struct GenericDecl : Node {
		std::vector<std::string> types;
	};
	
	struct Identifier : Node {
	protected:
		Identifier() { }
	};
	
	struct AccessIdentifier : Identifier {
		std::unique_ptr<Identifier> base;
		std::string name;
	};
	
	struct NameIdentifier : Identifier {
		std::string name;
	};
	
	struct DtorIdentifier : Identifier {
		std::string name;
	};
	
	struct AliasStmt : Statement {
		std::string name;
		std::unique_ptr<Identifier> original;
	};
	
	struct ClassStmt : Statement {
		std::string name;
		std::vector<std::unique_ptr<Identifier>> parents;
		std::vector<std::unique_ptr<Node>> statements;
	};
	
	struct EnumStmt : Statement {
		std::string name;
		std::vector<std::string> enumerations;
	};
	
	struct ExtensionStmt : Statement {
		std::string name;
		std::vector<std::unique_ptr<Identifier>> parents;
		std::vector<std::unique_ptr<Node>> statements;
	};
	
	struct InterfaceStmt : Statement {
		std::string name;
		std::vector<std::unique_ptr<Identifier>> parents;
		std::vector<std::unique_ptr<Node>> statements;
	};
	
	struct LoopStmt : Statement {
		std::unique_ptr<Expression> initializer;
		std::unique_ptr<Expression> condition;
		std::unique_ptr<Expression> afterthought;
		
		LoopCheckTime checkWhen;
		
		std::vector<std::unique_ptr<Node>> statements;
	};
	
	struct PackageStmt : Statement {
		std::unique_ptr<Identifier> name;
	};
	
	struct NamespaceStmt : Statement {
		std::unique_ptr<Identifier> name;
	};
	
	struct GetStmt : Statement {
		std::vector<std::unique_ptr<Node>> statements;
	};
	
	struct SetStmt : Statement {
		std::unique_ptr<VarDeclExpr> parameter;
		std::vector<std::unique_ptr<Node>> statements;
	};
	
	struct PropertyStmt : Statement {
		ModifierFlag propertyModifiers;
		std::string name;
		
		std::unique_ptr<GetStmt> getter;
		std::unique_ptr<SetStmt> setter;
	};
	
	struct ArrayExpr : Expression {
		std::vector<std::unique_ptr<Expression>> values;
	};
	
	struct IndexExpr : Expression {
		std::unique_ptr<Expression> base;
		std::unique_ptr<Expression> index;
	};
	
	struct CallExpr : Expression {
		std::unique_ptr<Expression> function;
		std::vector<std::unique_ptr<Expression>> arguments;
	};
	
	struct AccessExpr : Expression {
		std::unique_ptr<Expression> base;
		std::string name;
	};
	
	struct UnaryExpr : Expression {
		UnaryOp op;
		std::unique_ptr<Expression> base;
	};
	
	struct BinaryExpr : Expression {
		BinaryOp op;
		std::unique_ptr<Expression> lhs;
		std::unique_ptr<Expression> rhs;
	};
	
	struct CastExpr : Expression {
		std::unique_ptr<Expression> base;
		std::unique_ptr<Type> toType;
	};
	
	struct FunctionExpr : Expression {
		std::unique_ptr<Identifier> name;
		std::unique_ptr<GenericDecl> generics;
		std::vector<std::unique_ptr<VarDeclExpr>> params;
		std::unique_ptr<Type> retType;
		std::vector<std::unique_ptr<Node>> statements;
	};
	
	struct ExternFunctionExpr : Expression {
		std::string name;
		std::vector<std::unique_ptr<VarDeclExpr>> params;
		std::unique_ptr<Type> retType;
	};
	
	struct VarDeclExpr : Expression {
		std::string name;
		std::unique_ptr<Type> type;
		std::unique_ptr<Expression> value;
	};
	
	struct IntExpr : Expression {
		unsigned bitwidth;
		intmax_t value;
	};
	
	struct UIntExpr : Expression {
		unsigned bitwidth;
		uintmax_t value;
	};
	
	struct FloatExpr : Expression {
		float value;
	};
	
	struct DoubleExpr : Expression {
		double value;
	};
	
	struct CharExpr : Expression {
		char value;
	};
	
	struct StringExpr : Expression {
		std::string value;
	};
	
	struct BoolExpr : Expression {
		bool value;
	};
	
	struct IDRefExpr : Expression {
		std::string name;
	};
}}
