#include <statements/empty.hpp>

namespace mhc4
{

namespace statements
{

empty::empty(const source_location& loc)
    : statement_semicolon_printer(loc)
{  }

void empty::print(std::size_t) const
{

}

void empty::infer(semantics::scope&) const
{

}

void empty::generate_ir(irconstructer&, IRScope&)
{

}

}

}
