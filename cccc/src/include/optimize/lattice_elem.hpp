#pragma once

#include <utility>

//forward declarations
namespace llvm
{
    class Constant;
}

namespace mhc4
{

namespace pass
{

enum class lattice_value : char
{
    bottom,
    constant,      //<- a real constant
    prop_constant, //<- a propagated constant
    top
};

class lattice_elem
{
public:
    explicit lattice_elem();

    bool is_bot() const;
    bool is_constant() const;
    bool is_top() const;

    bool set_as_top();
    bool set_as_constant(llvm::Constant* other);
    bool set_as_prop_constant(llvm::Constant* other);

    llvm::Constant* constant() const;
private:
    lattice_value value() const;
private:
    std::pair<llvm::Constant*, lattice_value> data;
};

}

}
