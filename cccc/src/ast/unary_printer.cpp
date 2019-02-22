#include <ast/unary_printer.hpp>

#include <sema/analyzer.hpp>

#include <types/integral.hpp>
#include <types/thunk.hpp>
#include <types/ptr.hpp>

#include <ir/irconstructer.hpp>

#include <iostream>
#include <cassert>

namespace mhc4
{

namespace ast
{

unary_printer::unary_printer(token op, bool postfix, expression::ptr operand)
    : expression(op.get_location()), op(op), postfix(postfix), operand(std::move(operand))
{
    if(postfix && (op.get_kind() != tok::plusplus && op.get_kind() != tok::minusminus))
        semantics::throw_error(op, "Not a valid unary postfix operator.");
}

type::ptr unary_printer::_infer(semantics::scope& s)
{
    auto optyp = operand->infer(s);

    type::ptr result(nullptr);
    switch(op.get_kind())
    {
    case tok::plus:
    case tok::minus:
    {
        if(!optyp->is_arithmetic())
        {
            semantics::throw_error(op, "No arithmetic type to apply \"+\" to!");
        }
        result = util::make_unique<types::_integral>(tok::keyword_int);
    } break;

    case tok::exclaim:
    {
        if(!optyp->is_scalar())
        {
            semantics::throw_error(op, "No scalar type to apply \"!\" to!");
        }
        result = util::make_unique<types::_integral>(tok::keyword_int);
    } break;

    case tok::plusplus:
    case tok::minusminus:
    {
        // not implemented
    } break;


    case tok::amp:
    {
        if(!operand->is_l_value())
        {
            semantics::throw_error(op, "Can't take the reference of an rvalue!");
        }

        result = util::make_unique<types::ptr>(std::move(optyp));
    } break;

    case tok::star:
    {
        // dereference
        if(optyp->is_pointer())
        {
            return std::move(types::util::non_const_thunk_cast<types::ptr*>(optyp.get())->dereference());
        }
        else
        {
            // we may be a function, then it doesn't really
            // matter, whether we dereference or not
            if(!optyp->is_function())
            {
                semantics::throw_error(op, "Can't dereference a non-pointer!");
            }
            result = optyp->copy();
        }
    } break;

    default:
        assert(false);
        break;
    }
    return result;
}

llvm::Value* unary_printer::make_rvalue(irconstructer& irc, IRScope& irscope)
{
    switch(op.get_kind())
    {
    case tok::plus:
    {
        auto* v = operand->make_rvalue(irc, irscope);
        return irc.integer_promotion(v);
    }
    case tok::minus:
    {
        auto* v = operand->make_rvalue(irc, irscope);
        v = irc.integer_promotion(v);
        return irc._builder.CreateMul(v, irc._builder.getInt32(-1), "unary-minus");
    }
    case tok::exclaim:

    case tok::plusplus:
    case tok::minusminus: return nullptr;

    case tok::amp:  return operand->make_lvalue(irc, irscope);
    case tok::star:
    {
        if(operand->__get_expr_typ()->is_function())
        {
            return operand->make_lvalue(irc, irscope);
        }
        else if(operand->__get_expr_typ()->is_pointer())
        {
            auto* address = operand->make_rvalue(irc, irscope);

            auto* gt = operand->__get_expr_typ().get();
            auto* pt = dynamic_cast<types::ptr*>(gt);
            assert(pt);
            if(pt->base_type()->is_function())
                return address;
            else
                return irc._builder.CreateLoad(address);
        }
    } break;

    default: assert(false);
    }
    return nullptr;
}

llvm::Value* unary_printer::make_lvalue(irconstructer& irc, IRScope& irscope)
{
    switch(op.get_kind())
    {
    case tok::star: {
        if(operand->__get_expr_typ()->is_function())
        {
            return operand->make_lvalue(irc, irscope);
        }
        return operand->make_rvalue(irc, irscope);
    }

    // all of these shouldn't produce an lvalue
    case tok::amp:
    case tok::plus:
    case tok::minus:
    case tok::exclaim:
    case tok::plusplus:   //<- besides those maybe, however,
    case tok::minusminus: //<- they are not implemented
    default: assert(false); return nullptr;
    }
}

void unary_printer::print() const
{
    if(postfix)
    {
        operand->print_expr();
        std::cout << op.userdata();
    }
    else
    {
        std::cout << op.userdata();
        operand->print_expr();
    }
}

bool unary_printer::is_l_value() const noexcept
{
    switch(op.get_kind())
    {
    case tok::plus:
    case tok::minus:
    case tok::exclaim:
    case tok::plusplus:
    case tok::minusminus:
    {
        return false;
    } break;

    case tok::star:
    case tok::amp:
    {
        return true;
    } break;

    default:
        break;
    }
    // uh oh
    assert(false);
    return false;
}

}

}
