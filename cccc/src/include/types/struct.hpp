#pragma once

#include <lex/token.hpp>

#include <types/type.hpp>

#include <sema/scope.hpp>

#include <unordered_set>
#include <memory>
#include <string>
#include <vector>
#include <deque>
#include <util>

namespace mhc4
{

namespace types
{

class _struct : public type
{
public:
    explicit _struct(token_opt id);
    virtual ~_struct();

    bool operator==(const type& typ) const override;

    type* get_any_unprepared_thunk() noexcept override;

    void print(std::size_t d) const override;
    void print(std::string name, std::size_t d) const override;

    void infer(semantics::scope& s) override;

    llvm::Type* generate_ir(irconstructer& ir, IRScope& irscope) override;

    bool is_void() const noexcept override;
    bool is_pointer() const noexcept override;
    bool is_integral() const noexcept override;
    bool is_array() const noexcept override;
    bool is_function() const noexcept override;
    bool is_struct() const noexcept override;
    bool is_thunk() const noexcept override;

    void insert(type::ptr typ, token_opt name);

    type::ptr copy() const override;

    bool has_name() const noexcept;
    const token& name() const;

    std::size_t indexof(const token& tok);

    std::deque<semantics::scope> inner_scope(semantics::scope& s);
private:
    token_opt id;
    std::vector<std::pair<type::ptr, token>> data;
    std::vector<std::pair<type::ptr, token>> stalled_data;

    std::string irname;
};

}

}
