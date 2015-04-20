/*
** Copyright 2014-2015 Robert Fratto. See the LICENSE.txt file at the top-level 
** directory of this distribution.
**
** Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file 
** may not be copied, modified, or distributed except according to those terms.
*/ 

#include <orange/ArrayExpr.h>

Value* ArrayExpr::Codegen() {
	throw std::runtime_error("NYI: ArrayExpr::Codegen()");
}

ASTNode* ArrayExpr::clone() {
	throw std::runtime_error("NYI: ArrayExpr::clone()");
}

AnyType* ArrayExpr::getType() {
	// If we don't have any elements, we're an int*. 
	if (m_elements.size() == 0) {
		return AnyType::getIntNTy(64)->getPointerTo();
	}

	// We need to get the highest precedence type from all the elements.
	AnyType* highestType = m_elements[0]->getType();

	for (int i = 1; i < m_elements.size(); i++) {
		highestType = CastingEngine::GetFittingType(highestType, m_elements[i]->getType());
	}

	return highestType->getArray(m_elements.size());
}

void ArrayExpr::resolve() {
	if (m_resolved) return;
	m_resolved = true ;

	for (auto element : m_elements) {
		element->resolve();
	}
}

ArrayExpr::ArrayExpr(ArgList elements) {
	m_elements = elements;
}