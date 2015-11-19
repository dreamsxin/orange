/*
** Copyright 2014-2015 Robert Fratto. See the LICENSE.txt file at the top-level
** directory of this distribution.
**
** Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file
** may not be copied, modified, or distributed except according to those terms.
*/

#include <grove/IfStmt.h>
#include <grove/Function.h>
#include <grove/CondBlock.h>
#include <grove/Expression.h>
#include <grove/Module.h>
#include <grove/types/Type.h>

#include <util/assertions.h>
#include <util/copy.h>

#include <llvm/IR/IRBuilder.h>

std::vector<Block *> IfStmt::getBlocks() const
{
	return m_if_blocks;
}

std::vector<llvm::BasicBlock *> IfStmt::getLLVMBlocks() const
{
	return m_blocks;
}

llvm::BasicBlock* IfStmt::continueBlock() const
{
	return m_continue_block;
}

void IfStmt::addBlock(Block *block)
{
	assertExists(block, "Block must exist!");
	
	if (m_if_blocks.size() == 0 && block->is<CondBlock *>() == false)
	{
		throw std::invalid_argument("First block must be a CondBlock!");
	}
	else if (hasElse())
	{
		throw std::invalid_argument("No other blocks can be added!");
	}
	
	m_if_blocks.push_back(block);
	addChild(block);
}

llvm::Value* IfStmt::getCond(CondBlock *block)
{
	auto val = block->getExpression()->getValue();
	auto ty = block->getExpression()->getType();
	assertExists(val, "val is null!");
	assertExists(ty, "val type is null!");
	
	auto bool_ty = llvm::IntegerType::get(getModule()->getLLVMContext(),
										  1);
	return IRBuilder()->CreateIntCast(val, bool_ty, ty->isSigned());
}

bool IfStmt::isElse(Block *block)
{
	return block->is<CondBlock *>() == false;
}

ASTNode* IfStmt::copy() const
{
	auto ret = new IfStmt();
	ret->m_if_blocks = copyVector(m_if_blocks);
	return ret;
}

void IfStmt::resolve()
{
	if (m_if_blocks.size() == 0)
	{
		throw std::invalid_argument("IfStmt needs at least one block");
	}
}

void IfStmt::build()
{
	// There will always be m_if_blocks.size() if bodies.
	// There will always be m_if_blocks.size() - 1 if checks.
	// The final if check _may_ not necessarily be a check.
	
	// First, get our current function.
	auto parent_func = findParent<Function *>();
	auto llvm_func = parent_func->getLLVMFunction();
	
	std::vector<llvm::BasicBlock *> if_checks;
	
	// Generate all the if checks first
	for (unsigned int i = 1; i < getBlocks().size(); i++)
	{
		auto bb = llvm::BasicBlock::Create(getModule()->getLLVMContext(),
										   "if_check", llvm_func,
										   parent_func->getExit());
		if_checks.push_back(bb);
	}
	
	// Now, generate all the bodies.
	for (unsigned int i = 0; i < getBlocks().size(); i++)
	{
		auto bb = llvm::BasicBlock::Create(getModule()->getLLVMContext(),
										   "if_body", llvm_func,
										   parent_func->getExit());
		m_blocks.push_back(bb);
	}
	
	// Finally, create the continue block.
	m_continue_block = llvm::BasicBlock::Create(getModule()->getLLVMContext(),
												"if_continue", llvm_func,
												parent_func->getExit());
	
	// Add continue to the end of if_checks.
	if_checks.push_back(m_continue_block);
	
	// Create all of our if statements.
	for (unsigned int i = 0; i < getBlocks().size(); i++)
	{
		auto block = getBlocks().at(i);
		auto if_body = getLLVMBlocks().at(i);
		auto next = if_checks.at(i);
		
		if (isElse(block) == false)
		{
			auto cond = block->as<CondBlock *>();
			cond->getExpression()->build();
			IRBuilder()->CreateCondBr(getCond(cond), if_body, next);
		}
		else // handle else block
		{
			IRBuilder()->CreateBr(if_body);
		}
		
		// Now, set insert point to the body and generate the block.
		IRBuilder()->SetInsertPoint(if_body);
		block->build();
		
		// If the block isn't a terminator, go to continue.
		if (block->isTerminator() == false)
		{
			IRBuilder()->CreateBr(m_continue_block);
		}
		
		IRBuilder()->SetInsertPoint(next);
	}
}

bool IfStmt::isTerminator() const
{
	for (auto block : m_if_blocks)
	{
		if (block->isTerminator() == false)
		{
			return false;
		}
	}
	
	return true;
}

bool IfStmt::hasElse()
{
	for (auto block : m_if_blocks)
	{
		if (block->is<CondBlock *>() == false)
		{
			return true;
		}
	}
	
	return false;
}

IfStmt::IfStmt()
{
	// Do nothing.
}