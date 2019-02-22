#include <optimize/lattice_elem.hpp>

#include <cassert>

namespace mhc4
{

namespace pass
{

lattice_elem::lattice_elem()
    : data(nullptr, lattice_value::bottom)
{  }

bool lattice_elem::is_bot() const
{
    return lattice_value::bottom == value();
}

bool lattice_elem::is_constant() const
{
    return lattice_value::constant == value()
        || lattice_value::prop_constant == value();
}

bool lattice_elem::is_top() const
{
    return lattice_value::top == value();
}

bool lattice_elem::set_as_top()
{
    if(is_top())
        return false;
    data.second = lattice_value::top;
    return true;
}

bool lattice_elem::set_as_prop_constant(llvm::Constant* other)
{
    assert(is_bot());
    data.first = other;
    data.second = lattice_value::prop_constant;
    return true;
}

bool lattice_elem::set_as_constant(llvm::Constant* other)
{
    if(is_constant())
    {
        assert(other == constant());
        return false;
    }
    else if(is_bot())
    {
        assert(data.first = other);
        data.second = lattice_value::constant;
    }
    else
    {
        if(constant() == other)
            return false;
        data.second = lattice_value::top;
    }
    return true;
}

llvm::Constant* lattice_elem::constant() const
{
    assert(is_constant());
    return data.first;
}

lattice_value lattice_elem::value() const
{
    return data.second;
}

}

}
