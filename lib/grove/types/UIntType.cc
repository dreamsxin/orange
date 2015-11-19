/*
** Copyright 2014-2015 Robert Fratto. See the LICENSE.txt file at the top-level
** directory of this distribution.
**
** Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file
** may not be copied, modified, or distributed except according to those terms.
*/

#include <grove/types/UIntType.h>
#include <grove/types/DoubleType.h>
#include <grove/types/FloatType.h>
#include <grove/types/IntType.h>
#include <grove/types/BoolType.h>
#include <grove/types/PointerType.h>

#include <grove/ASTNode.h>

#include <util/assertions.h>
#include <util/llvmassertions.h>

#include <llvm/IR/Type.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>

static int UIntToInt(Type* f, Type* t)
{
	auto from = dynamic_cast<UIntType *>(f);
	auto to = dynamic_cast<IntType *>(t);
	
	if (from->getIntegerBitWidth() > to->getIntegerBitWidth())
	{
		return llvm::Instruction::CastOps::Trunc;
	}
	else if (from->getIntegerBitWidth() == to->getIntegerBitWidth())
	{
		return 0;
	}
	else
	{
		return llvm::Instruction::CastOps::ZExt;
	}
}

static int UIntToUInt(Type* f, Type* t)
{
	auto from = dynamic_cast<UIntType *>(f);
	auto to = dynamic_cast<UIntType *>(t);
	
	if (from->getIntegerBitWidth() > to->getIntegerBitWidth())
	{
		return llvm::Instruction::CastOps::Trunc;
	}
	else if (from->getIntegerBitWidth() == to->getIntegerBitWidth())
	{
		return 0;
	}
	else
	{
		return llvm::Instruction::CastOps::ZExt;
	}
}

static llvm::Value* BoolCast(void* irBuilder, llvm::Value* val, Type* from,
							 Type* to)
{
	assertExists(irBuilder, "irbuilder must exist");
	assertExists(val, "val must exist");
	assertExists(from, "from must exist");
	assertExists(to, "to must exist");
	
	IRBuilder* IRB = (IRBuilder *)irBuilder;
	
	auto zero = llvm::ConstantInt::get(from->getLLVMType(), 0, false);
	assertEqual(zero, val, "Could not create bool cast");
	
	return IRB->CreateICmpNE(val, zero);
}

UIntType::UIntType(unsigned int width, bool isConst)
: Type(isConst)
{
	if (width == 0)
	{
		throw std::invalid_argument("width must not be 0.");
	}
	
	m_width = width;
	m_type = (llvm::Type *)llvm::Type::getIntNTy(*m_context, width);
	
	defineCast(typeid(UIntType), UIntToUInt);
	defineCast(typeid(IntType), UIntToInt);
	
	defineCast(typeid(DoubleType), llvm::Instruction::CastOps::UIToFP);
	defineCast(typeid(FloatType), llvm::Instruction::CastOps::UIToFP);
	defineCast(typeid(PointerType), llvm::Instruction::CastOps::IntToPtr);
	
	defineCast(typeid(BoolType), UIntToUInt, BoolCast);
}

unsigned int IntType::getIntegerBitWidth() const
{
	return m_width;
}

std::string UIntType::getSignature() const
{
	std::stringstream ss;
	ss << "I." << m_width;
	return ss.str();
}

bool UIntType::isPODTy() const
{
	return true;
}

bool UIntType::isSigned() const
{
	return false;
}

bool UIntType::isIntTy() const
{
	return true;
}

unsigned int UIntType::getIntegerBitWidth() const
{
	return m_width;
}

BasicType UIntType::PODTy() const
{
	switch (m_width)
	{
		case 1:
			return UINT1;
		case 8:
			return UINT8;
		case 16:
			return UINT16;
		case 32:
			return UINT32;
		case 64:
			return UINT64;
		default:
			return OTHER;
	}
}

Type* UIntType::getConst() const
{
	return UIntType::get(m_width, true);
}

UIntType* UIntType::get(unsigned int width, bool isConst)
{
	if (width == 0)
	{
		throw std::invalid_argument("width must not be 0.");
	}
	
	std::stringstream ss;
	ss << "I." << width;
	
	if (isConst)
	{
		ss << "!";
	}
	
	auto defined = getDefined(ss.str());
	if (defined != nullptr)
	{
		return dynamic_cast<UIntType*>(defined);
	}
	
	UIntType* ty = new UIntType(width, isConst);
	define(ss.str(), ty);
	
	return ty;
}