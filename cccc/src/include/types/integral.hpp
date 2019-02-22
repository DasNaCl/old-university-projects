#pragma once

#include <lex/token_kind.hpp>

#include <types/type.hpp>

namespace mhc4
{

namespace types
{

class _integral : public type
{
public:
    explicit _integral(tok::token_kind kind);
    virtual ~_integral();

    bool operator==(const type& typ) const override;

    type* get_any_unprepared_thunk() noexcept override;

    void print(std::size_t) const override;
    void print(std::string name, std::size_t) const override;

    void infer(semantics::scope& s) override;

    llvm::Type* generate_ir(irconstructer& irc, IRScope& irscope) override;

    bool is_void() const noexcept override;
    bool is_pointer() const noexcept override;
    bool is_integral() const noexcept override;
    bool is_function() const noexcept override;
    bool is_array() const noexcept override;
    bool is_struct() const noexcept override;
    bool is_thunk() const noexcept override;

    type::ptr copy() const override;
private:
    tok::token_kind kind;
};

}

}
