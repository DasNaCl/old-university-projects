#include <statements/label.hpp>

#include <ir/irconstructer.hpp>

#include <sema/scope.hpp>

#include <iostream>

namespace mhc4
{

namespace statements
{

label::label(const source_location& loc, token id, statement::ptr follow)
    : statement(loc), id(id), stmt(std::move(follow))
{  }

void label::print(std::size_t d) const
{
    std::cout << id.userdata() << ":\n";
    stmt->print_statement(d);
}

void label::print_statement(std::size_t depth) const
{
    // do not indent label
    print(depth);
}

void label::infer(semantics::scope& s) const
{
    s.insert_label(id);
    stmt->infer(s);
}

void label::generate_ir(irconstructer& irc, IRScope& irscope)
{
    /*
    llvm::BasicBlock* labelBB =
        llvm::BasicBlock::Create(irc.ctx, "label-block",
                                 irc.current_fun, nullptr);
    irscope.insert_label(id.userdata(), labelBB);

    for(auto& gotoBB : irscope.highest().marked_gotos[id.userdata()])
    {
        irc._builder.SetInsertPoint(gotoBB);
        irc._builder.CreateBr(labelBB);
    }
    irscope.highest().marked_gotos[id.userdata()].clear();

    irc._builder.SetInsertPoint(labelBB);
    */
    stmt->generate_ir(irc, irscope);
}

}

}
