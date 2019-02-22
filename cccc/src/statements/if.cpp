#include <statements/if.hpp>
#include <statements/block.hpp>

#include <sema/analyzer.hpp>
#include <sema/scope.hpp>

#include <ir/irconstructer.hpp>

#include <iostream>

namespace mhc4
{

namespace statements
{

_if::_if(const source_location& loc, expression::ptr condition,
         statement::ptr consequence)
    : statement(loc), condition(std::move(condition)),
      consequence(std::move(consequence)), alternative(util::nullopt),
      indent_first(true)
{  }

_if::_if(const source_location& loc, expression::ptr condition,
         statement::ptr consequence, statement::ptr alternative)
    : statement(loc), condition(std::move(condition)),
      consequence(std::move(consequence)), alternative(std::move(alternative)),
      indent_first(true)
{  }

bool _if::is_if() const noexcept
{
    return true;
}

void _if::do_not_indent_first() noexcept
{
    indent_first = false;
}

void _if::print(std::size_t d) const
{
    if(indent_first)
    {
        for(std::size_t dd = d; dd > 0; --dd)
            std::cout << "\t";
    }
    else
        std::cout << " ";
    
    std::cout << "if (";
    condition->print_expr();
    std::cout << ")";

    std::size_t depth = d;
    if(!consequence->is_block())
    {
        std::cout << "\n";
        depth++;
    }
    else
    {
        block* ptr = dynamic_cast<block*>(consequence.get());
        ptr->do_not_indent_first();
        if(alternative)
        {
            ptr->do_not_add_newline();
        }
    }
    consequence->print_statement(depth);// if not a block statement

    if(alternative)
    {
        if(!consequence->is_block())
        {
            for(std::size_t dd = d; dd > 0; --dd)
                std::cout << "\t";
        }
        else
            std::cout << " ";
        std::cout << "else";

        depth = d;
        const bool isblock = alternative.get()->is_block();
        const bool isif = alternative.get()->is_if();
        if(!isblock && !isif)
        {
            std::cout << "\n";
            depth++;
        }
        else if(!isblock && isif)
        {
            _if* ptr = dynamic_cast<_if*>(alternative.get().get());
            ptr->do_not_indent_first();
        }
        else
        {
            block* ptr = dynamic_cast<block*>(alternative.get().get());
            ptr->do_not_indent_first();
        }
        alternative.get()->print_statement(depth);
    }
}

void _if::print_statement(std::size_t d) const
{
    print(d);
}

void _if::infer(semantics::scope& s) const
{
    semantics::scope scop = s.inherit();

    auto typ = condition->infer(s);
    if(!typ->is_scalar())
    {
        semantics::throw_error(loc, "Condition is not of scalar type.");
    }

    consequence->infer(scop);
    if(alternative.has())
    {
        alternative.get()->infer(scop);
    }
}

void _if::generate_ir(irconstructer& irc, IRScope& irscope)
{
    assert(irc.current_fun);

    llvm::BasicBlock* headerBB =
        llvm::BasicBlock::Create(irc.ctx, "if-header",
                                 irc.current_fun, nullptr);
    irc._builder.CreateBr(headerBB);
    irc._builder.SetInsertPoint(headerBB);

    llvm::BasicBlock* conseqBB =
        llvm::BasicBlock::Create(irc.ctx, "if-consequence",
                                 irc.current_fun, nullptr);

    // right after our if
    llvm::BasicBlock* endBB = nullptr;
    if(alternative.has())
    {
        llvm::BasicBlock* alternBB =
            llvm::BasicBlock::Create(irc.ctx, "if-alternative",
                                     irc.current_fun, nullptr);
        condition->make_cf(irc, irscope, conseqBB, alternBB);

        irc._builder.SetInsertPoint(conseqBB);
        {
        IRScope inner = irscope.inherit();
        auto* b = dynamic_cast<block*>(consequence.get());
        if(b)
            b->generate_ir_after_block(irc, inner);
        else
            consequence->generate_ir(irc, inner);
        }

        endBB = llvm::BasicBlock::Create(irc.ctx, "if-end",
                                         irc.current_fun, nullptr);

        irc._builder.CreateBr(endBB);
        irc._builder.SetInsertPoint(alternBB);

        {
        IRScope inner = irscope.inherit();
        auto* b = dynamic_cast<block*>(alternative.get().get());
        if(b)
            b->generate_ir_after_block(irc, inner);
        else
            alternative.get()->generate_ir(irc, inner);
        }
        irc._builder.CreateBr(endBB);
    }
    else
    {
        irc._builder.SetInsertPoint(conseqBB);

        IRScope inner = irscope.inherit();
        auto* b = dynamic_cast<block*>(consequence.get());
        if(b)
            b->generate_ir_after_block(irc, inner);
        else
            consequence->generate_ir(irc, inner);
        endBB = llvm::BasicBlock::Create(irc.ctx, "if-end",
                                         irc.current_fun, nullptr);
        irc._builder.CreateBr(endBB);
        irc._builder.SetInsertPoint(headerBB);
        condition->make_cf(irc, irscope, conseqBB, endBB);
    }

    irc._builder.SetInsertPoint(endBB);
}

}

}
