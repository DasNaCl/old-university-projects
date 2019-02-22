#include <types/struct.hpp>
#include <types/function.hpp>
#include <types/integral.hpp>
#include <types/thunk.hpp>
#include <types/ptr.hpp>

#include <sema/analyzer.hpp>
#include <sema/scope.hpp>

#include <ir/irconstructer.hpp>

#include <iostream>

namespace mhc4
{

namespace types
{

static std::string dummy_loc("");

_struct::_struct(token_opt id) : id(id), data(), stalled_data()
{  }

_struct::~_struct()
{  }

bool _struct::operator==(const type& typ) const
{
    auto* r = (typ.is_thunk()
               ?
               dynamic_cast<const types::_struct*>(static_cast<const thunk*>(&typ)->get().get())
               :
               dynamic_cast<const types::_struct*>(&typ));
    if(!r || typeid(*r) != typeid(*this))
        return false;
    const _struct& strct = *util::thunk_cast<const _struct*>(&typ);

    bool differ = data.size() != strct.data.size();
    if(!differ)
    {
        for(std::size_t i = 0; i < data.size(); ++i)
        {
            if(!(*data[i].first == *strct.data[i].first
              && data[i].second.userdata() == strct.data[i].second.userdata()))
            {
                differ = true;
                break;
            }
        }
    }
    bool oneiszero = data.empty() || strct.data.empty();

    return (oneiszero && id.has() && strct.id.has() && id.get().userdata() == strct.id.get().userdata())
        || (!oneiszero && id.has() && strct.id.has() && id.get().userdata() == strct.id.get().userdata() && !differ);
}

type* _struct::get_any_unprepared_thunk() noexcept
{
    return nullptr;
}

void _struct::print(std::size_t depth) const
{
    std::cout << "struct ";
    if(id.has())
        std::cout << id.get().userdata();
    if(!data.empty())
    {
        if(id.has())
        {
            std::cout << "\n";
            for(std::size_t d = depth; d > 0; --d)
                std::cout << "\t";
        }
        std::cout << "{\n";

        for(auto it = data.begin(); it != data.end(); ++it)
        {
            for(std::size_t d = depth + 1; d > 0; --d)
                std::cout << "\t";
            if(it->second.userdata().empty())
                it->first->print(depth + 1);
            else
                it->first->print(it->second.userdata(), depth + 1);
            std::cout << ";\n";
        }
       for(std::size_t d = depth; d > 0; --d)
            std::cout << "\t";
        std::cout << "}";
    }
}

void _struct::print(std::string name, std::size_t depth) const
{
    print(depth);
    std::cout << " " << name;
}

llvm::Type* _struct::generate_ir(irconstructer& irc, IRScope& irscope)
{
    // no anonymous structs
    assert(id.has() && !irname.empty());

    auto* mightret = irscope.find_struct(irname);
    if(mightret && mightret->getNumElements() > 0)
        return mightret;

    llvm::StructType* struct_t = (mightret
                                  ? mightret
                                  :
                                  llvm::StructType::create(irc.ctx, irname));
    if(!mightret)
    {
        irscope.insert_struct(irname, struct_t);
    }

    // stalled_data might be empty if we are a forward declaration
    std::vector<llvm::Type*> member_ts;
    for(auto& x : data)
    {
        auto* v = x.first->generate_ir(irc, irscope);

        if(!x.second.userdata().empty() && v)
            member_ts.push_back(v);
    }
    if(!member_ts.empty())
        struct_t->setBody(member_ts);
    return struct_t;
}

void _struct::infer(semantics::scope& s)
{
    if(id.has())
    {
        // only add when struct defined?
        s.insert_struct(id.get(), copy(), !data.empty());
    }

    auto z = s.inherit();
    for(const auto& pair : data)
    {
        const auto& typ = pair.first;
        const auto& id = pair.second;

        if(typ->is_function())
        {
            semantics::throw_error(id.get_location(), "Function inside struct");
        }

        // structs do not encapsulate other structs, thus use s instead of z
        if(typ->is_struct())
            typ->infer(s);
        else
            typ->infer(z);
        if(id.get_kind() != tok::end_of_file)
        {
            // only add if you are not a dummy (= we have a name)
            z.insert_decl(typ, id);
        }
    }
    if(!data.empty())
    {
        for(auto& p : data)
        {
            stalled_data.emplace_back(std::make_pair(p.first->copy(), p.second));

            stalled_data.back().first->infer(z);
        }
    }

    // now set ir name. (might get set again if we are a member access)
    //! first, lookup if we are present and data.empty()
    auto pdecl = s.find_struct_as_decl(this);
    // we inserted ourselves...
    assert(pdecl);
    auto& decl = *pdecl;
    if(decl.irname.empty())
    {
        decl.irname = irname = "struct" + std::to_string(s.struct_counter[decl.name]) + "." + id.get().userdata();

        if(!data.empty())
            ++s.struct_counter[decl.name];
    }
    else
    {
        irname = decl.irname;
    }
}

bool _struct::is_void() const noexcept
{
    return false;
}

bool _struct::is_pointer() const noexcept
{
    return false;
}

bool _struct::is_integral() const noexcept
{
    return false;
}

bool _struct::is_function() const noexcept
{
    return false;
}

bool _struct::is_array() const noexcept
{
    return false;
}

bool _struct::is_struct() const noexcept
{
    return true;
}

bool _struct::is_thunk() const noexcept
{
    return false;
}

void _struct::insert(type::ptr typ, token_opt name)
{
    token n = name.has()
        ?
        name.get()
        :
        token(source_location(dummy_loc, -1, -1), tok::end_of_file);
    data.emplace_back(std::make_pair(typ->copy(), n));
    stalled_data.emplace_back(std::make_pair(typ->copy(), n));
}

type::ptr _struct::copy() const
{
    auto str = (id.has() ? ::util::make_unique<_struct>(id.get()) : ::util::make_unique<_struct>(::util::nullopt));
    for(const auto& p : data)
        str->data.emplace_back(std::make_pair(p.first->copy(), p.second));
    for(const auto& p : stalled_data)
        str->stalled_data.emplace_back(std::make_pair(p.first->copy(), p.second));

    str->set_location(this->location);
    str->irname = irname;

    return str;
}

bool _struct::has_name() const noexcept
{
    return id.has();
}

const token& _struct::name() const
{
    return id.get();
}

std::size_t _struct::indexof(const token& tok)
{
    for(std::size_t i = 0U; i < data.size(); ++i)
    {
        if(data[i].second.userdata() == tok.userdata())
            return i;
    }
    for(std::size_t i = 0U; i < stalled_data.size(); ++i)
    {
        if(stalled_data[i].second.userdata() == tok.userdata())
            return i;
    }
    return static_cast<std::size_t>(-1);
}

std::deque<semantics::scope> _struct::inner_scope(semantics::scope& s)
{
    // if we are empty, we should look us up in the scope
    if(data.empty())
    {
        // we are declared like `struct S myS;`, so look us up
        auto def = s.find_struct(this)->copy();
        assert(def->is_struct());

        stalled_data = std::move(static_cast<_struct*>(def.get())->data);
    }
    else
    {
        stalled_data.clear();
        for(auto& p : data)
            stalled_data.emplace_back(std::make_pair(p.first->copy(), p.second));
    }
    // we have a gurantee that we are not shadowed, since we only get called by member_access! :)
    // (however, quite dirty, to be honest... Sorry Dijkstra, yet again)


    std::deque<semantics::scope> tmp;
    semantics::scope::create_with_structs(s, tmp);
    auto& z = tmp.back();
    for(const auto& pair : stalled_data)
    {
        const auto& typ = pair.first;
        const auto& id = pair.second;
        if(id.get_kind() != tok::end_of_file)
        {
            // only add if you are not a dummy (= we have a name)
            z.insert_decl(typ->copy(), id);
        }
    }
    return tmp;
}

}

}
