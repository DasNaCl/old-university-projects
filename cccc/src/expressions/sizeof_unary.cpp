#include <expressions/sizeof_unary.hpp>
#include <expressions/variable.hpp>
#include <expressions/string.hpp>

#include <types/integral.hpp>

#include <sema/analyzer.hpp>

#include <ir/irconstructer.hpp>

#include <iostream>
#include <cassert>

namespace mhc4
{

namespace expressions
{

sizeof_::sizeof_(token op, bool, expression::ptr operand)
    : expression(op.get_location()), op(op),
      operand(std::move(operand)), typ(util::nullopt)
{  }

sizeof_::sizeof_(token op, bool, type::ptr typ)
    : expression(op.get_location()), op(op),
      operand(util::nullopt), typ(std::move(typ))
{  }

type::ptr sizeof_::_infer(semantics::scope& s)
{
    auto ty = (typ.has() ? typ.get()->copy() : operand.get()->infer(s)->copy());

    //we need to infer our type as well
    if(typ.has())
    {
        typ.get()->infer(s);
    }

    if(ty->is_function())
    {
        semantics::throw_error(loc, "Can't take the size of a function type!");
    }
    else if(ty->is_void())
    {
        semantics::throw_error(loc, "Can't take the size of type void!");
    }

    return util::make_unique<types::_integral>(tok::keyword_int);
}

llvm::Value* sizeof_::make_rvalue(irconstructer& irc, IRScope& irscope)
{
    llvm::Type* lltyp = nullptr;
    if(operand.has())
    {
        // sizeof does not evaluate the expression!!

        auto* str = dynamic_cast<expressions::string*>(operand.get().get());
        if(str)
        {
            return irc._builder.getInt32(str->length());
        }
        else
            lltyp = operand.get()->get_llvm_type(irc, irscope);
    }
    else if(typ.has())
        lltyp = typ.get()->generate_ir(irc, irscope);
    assert(lltyp);

    if(lltyp->isSized())
    {
        llvm::DataLayout dat(&irc.module);
        return irc._builder.getInt32(dat.getTypeAllocSize(lltyp));
    }
    else
        return irc._builder.getInt32(0);
}

void sizeof_::print() const
{
    std::cout << op.userdata();

    if(operand)
    {
        std::cout << " ";
        operand.get()->print_expr();
    }
    else
    {
        std::cout << "(";
        typ.get()->print();
        std::cout << ")";
    }
}

bool sizeof_::is_l_value() const noexcept
{
    return false;
}

}

}
