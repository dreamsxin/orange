/*
** Copyright 2014-2015 Robert Fratto. See the LICENSE.txt file at the top-level
** directory of this distribution.
**
** Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file
** may not be copied, modified, or distributed except according to those terms.
*/

#include <grove/types/DoubleType.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Instruction.h>

#include <grove/types/UIntType.h>
#include <grove/types/IntType.h>
#include <grove/types/FloatType.h>

DoubleType::DoubleType(bool isConst)
: Type(isConst)
{
	m_type = llvm::Type::getDoubleTy(*m_context);

	defineCast(typeid(UIntType), llvm::Instruction::CastOps::FPToUI);
	defineCast(typeid(IntType), llvm::Instruction::CastOps::FPToSI);
	defineCast(typeid(FloatType), llvm::Instruction::CastOps::FPTrunc);
}

std::string DoubleType::getString() const
{
	std::stringstream ss;
	
	if (isConst())
	{
		ss << "const ";
	}
	
	ss << "double";
	return ss.str();
}

std::string DoubleType::getSignature() const
{
	std::stringstream ss;
	
	if (isConst())
	{
		ss << getConstIdentifier();
	}
	
	ss << "F";
	
	return ss.str();
}

bool DoubleType::isPODTy() const
{
	return true;
}

bool DoubleType::isDoubleTy() const
{
	return true;
}

bool DoubleType::isFloatingPointTy() const
{
	return true;
}

BasicType DoubleType::PODTy() const
{
	return TYDOUBLE;
}

Type* DoubleType::getConst() const
{
	return DoubleType::get(true);
}

DoubleType* DoubleType::get(bool isConst)
{
	std::stringstream ss;
	
	if (isConst)
	{
		ss << getConstIdentifier();
	}
	
	ss << "F";
	
	auto defined = getDefined(ss.str());
	if (defined != nullptr)
	{
		return defined->as<DoubleType*>();
	}

	DoubleType* ty = new DoubleType(isConst);
	define(ss.str(), ty);

	return ty;
}
