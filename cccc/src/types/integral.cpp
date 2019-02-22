#include <types/integral.hpp>
#include <types/thunk.hpp>

#include <ir/irconstructer.hpp>

#include <iostream>
#include <cassert>

namespace mhc4
{

namespace types
{

_integral::_integral(tok::token_kind kind)
    : kind(kind)
{  }

_integral::~_integral()
{  }

bool _integral::operator==(const type& typ) const
{
    auto* r = (typ.is_thunk()
               ?
               dynamic_cast<const types::_integral*>(static_cast<const thunk*>(&typ)->get().get())
               :
               dynamic_cast<const types::_integral*>(&typ));
    if(!r || typeid(*r) != typeid(*this))
        return false;
    const _integral& _int = *util::thunk_cast<const _integral*>(&typ);
    return kind == _int.kind;
}

type* _integral::get_any_unprepared_thunk() noexcept
{
    return nullptr;
}

void _integral::print(std::size_t) const
{
    switch(kind)
    {
    case tok::keyword_void: std::cout << "void"; break;
    case tok::keyword_int:  std::cout << "int";  break;
    case tok::keyword_char: std::cout << "char"; break;
    default: assert(false);
    }
}

void _integral::print(std::string name, std::size_t ) const
{
    switch(kind)
    {
    case tok::keyword_void: std::cout << "void " << name; break;
    case tok::keyword_int:  std::cout << "int "  << name; break;
    case tok::keyword_char: std::cout << "char " << name; break;
    default: assert(false);
    }
}

void _integral::infer(semantics::scope& )
{
    // TODO: ?
}

llvm::Type* _integral::generate_ir(irconstructer& irc, IRScope& irscope)
{
    switch(kind)
    {
    case tok::keyword_void: return irc._builder.getVoidTy();
    case tok::keyword_int:  return irc._builder.getInt32Ty();
    case tok::keyword_char: return irc._builder.getInt8Ty();
    default: assert(false); return nullptr;
    }
}

bool _integral::is_void() const noexcept
{
    return kind == tok::keyword_void;
}

bool _integral::is_pointer() const noexcept
{
   return false;
}

bool _integral::is_integral() const noexcept
{
   return true;
}

bool _integral::is_function() const noexcept
{
    return false;
}

bool _integral::is_array() const noexcept
{
    return false;
}

bool _integral::is_struct() const noexcept
{
    return false;
}

bool _integral::is_thunk() const noexcept
{
    return false;
}

type::ptr _integral::copy() const
{
    auto p = ::util::make_unique<_integral>(kind);
    p->set_location(this->location);
    return p;
}

}

}
