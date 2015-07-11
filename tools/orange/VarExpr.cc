/*
** Copyright 2014-2015 Robert Fratto. See the LICENSE.txt file at the top-level 
** directory of this distribution.
**
** Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file 
** may not be copied, modified, or distributed except according to those terms.
*/ 

#include <orange/VarExpr.h>
#include <orange/generator.h>


Value* VarExpr::Codegen() {
	return m_value;
}

std::string VarExpr::string() {
	return "(" + getType()->string() + ")" + m_name; 
}

void VarExpr::setType(OrangeTy* type) {
	m_type = type;
}

bool VarExpr::isLocked() {
	if (m_locked) return true; 

	return false;
}

void VarExpr::resolve() {
	if (m_resolved) return; 
	m_resolved = true;

	if (getType()->isVariadicArray()) {
		std::vector<Expression *> variadicElements; 

		auto ptr = getType(); 
		while (ptr->isVariadicArray()) {
			variadicElements.push_back(ptr->getVariadicArrayElement());
			ptr = ptr->getArrayElementType();
		}

		for (auto expr : variadicElements) {
			if (expr) expr->resolve();
		}
	}
}

void VarExpr::initialize() {
	// Tries to add this variable to the symbol table.
	// If it already exists in the symbol table, and it's not this, throw an error.
	SymTable* tab = GE::runner()->topBlock()->symtab();

	if (tab->create(m_name, this) == false) {
		// We couldn't create it because it already exists. 
		// If the ASTNode in the table isn't this var, throw an error.
		if (tab->find(m_name, this) && tab->find(m_name, this) != this) {
			throw CompilerMessage(*this, "A variable by this name already exists!");
		} 
	}
}

bool VarExpr::existsInParent() {
	SymTable* tab = GE::runner()->topBlock()->symtab();
	return tab->find(m_name, this) != nullptr;
}

void VarExpr::createValue(Value* value) {
	m_value = GE::builder()->CreateAlloca(value->getType(), nullptr, m_name);
	GE::builder()->CreateStore(value, m_value);
}

void VarExpr::setValue(Value *value) {
	m_value = value; 
}

bool VarExpr::returnsPtr() {
	return true;
}


Value* VarExpr::allocate() {
	if (m_value != nullptr) {
		throw CompilerMessage(*this, "variable already allocated!");
	}

	// If we're variadic array, getLLVMType() will just return type*. 
	// However, we need to actually initialize ourselves with the number 
	// of elements, so handle that here 

	Type* llvmType = getLLVMType();
	
	if (llvmType == nullptr) {
		throw std::runtime_error("VarExpr::allocate(): getLLVMType returned nullptr!");
	}
	
	if (getType()->isVariadicArray()) {
		Type* baseType = llvmType; 
		while (baseType->isPointerTy()) {
			baseType = baseType->getPointerElementType();
		}

		std::vector<Expression *> variadicElements; 

		auto ptr = getType(); 
		while (ptr->isVariadicArray()) {
			variadicElements.push_back(ptr->getVariadicArrayElement());
			ptr = ptr->getArrayElementType();
		}

		// Codegen all of the types into values 
		std::vector<Value*> values; 

		for (auto expr : variadicElements) {
			auto value = expr->Codegen();
			
			if (value == nullptr) {
				throw std::runtime_error("VarExpr::allocate(): expr did not generate a value!");
			}

			if (expr->returnsPtr() == true) {
				value = GE::builder()->CreateLoad(value);
			}

			if (value->getType()->isIntegerTy() == false) {
				throw CompilerMessage(*expr, "Type is not an integer!");
			}

			values.push_back(value);
		}

		// The total size of this array is all the elements in the vector multiplied together.
		Value *totSize = values[0]; 
		for (unsigned int i = 1; i < values.size(); i++) {
			totSize = GE::builder()->CreateMul(totSize, values[i]);
		}

		Value *v = GE::builder()->CreateAlloca(baseType, totSize, name());
		setValue(v);
		return v;
	}

	Value* v = GE::builder()->CreateAlloca(llvmType, nullptr, name());
	setValue(v);
	return v; 
}

VarExpr::VarExpr(std::string name) : m_name(name) { 

}

VarExpr::VarExpr(std::string name, bool constant) : m_name(name) {
	m_constant = constant;
}

VarExpr::VarExpr(std::string name, OrangeTy* type) : m_name(name) {
	m_locked = true;
	m_type = type; 
}

VarExpr::VarExpr(std::string name, OrangeTy* type, bool constant) : m_name(name) {
	m_locked = true;
	m_constant = constant;
	m_type = type; 
}
