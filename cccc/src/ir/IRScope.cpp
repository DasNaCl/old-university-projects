#include <ir/IRScope.hpp>

namespace mhc4
{

IRScope::IRScope(IRScope* parent)
    : data(), type_data(), struct_data(), function_data(),
      while_headBB(nullptr), while_endBB(nullptr), parent(parent)
{  }

IRScope IRScope::create()
{
    return IRScope(nullptr);
}

IRScope IRScope::inherit()
{
    return IRScope(this);
}

bool IRScope::has_val(const std::string& name)
{
    auto it = data.find(name);
    if(it != data.end())
        return true;
    if(!parent)
        return false;
    return parent->has_val(name);
}

void IRScope::insert(const std::string& name, llvm::Value* val)
{
    auto it = data.find(name);

    // if this fails, we have a broken sema
    assert(it == data.end());

    data.insert(std::make_pair(name, val));
}

llvm::Value* IRScope::find(const std::string& name)
{
    auto it = data.find(name);
    if(it == data.end())
    {
        // we must not miss this variable, due to our semantic analysis
        assert(parent);
        return parent->find(name);
    }
    return it->second;
}

void IRScope::insert_type(const std::string& name, llvm::Type* val)
{
    auto it = type_data.find(name);

    // if this fails, we have a broken sema
    assert(it == type_data.end());

    type_data.insert(std::make_pair(name, val));
}

llvm::Type* IRScope::find_type(const std::string& name)
{
    auto it = type_data.find(name);
    if(it == type_data.end())
    {
        // we must not miss this type, due to our semantic analysis
        assert(parent);
        return parent->find_type(name);
    }
    return it->second;
}

void IRScope::insert_struct(const std::string& name, llvm::StructType* struct_t)
{
    auto it = struct_data.find(name);

    // if this fails, we have a broken sema
    assert(it == struct_data.end());

    struct_data.insert(std::make_pair(name, struct_t));
}

llvm::StructType* IRScope::find_struct(const std::string& name)
{
    auto it = struct_data.find(name);
    assert(!name.empty());

    if(it == struct_data.end())
    {
        // we want soft error here
        if(parent)
            return parent->find_struct(name);
        else
            return nullptr;
    }
    return it->second;
}

void IRScope::insert_function(const std::string& name, llvm::Function* func)
{
    // we need to be external !!
    assert(!parent);

    auto it = function_data.find(name);

    // if this fails, we have a broken sema
    assert(it == function_data.end());

    function_data.insert(std::make_pair(name, func));
}

llvm::Function* IRScope::find_function(const std::string& name)
{
    if(parent)
        return parent->find_function(name);
    auto it = function_data.find(name);

    // we are the topmost scope...
    if(it == function_data.end())
        return nullptr;

    return it->second;
}

void IRScope::insert_label(const std::string& name, llvm::BasicBlock* val)
{
    if(parent && parent->parent)
        parent->insert_label(name, val);

    auto it = label_data.find(name);

    // if this fails, we have a broken sema
    assert(it == label_data.end());

    label_data.insert(std::make_pair(name, val));
}

llvm::BasicBlock* IRScope::find_label(const std::string& name)
{
    if(parent && parent->parent)
        return parent->find_label(name);
    auto it = label_data.find(name);
    if(it == label_data.end())
    {
        if(parent)
            return parent->find_label(name);
        else
            return nullptr;
    }
    return it->second;
}

void IRScope::set_while_headBB(llvm::BasicBlock* head)
{
    while_headBB = head;
}

void IRScope::set_while_endBB(llvm::BasicBlock* end)
{
    while_endBB = end;
}

llvm::BasicBlock* IRScope::find_while_headBB()
{
    if(while_headBB)
        return while_headBB;
    assert(parent);
    return parent->find_while_headBB();
}

llvm::BasicBlock* IRScope::find_while_endBB()
{
    if(while_endBB)
        return while_endBB;
    assert(parent);
    return parent->find_while_endBB();
}

IRScope& IRScope::highest()
{
    if(parent)
        return parent->highest();
    return *this;
}

}
