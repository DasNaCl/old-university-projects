#include <expressions/member_access.hpp>
#include <expressions/variable.hpp>

#include <sema/analyzer.hpp>

#include <ir/irconstructer.hpp>

#include <types/struct.hpp>
#include <types/thunk.hpp>
#include <types/ptr.hpp>

#include <iostream>

namespace mhc4
{

namespace expressions
{

member_access::member_access(token tok, expression::ptr from, expression::ptr to)
    : expression(tok.get_location()), tok(tok),
      from_expr(std::move(from)), to_expr(std::move(to))
{  }

type::ptr member_access::_infer(semantics::scope& s)
{
    auto typ = from_expr->infer(s);

    if(tok.is_kind_of(tok::period) && !typ->is_struct())
    {
        semantics::throw_error(tok, "Member access must be of type struct");
    }
    else if(tok.is_kind_of(tok::arrow))
    {
        if(!typ->is_pointer())
        {
            semantics::throw_error(tok, "Need a left operand of type pointer to struct");
        }
        const auto* ptrtyp = types::util::thunk_cast<const types::ptr*>(typ.get());
        if(ptrtyp->pointer_count() != 1U)
        {
            semantics::throw_error(tok, "Pointer must have exactly a depth of 1");
        }
        if(!ptrtyp->base_type()->is_struct())
        {
            semantics::throw_error(tok, "Pointer must point to a struct type");
        }

        type::ptr ptstrct;
        if(dynamic_cast<expressions::variable*>(from_expr.get()))
        {
            auto* tmp = static_cast<expressions::variable*>(from_expr.get());
            ptstrct = std::move(tmp->struct_definition(s));
        }
        else if(dynamic_cast<expressions::member_access*>(from_expr.get()))
        {
            auto* tmp = static_cast<expressions::member_access*>(from_expr.get());
            ptstrct = std::move(tmp->struct_definition(s));
        }
        else
        {
            // We may not be anything else
            assert(false);
        }
        assert(dynamic_cast<types::_struct*>(ptstrct.get()));
        auto strct = static_cast<types::_struct*>(ptstrct.get());

        deq = strct->inner_scope(s);
        auto structscope = strct->inner_scope(s);
        pos_in_struct = strct->indexof(name_of_struct_member());
        return to_expr->infer(structscope.back());
    }
    assert(dynamic_cast<types::_struct*>(typ.get()));
    auto strct = static_cast<types::_struct*>(typ.get());

    deq = strct->inner_scope(s);
    auto structscope = strct->inner_scope(s);
    pos_in_struct = strct->indexof(name_of_struct_member());
    return to_expr->infer(structscope.back());
}

void member_access::print() const
{
    from_expr->print_expr();
    std::cout << tok.userdata();
    to_expr->print_expr();
}

llvm::Value* member_access::make_lvalue(irconstructer& irc, IRScope& irscope)
{
    return make_value(irc, irscope).second;
}

llvm::Value* member_access::make_rvalue(irconstructer& irc, IRScope& irscope)
{
    return make_value(irc, irscope).first;
}

bool member_access::is_l_value() const noexcept
{
    return to_expr->is_l_value();
}

std::pair<llvm::Value*, llvm::Value*> member_access::make_value(irconstructer& irc, IRScope& irscope)
{
    std::vector<llvm::Value*> indexes;
    indexes.push_back(irc._builder.getInt32(0));
    indexes.push_back(irc._builder.getInt32(pos_in_struct));

    //We do not codegen to_expr and instead just directly use
    //the index known from our static analysis

    auto* toLoad = from_expr->make_lvalue(irc, irscope);
    if(tok.is_kind_of(tok::arrow))
        toLoad = irc._builder.CreateLoad(toLoad);
    llvm::Value* valptr = irc._builder.CreateInBoundsGEP(toLoad, indexes);
    llvm::Value* val = irc._builder.CreateLoad(valptr);

    return std::make_pair(val, valptr);
}

type::ptr member_access::struct_definition(semantics::scope& s)
{
    auto* varexpr = dynamic_cast<expressions::variable*>(to_expr.get());
    // we should always be a variable
    assert(varexpr);
    return varexpr->struct_definition(deq.empty() ? s : deq.back());
}

token member_access::name_of_struct_member() const
{
    auto* varexpr = dynamic_cast<expressions::variable*>(to_expr.get());
    // we should always be a variable
    assert(varexpr);
    return varexpr->name();
}

}

}
