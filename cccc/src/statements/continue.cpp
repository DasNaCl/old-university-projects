#include <statements/continue.hpp>

#include <sema/analyzer.hpp>
#include <sema/scope.hpp>

#include <ir/irconstructer.hpp>

#include <iostream>

namespace mhc4
{

namespace statements
{

_continue::_continue(const source_location& loc)
    : statement_semicolon_printer(loc)
{  }

void _continue::print(std::size_t) const
{
    std::cout << "continue";
}

void _continue::infer(semantics::scope& s) const
{
    if(!s.in_loop())
    {
        throw semantics::broken_semantic_exception(loc, "continue not used inside of a loop");
    }
}

void _continue::generate_ir(irconstructer& irc, IRScope& irscope)
{
    irc._builder.CreateBr(irscope.find_while_headBB());
    llvm::BasicBlock* dead_block =
        llvm::BasicBlock::Create(irc.ctx, "DEAD_BLOCK_continue",
                                 irc.current_fun, nullptr);
    irc._builder.SetInsertPoint(dead_block);
}

}

}
