/*
** Copyright 2014-2015 Robert Fratto. See the LICENSE.txt file at the top-level
** directory of this distribution.
**
** Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file
** may not be copied, modified, or distributed except according to those terms.
*/

#include <grove/ReturnStmt.h>
#include <grove/Function.h>
#include <grove/Expression.h>

#include <grove/types/VoidType.h>

#include <grove/exceptions/code_error.h>

#include <util/assertions.h>
#include <util/llvmassertions.h>

#include <llvm/IR/IRBuilder.h>

Expression* ReturnStmt::getExpression()
{
	return m_expr;
}

bool ReturnStmt::hasExpression()
{
	return m_expr != nullptr;
}

bool ReturnStmt::isTerminator() const
{
	return true;
}

ASTNode* ReturnStmt::copy() const
{
	if (m_expr)
	{
		return new ReturnStmt(m_expr->copy()->as<Expression *>());
	}
	else
	{
		return new ReturnStmt(nullptr);
	}
}

void ReturnStmt::resolve()
{
	Statement::resolve();
	
	if (hasExpression())
	{
		setType(getExpression()->getType());
	}
	else
	{
		setType(VoidType::get());
	}
}

void ReturnStmt::build()
{
	auto func = findParent<Function *>();
	assertExists(func, "Couldn't find parent function of ReturnStmt");

	if (hasExpression())
	{
		getExpression()->build();
	
    	auto value = getExpression()->getValue();
		
		auto ret_ty = getType();
    	if (ret_ty != func->getReturnType())
    	{
    		switch (Type::compare(getType(), func->getReturnType()))
    		{
    			case LOWER_PRECEDENCE:
    			case HIGHER_PRECEDENCE:
    				value = getExpression()->castTo(func->getReturnType());
    				break;
    			case INCOMPATIBLE:
					throw code_error(this, func, [ret_ty, func]() -> std::string
						{
							std::stringstream ss;
							ss << "Return type with value "
							   << ret_ty->getString() << " cannot be converted "
							   << "to function return type "
							   << func->getReturnType()->getString();
							
							return ss.str();
						});
					break;
    			default:
    				break;
    		}
    	}
		
		auto ptr = func->getRetValue();
		
    	assertEqual<VAL,PTR>(value, ptr, "Can't store incompatible type.");
		
		IRBuilder()->CreateStore(value, ptr);
	}
	
	IRBuilder()->CreateBr(func->getExit());
}

ReturnStmt::ReturnStmt(Expression* expression)
{
	addChild(expression);
	
	m_expr = expression;
}