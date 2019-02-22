#include <expressions/arithmetic_binary.hpp>

#include <ir/irconstructer.hpp>

#include <sema/analyzer.hpp>

#include <types/integral.hpp>
#include <types/array.hpp>
#include <types/thunk.hpp>
#include <types/ptr.hpp>

#include <cassert>

namespace mhc4
{

namespace expressions
{

arithmetic::arithmetic(token tok, expression::ptr left, expression::ptr right)
    : binary_printer(tok, std::move(left), std::move(right))
{  }

type::ptr arithmetic::_infer(semantics::scope& s)
{
    auto ltyp = lhs->infer(s);
    auto rtyp = rhs->infer(s);

    const bool arithmetic = ltyp->is_arithmetic() && rtyp->is_arithmetic();
    const bool pointer = ltyp->is_pointer() && rtyp->is_pointer();
    const bool array = ltyp->is_array() && rtyp->is_array();

    type::ptr result(nullptr);
    switch(op.get_kind())
    {
    case tok::star:
    case tok::slash:
    case tok::percent:
    {
        if(!arithmetic)
        {
            semantics::throw_error(op, "Operands of \"" + op.userdata() + "\" need to have arithmetic types.");
        }
        result = ltyp->copy();
    } break;

    case tok::plus:
    {
        const bool right_is_ptrable = rtyp->is_pointer() || rtyp->is_array();
        const bool left_is_ptrable = ltyp->is_pointer() || ltyp->is_array();
        if(arithmetic || (ltyp->is_arithmetic() && right_is_ptrable)
        || (left_is_ptrable && rtyp->is_arithmetic()))
        {
            // ok
            result = (ltyp->is_pointer() ? ltyp : rtyp)->copy();
        }
        else
        {
            semantics::throw_error(op, "Operands of \"+\" need to be arithmetic types or one may qualify as pointer type.");
        }
    } break;

    case tok::minus:
    {
        if(arithmetic || ((pointer || array) && *ltyp == *rtyp))
        {
            // integer promotion in case of `arithmetic == true`
            result = util::make_unique<types::_integral>(tok::keyword_int);
        }
        else if((ltyp->is_pointer() || ltyp->is_array()) && rtyp->is_arithmetic())
        {
            const auto* arr = dynamic_cast<const types::array*>(ltyp.get());
            if(arr)
            {
                // degenerate to a pointer
                result = util::make_unique<types::ptr>(arr->base());
            }
            else
                result = ltyp->copy();
        }
        else
        {
            semantics::throw_error(op, "Operands of \"-\" need to be arithmetic types or compatible pointer types or the left may be a pointer and the right an integer type.");
        }
    } break;

    default: break;
    }
    assert(result);
    return result;
}

llvm::Value* arithmetic::make_rvalue(irconstructer& irc, IRScope& irscope)
{
    llvm::Value* l = lhs->make_rvalue(irc, irscope);
    llvm::Value* r = rhs->make_rvalue(irc, irscope);

    llvm::Value* result = nullptr;
    switch(op.get_kind())
    {
    case tok::plus:
    {
        if(l->getType()->isPointerTy() || r->getType()->isPointerTy())
        {
            auto* ptr = l->getType()->isPointerTy() ? l : r;
            auto* oth = l->getType()->isPointerTy() ? r : l;

            auto* i64 = irc.integer_conversion(oth, irc._builder.getInt64Ty());

            std::vector<llvm::Value*> args;
            args.push_back(i64);

            result = irc._builder.CreateGEP(ptr, args, "addgep");
        }
        else
        {
            //integer promotion....
            l = irc.integer_promotion(l);
            r = irc.integer_promotion(r);
            result = irc._builder.CreateAdd(l, r, "addtmp");
        }
    } break;
    case tok::minus:
    {
        if(l->getType()->isPointerTy() && !r->getType()->isPointerTy())
        {
            auto* i64 = irc.integer_conversion(r, irc._builder.getInt64Ty());

            std::vector<llvm::Value*> args;
            args.push_back(i64);

            result = irc._builder.CreateGEP(l, args, "subgep");
        }
        else if(l->getType()->isPointerTy() && r->getType()->isPointerTy())
        {
            auto* ltyp = static_cast<llvm::PointerType*>(l->getType());
            auto* lltyp = ltyp->getElementType();
            l = irc._builder.CreatePtrToInt(l, irc._builder.getInt32Ty());
            r = irc._builder.CreatePtrToInt(r, irc._builder.getInt32Ty());
            auto* res = irc._builder.CreateSub(l, r, "subtmp");
            llvm::DataLayout dat(&irc.module);
            result = irc._builder.CreateSDiv(res, irc._builder.getInt32(dat.getTypeAllocSize(lltyp)), "divtmpptr");
        }
        else
        {
            l = irc.integer_promotion(l);
            r = irc.integer_promotion(r);
            result = irc._builder.CreateSub(l, r, "subtmp");
        }
    } break;
    case tok::star:
    {
        l = irc.integer_promotion(l);
        r = irc.integer_promotion(r);
        result = irc._builder.CreateMul(l, r, "multmp");
    } break;
    case tok::slash:
    {
        l = irc.integer_promotion(l);
        r = irc.integer_promotion(r);
        result = irc._builder.CreateSDiv(l, r, "divtmp");
    } break;
    default: break;
    }

    assert(result);
    return result;
}

}

}
