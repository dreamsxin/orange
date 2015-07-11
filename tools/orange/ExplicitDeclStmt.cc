/*
** Copyright 2014-2015 Robert Fratto. See the LICENSE.txt file at the top-level 
** directory of this distribution.
**
** Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file 
** may not be copied, modified, or distributed except according to those terms.
*/ 

#include <orange/ExplicitDeclStmt.h>
#include <orange/VarExpr.h>
#include <orange/generator.h>

Value* ExplicitDeclStmt::CodegenPair(VarExpr* var, Expression* expr) {
	// If we have an expression, cast it to the type of our variable and assign it.
	if (var->getType()->isVarTy()) {
		throw CompilerMessage(*var, "var type never given a value");
	}

	if (expr) {
		Value* value = expr->Codegen();

		// If the expr returns a pointer, don't load if LHS is a pointer and RHS is an array 
		bool doLoad = expr->returnsPtr();
		if (doLoad && var->getType()->isPointerTy() && expr->getType()->isArrayTy()) {
			doLoad = false;
		}

		if (doLoad) {
			value = GE::builder()->CreateLoad(value);
		}
		
		bool arrayToArray = expr->getType()->isArrayTy() && var->getType()->isArrayTy(); 

		if (arrayToArray == false) {
			bool casted = CastingEngine::CastValueToType(&value, var->getType(), var->isSigned(), true);
			if (casted == false) {
				throw CompilerMessage(*expr, "Could not cast expression to variable type!");
			}
		}

		var->allocate();

		GE::builder()->CreateStore(value, var->getValue());
	} else {
		// Otherwise, just allocate the variable.
		var->allocate();
	}

	m_value = var->Codegen();
	return m_value;
}

Value* ExplicitDeclStmt::Codegen() {
	auto retVal = CodegenPair(m_var, m_expr);

	// Codegen our extra pairs.
	for (auto pair : m_extras) {
		CodegenPair(pair.var, pair.val);
	}

	// Only return the first variable.
	return retVal;
}

ASTNode* ExplicitDeclStmt::clone() {
	std::vector<DeclPair> extraClones;

	for (auto pair : m_extras) {
		extraClones.push_back(DeclPair(pair.var->name(), (Expression *)pair.val->clone()));
	}

	ASTNode* clone = nullptr; 

	if (m_expr) {
		clone = new ExplicitDeclStmt((VarExpr*)m_var->clone(), (Expression*)m_expr->clone(), extraClones);
	} else {
		clone = new ExplicitDeclStmt((VarExpr*)m_var->clone(), extraClones);
	}

	clone->copyProperties(this);
	return clone;
}

bool ExplicitDeclStmt::isSigned() {
	return m_var->isSigned();
}

void ExplicitDeclStmt::resolve() {
	ASTNode::resolve();

	if (m_resolved) return; 
	m_resolved = true;

	//m_var->create();

	if (m_var->getType()->isVoidTy()) {
		throw CompilerMessage(*m_var, "keyword void cannot be used to create a variable");
	}
	
	if (m_var->getType()->isVarTy() && m_expr) {
		m_var->setType(m_expr->getType());
		m_var->resolve(); 
	} 

	m_var->getType()->resolve();

	if (m_var->getType()->isVariadicArray() && m_expr) {
		throw CompilerMessage(*m_var, "Variable-sized arrays may not be initialized");
	}

	for (auto pair : m_extras) {
		//pair.var->create();
		pair.var->getType()->resolve();
		
		if (pair.var->getType()->isVoidTy()) {
			throw CompilerMessage(*m_var, "keyword void cannot be used to create a variable");
		}

		if (pair.var->getType()->isVarTy() && pair.val) {
			pair.var->setType(pair.val->getType()); 
			pair.var->resolve(); // type has changed; re-resolve the variable 
		}

		if (pair.var->getType()->isVariadicArray() && pair.val) {
			throw CompilerMessage(*pair.var, "Variable-sized arrays may not be initialized");
		}
	}

	m_type = m_var->getType();
}

std::string ExplicitDeclStmt::string() {
	std::stringstream ss; 

	ss << m_type->string() << " ";

	if (m_expr) {
		ss << m_var->string() << " = " << m_expr->string();
	} else {
		ss << m_var->string();
	}

	for (auto pair : m_extras) {
		ss << "\n";

		if (pair.val) {
			ss << pair.var->string() << " = " << pair.val->string(); 
		} else {
			ss << pair.var->string();
		}
	}
	
	return ss.str();
}

ExplicitDeclStmt::ExplicitDeclStmt(VarExpr* var) {
	if (var == nullptr) {
		throw std::runtime_error("ExplicitDeclStmt ctor: var can not be null!");
	}

	m_var = var;
	addChild("m_var", m_var);
}

ExplicitDeclStmt::ExplicitDeclStmt(VarExpr* var, std::vector<DeclPair> extras) {
	if (var == nullptr) {
		throw std::runtime_error("ExplicitDeclStmt ctor: var can not be null!");
	}

	m_var = var;
	addChild("m_var", m_var);

	auto typeForExtras = m_var->getType();

	for (auto pair : extras) {
		auto newVar = new VarExpr(pair.name, typeForExtras); 

		addChild(newVar);

		if (pair.expression) {
			addChild(pair.expression);
		}

		m_extras.push_back(DeclPairInternal(newVar, pair.expression));
	}	
}


ExplicitDeclStmt::ExplicitDeclStmt(VarExpr* var, Expression* value) {
	if (var == nullptr) {
		throw std::runtime_error("ExplicitDeclStmt ctor: var can not be null!");
	}

	if (value == nullptr) {
		throw std::runtime_error("ExplicitDeclStmt ctor: value can not be null!");
	}

	m_var = var;
	m_expr = value; 

	addChild("m_var", m_var);
	addChild("m_expr", m_expr);
}

ExplicitDeclStmt::ExplicitDeclStmt(VarExpr* var, Expression* value, std::vector<DeclPair> extras) {
	if (var == nullptr) {
		throw std::runtime_error("ExplicitDeclStmt ctor: var can not be null!");
	}

	if (value == nullptr) {
		throw std::runtime_error("ExplicitDeclStmt ctor: value can not be null!");
	}

	m_var = var;
	m_expr = value; 

	addChild("m_var", m_var);
	addChild("m_expr", m_expr);

	auto typeForExtras = m_var->getType();


	for (unsigned int i = 0; i < extras.size(); i++) {
		auto pair = extras[i];

		auto newVar = new VarExpr(pair.name, typeForExtras); 

		addChild(newVar);

		if (pair.expression) {
			addChild(pair.expression);
		}

		m_extras.push_back(DeclPairInternal(newVar, pair.expression));
	}
}
