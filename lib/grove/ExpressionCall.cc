/*
** Copyright 2014-2015 Robert Fratto. See the LICENSE.txt file at the top-level
** directory of this distribution.
**
** Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file
** may not be copied, modified, or distributed except according to those terms.
*/

#include <grove/ExpressionCall.h>

#include <grove/types/Type.h>
#include <grove/types/FunctionType.h>
#include <grove/types/IntType.h>
#include <grove/types/UIntType.h>
#include <grove/types/DoubleType.h>

#include <grove/exceptions/code_error.h>

#include <util/assertions.h>

#include <llvm/IR/IRBuilder.h>

ExpressionCall::ExpressionCall(std::vector<Expression *> args)
{
	for (auto& arg : args)
	{
		addChild(arg, true);
	}
	
	m_expr = nullptr;
	m_args = args;
}

ExpressionCall::ExpressionCall(ASTNode* expr, std::vector<Expression *> args,
							   bool addAsChild)
{
	assertExists(expr, "ExpressionCall created with no expression");
	
	if (expr->is<Valued *>() == false)
	{
		throw code_error(expr,
			[]() -> std::string
			{
				return "Calling a valueless element";
			});
	}
	
	if (expr->is<Typed *>() == false)
	{
		throw code_error(expr,
		[]() -> std::string
		{
			return "Calling a typeless element";
		});
	}
	
	if (addAsChild)
	{
		addChild(expr, true);
	}
	
	for (auto& arg : args)
	{
		addChild(arg, true);
	}
	
	m_expr = expr;
	m_args = args;
}

void ExpressionCall::assertCallingFunction(llvm::Value* function)
{
	auto llvm_ty = function->getType();
	
	if (llvm_ty->getNumContainedTypes() > 0)
	{
		llvm_ty = llvm_ty->getContainedType(llvm_ty->getNumContainedTypes() - 1);
	}
	
	if (llvm_ty->isFunctionTy() == false)
	{
		throw fatal_error("About to call a non-function!");
	}
}

const FunctionType* ExpressionCall::getFunctionTy() const
{
	return m_function_ty;
}

ASTNode* ExpressionCall::copy() const
{
	/// @todo Figure out how to copy nodes that are constructed
	/// with references to other nodes.
	///
	/// We can't just copy m_expr; we need to construct
	/// a copy with
	throw fatal_error("NYI: ExpressionCall::copy()");
}

void ExpressionCall::setExpr(ASTNode *expr)
{
	if (expr->is<Valued *>() == false)
	{
		throw code_error(expr,
						 []() -> std::string
						 {
							 return "Calling a valueless element";
						 });
	}
	
	if (expr->is<Typed *>() == false)
	{
		throw code_error(expr,
						 []() -> std::string
						 {
							 return "Calling a typeless element";
						 });
	}
	
	m_expr = expr;
}

const Valued* ExpressionCall::exprAsValued() const
{
	return m_expr->as<const Valued *>();
}

const Typed* ExpressionCall::exprAsTyped() const
{
	return m_expr->as<const Typed *>();
}

const ASTNode* ExpressionCall::getExpr() const
{
	return m_expr;
}

std::vector<Expression *> ExpressionCall::getArgs() const
{
	return m_args;
}

void ExpressionCall::findDependencies()
{
	ASTNode::findDependencies();
	
	// m_expr isn't a child, so we're
	// adding it as a dependency here.
	addDependency(m_expr);
}

void ExpressionCall::resolve()
{
	auto ty = exprAsTyped()->getType();
	if (ty->isFunctionTy() == false)
	{
		throw code_error(this, m_expr,
			[]() -> std::string
			{
				return "attempting to call an expression that is not a "
					"function";
			});
	}
	
	auto func_ty = ty->as<FunctionType *>();
	
	if (!func_ty->isVarArg() && m_args.size() != func_ty->getArgs().size())
	{
		throw code_error(this, m_expr,
			[]() -> std::string
			{
				return "number of arguments in function call do "
					"not match function parameters";
			});
	}
	else if (func_ty->isVarArg() && m_args.size() < func_ty->getArgs().size())
	{
		throw code_error(this, m_expr,
			[]() -> std::string
			{
				return "not enough arguments in function call to "
					"call variable argument sized function";
			});
	}
	
	m_function_ty = func_ty;
	
	setType(func_ty->getReturnTy());
}

void ExpressionCall::build()
{
	std::vector<llvm::Value *> llvm_args;
	
	auto func_ty = getFunctionTy();
	auto num_args = func_ty->getArgs().size();
	
	for (unsigned int i = 0; i < num_args; i++)
	{
		auto param_ty = func_ty->getArgs()[i];
		
		auto arg = m_args[i];
		arg->build();
		
		// Cast the argument if it doesn't match the parameter.
		if (arg->getType() != param_ty)
		{
			llvm_args.push_back(arg->castTo(param_ty));
		}
		else
		{
			llvm_args.push_back(arg->getValue());
		}
	}
	
	// Add remaining arguments if it is vaarg.
	for (unsigned int i = num_args; i < m_args.size(); i++)
	{
		auto arg = m_args[i];
		arg->build();
		
		auto arg_ty = arg->getType();
		
		// To meet with C calling conventials, arugments that are in the
		// "variable argument" section of the argument list are upcasted
		// in certain conditions:
		// 	- integers lower than 32 bits are casted to int32
		// 	- floats are casted to doubles.
		if (arg_ty->isIntTy() && arg_ty->getIntegerBitWidth() < 32)
		{
			if (arg_ty->isSigned())
			{
				llvm_args.push_back(arg->castTo(IntType::get(getModule(),
															 32)));
			}
			else
			{
				llvm_args.push_back(arg->castTo(UIntType::get(getModule(),
															  32)));
			}
		}
		else if (arg_ty->isFloatTy())
		{
			llvm_args.push_back(arg->castTo(DoubleType::get(getModule())));
		}
		else
		{
			llvm_args.push_back(arg->getValue());
		}
	}
	
	// Make sure we're about to call a function.
	auto function = exprAsValued()->getValue();
	assertCallingFunction(function);
	
	auto res = IRBuilder()->CreateCall(function, llvm_args);
	setValue(res);
}