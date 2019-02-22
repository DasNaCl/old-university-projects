#include <statements/goto.hpp>

#include <statements/label.hpp>
#include <ir/irconstructer.hpp>

#include <sema/scope.hpp>

#include <iostream>

namespace mhc4
{

namespace statements
{

_goto::_goto(const source_location& loc, token id)
    : statement_semicolon_printer(loc), id(id)
{  }

void _goto::print(std::size_t) const
{
    std::cout << "goto " << id.userdata();
}

void _goto::infer(semantics::scope& s) const
{
    s.insert_goto(id);
}

void _goto::generate_ir(irconstructer& irc, IRScope& irscope)
{
    /*
    auto* lab = irscope.find_label(id.userdata());
    if(lab)
        irc._builder.CreateBr(lab);
    else
    {
        // we need to wait and insert the instruction later
        // as soon as the label got declared
        irscope.highest().marked_gotos[id.userdata()].push_back(irc._builder.GetInsertBlock());
    }
    llvm::BasicBlock* dead_block =
        llvm::BasicBlock::Create(irc.ctx, "DEAD_BLOCK_goto",
                                 irc.current_fun, nullptr);
    irc._builder.SetInsertPoint(dead_block);
    */
}

}

}
