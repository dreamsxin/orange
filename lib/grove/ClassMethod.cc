/*
** Copyright 2014-2015 Robert Fratto. See the LICENSE.txt file at the top-level
** directory of this distribution.
**
** Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file
** may not be copied, modified, or distributed except according to those terms.
*/

#include <grove/ClassMethod.h>
#include <grove/ClassDecl.h>
#include <grove/Parameter.h>

#include <grove/types/Type.h>

#include <util/assertions.h>

Type* ClassMethod::getBasicType() const
{
	/// Just return getType until a this parameter is added.
	return getType();
}

std::vector<Parameter *> ClassMethod::getBasicParams() const
{
	/// Just return getParams until a this parameter is added.
	return getParams();
}

void ClassMethod::findDependencies()
{
	Function::findDependencies();
	
	auto parentClass = getParent()->as<ClassDecl *>();
	addDependency(parentClass);
}

void ClassMethod::resolve()
{
	auto parentClass = getParent()->as<ClassDecl *>();
	assertExists(parentClass->getType(), "Class has no defined type");
	auto this_type = parentClass->getType()->getPointerTo();
	
	auto this_param = new Parameter(this_type, "this");
	
	addChild(this_param, 0);
	
	m_params.insert(m_params.begin(), this_param);
	
	Function::resolve();
}

ClassMethod::ClassMethod(OString name, std::vector<Parameter *> params)
: Function(name, params)
{
	// Do nothing.
}