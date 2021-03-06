/*
** Copyright 2014-2015 Robert Fratto. See the LICENSE.txt file at the top-level
** directory of this distribution.
**
** Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file
** may not be copied, modified, or distributed except according to those terms.
*/

#include <grove/Typed.h>
#include <grove/types/Type.h>
#include <util/assertions.h>

Type* Typed::getType() const
{
	return m_type;
}

llvm::Type* Typed::getLLVMType() const
{
	if (m_type == nullptr)
	{
		return nullptr;
	}
	
	return getType()->getLLVMType();
}

Comparison Typed::compare(Typed *source, Typed *target)
{
	assertExists(source, "source type must exist.");
	assertExists(target, "target type must exist.");
	
	auto source_ty = source->getType();
	auto target_ty = target->getType();
	
	assertExists(source_ty, "source didn't have a type");
	assertExists(target_ty, "target didn't have a type");
	
	return Type::compare(source_ty, target_ty);
}

bool Typed::matchesType(Type *ty) const
{
	return m_type->matches(ty);
}

void Typed::setType(Type* type)
{
	m_type = type;
}