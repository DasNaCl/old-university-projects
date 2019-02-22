#include <statements/while.hpp>
#include <statements/block.hpp>

#include <sema/analyzer.hpp>
#include <sema/scope.hpp>

#include <ir/irconstructer.hpp>

#include <iostream>

namespace mhc4
{

namespace statements
{

_while::_while(const source_location& loc, expression::ptr condition,
               statement::ptr body, _while::kind t)
    : statement(loc), condition(std::move(condition)), body(std::move(body)), typ(t)
{  }

void _while::print(std::size_t d) const
{
    if(typ == kind::normal)
    {
        std::cout << "while (";
        condition->print_expr();
        std::cout << ")";

        std::size_t depth = d;
        if(body->is_block())
        {
            block* ptr = dynamic_cast<block*>(body.get());
            ptr->do_not_indent_first();
        }
        else
        {
            std::cout << "\n";
            depth++;
        }
        body->print_statement(depth);
    }
    else
    {
        // this is bonus, thus not well maintained :(

        std::cout << "do";
        std::size_t depth = d;
        if(body->is_block())
        {
            block* ptr = dynamic_cast<block*>(body.get());
            ptr->do_not_indent_first();
            ptr->do_not_add_newline();
        }
        else
        {
            std::cout << "\n";
            depth++;
        }
        body->print_statement(depth);
        if(body->is_block())
            std::cout << " ";
        std::cout << "while (";
        condition->print_expr();
        std::cout << ")";
    }
}

void _while::infer(semantics::scope& s) const
{
    semantics::scope scop = s.inherit();
    scop.set_in_loop();

    auto typ = condition->infer(s);
    if(!typ->is_scalar())
    {
        semantics::throw_error(loc, "Condition is not of scalar type");
    }

    body->infer(scop);
}

void _while::generate_ir(irconstructer& irc, IRScope& irscope)
{
    assert(irc.current_fun);

    llvm::BasicBlock* headerBB =
        llvm::BasicBlock::Create(irc.ctx, "while-header",
                                 irc.current_fun, nullptr);
    irc._builder.CreateBr(headerBB);
    irc._builder.SetInsertPoint(headerBB);

    llvm::BasicBlock* bodyBB =
        llvm::BasicBlock::Create(irc.ctx, "while-body",
                                 irc.current_fun, nullptr);
    llvm::BasicBlock* endBB =
        llvm::BasicBlock::Create(irc.ctx, "while-end",
                                 irc.current_fun, nullptr);
    condition->make_cf(irc, irscope, bodyBB, endBB);
    irc._builder.SetInsertPoint(bodyBB);

    IRScope inner = irscope.inherit();
    inner.set_while_headBB(headerBB);
    inner.set_while_endBB(endBB);
    {
    auto* b = dynamic_cast<block*>(body.get());
    if(b)
        b->generate_ir_after_block(irc, inner);
    else
        body->generate_ir(irc, inner);
    }
    inner.set_while_headBB(nullptr);
    inner.set_while_endBB(nullptr);

    irc._builder.CreateBr(headerBB);
    irc._builder.SetInsertPoint(endBB);
}

}

}
