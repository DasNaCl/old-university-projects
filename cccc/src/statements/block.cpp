#include <statements/block.hpp>

#include <sema/scope.hpp>

#include <ir/irconstructer.hpp>

#include <iostream>

namespace mhc4
{

namespace statements
{

block::block(const source_location& loc, std::vector<statement::ptr> statements)
    : statement(loc), statements(std::move(statements)),
      indent_first(true), newline(true)
{  }

void block::do_not_indent_first() noexcept
{
    indent_first = false;
}

void block::do_not_add_newline() noexcept
{
    newline = false;
}

bool block::is_block() const noexcept
{
    return true;
}

void block::infer(semantics::scope& s) const
{
    semantics::scope son = s.inherit();
    for(const auto& z : statements)
    {
        z->infer(son);
    }
}

void block::generate_ir(irconstructer& irc, IRScope& irscope)
{
    //auto* oldBB = irc._builder.GetInsertBlock();

    llvm::BasicBlock* block =
        llvm::BasicBlock::Create(irc.ctx, "block",
                                 irc.current_fun, nullptr);
    irc._builder.CreateBr(block);
    irc._builder.SetInsertPoint(block);

    auto scop = irscope.inherit();
    for(auto& stmt : statements)
        stmt->generate_ir(irc, scop);

    /*
    irc._builder.CreateBr(oldBB);
    irc._builder.SetInsertPoint(oldBB);
    */
}

void block::generate_ir_after_block(irconstructer& irc, IRScope& irscope)
{
    for(auto& stmt : statements)
        stmt->generate_ir(irc, irscope);
}

void block::infer_in_scope(semantics::scope& s) const
{
    for(const auto& z : statements)
    {
        z->infer(s);
    }
}

void block::print_statement(std::size_t depth) const
{
    //simply propagate
    print(depth);
}

void block::print(std::size_t depth) const
{
    if(indent_first)
    {
        for(std::size_t d = depth; d > 0; --d)
            std::cout << "\t";
    }
    else
    {
        std::cout << " ";
    }
    std::cout << "{\n";

    for(auto& s : statements)
        s->print_statement(depth + 1);

    for(std::size_t d = depth; d > 0; --d)
        std::cout << "\t";
    std::cout << "}";
    if(newline)
        std::cout << "\n";
}

}

}
