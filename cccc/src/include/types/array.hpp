#pragma once

#include <lex/token_kind.hpp>

#include <types/type.hpp>

namespace mhc4
{

namespace types
{

class array : public type
{
public:
    explicit array(type::ptr of, std::size_t size = -1);
    virtual ~array();

    bool operator==(const type& typ) const override;

    type* get_any_unprepared_thunk() noexcept override;

    type::ptr base() const;

    void print(std::size_t) const override;
    void print(std::string name, std::size_t) const override;

    void print_base(std::size_t) const;
    void print_array_part() const;

    void infer(semantics::scope& s) override;
    llvm::Type* generate_ir(irconstructer&, IRScope&) override
    { return nullptr; }

    bool is_void() const noexcept override;
    bool is_pointer() const noexcept override;
    bool is_integral() const noexcept override;
    bool is_function() const noexcept override;
    bool is_array() const noexcept override;
    bool is_struct() const noexcept override;
    bool is_thunk() const noexcept override;

    type::ptr copy() const override;
private:
    type::ptr of;
    std::size_t size;
};

}

}
