/*
** Copyright 2014-2015 Robert Fratto. See the LICENSE.txt file at the top-level
** directory of this distribution.
**
** Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file
** may not be copied, modified, or distributed except according to those terms.
*/

#include <iostream>

#include <test/Test.h>
#include <test/TestLib.h>

Test::Test(TestFunction func, std::string desc)
{
	this->func = func;
	this->desc = desc;

	TestingEngine::shared()->addTest(this);
}
