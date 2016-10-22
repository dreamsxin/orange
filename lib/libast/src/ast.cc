/*
** Copyright 2014-2016 Robert Fratto. See the LICENSE.txt file at the top-level
** directory of this distribution.
**
** Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file
** may not be copied, modified, or distributed except according to those terms.
*/

#include <libast/ast.h>

uintmax_t lastID = 0;

using namespace orange::ast;

Node::Node() { this->id = lastID++; }
Node::~Node() { /* empty */ }

