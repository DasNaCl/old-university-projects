#include <statements/break.hpp>

#include <sema/analyzer.hpp>
#include <sema/scope.hpp>

#include <ir/irconstructer.hpp>

#include <iostream>

namespace mhc4
{

namespace statements
{

_break::_break(const source_location& loc)
    : statement_semicolon_printer(loc)
{  }

void _break::print(std::size_t) const
{
    std::cout << "break";
}

void _break::infer(semantics::scope& s) const
{
    if(!s.in_loop())
    {
        throw semantics::broken_semantic_exception(loc, "break not used inside of a loop");
    }
}

void _break::generate_ir(irconstructer& irc, IRScope& irscope)
{
    irc._builder.CreateBr(irscope.find_while_endBB());
    llvm::BasicBlock* dead_block =
        llvm::BasicBlock::Create(irc.ctx, "DEAD_BLOCK_break",
                                 irc.current_fun, nullptr);
    irc._builder.SetInsertPoint(dead_block);
}

}

}
