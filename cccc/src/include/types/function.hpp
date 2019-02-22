#pragma once

#include <lex/token_kind.hpp>

#include <types/type.hpp>

#include <vector>

namespace mhc4
{

namespace types
{

class function : public type
{
public:
    explicit function(type::ptr return_type,
                      std::vector<type::ptr> param_types);
    virtual ~function();

    bool operator==(const type& typ) const override;

    type* get_any_unprepared_thunk() noexcept override;

    void print(std::size_t) const override;
    void print(std::string name, std::size_t) const override;
    void print(std::string name, std::vector<std::string> args, std::size_t) const;

    void infer(semantics::scope& s) override;

    llvm::Type* generate_ir(irconstructer& ir, IRScope& irscope) override;

    const type::ptr& parameter_type(std::size_t index) const;
    const type::ptr& get_return_type() const;

    std::size_t parameter_count() const;

    void print_return_type(std::size_t depth = 0) const;
    void print_parameter_type(std::size_t index, std::size_t depth = 0) const;
    void print_parameter_type(std::size_t index, const std::string& name, std::size_t depth = 0) const;
    void print_parameters(std::size_t) const;

    bool returns_void() const noexcept;

    bool is_void() const noexcept override;
    bool is_pointer() const noexcept override;
    bool is_integral() const noexcept override;
    bool is_function() const noexcept override;
    bool is_array() const noexcept override;
    bool is_struct() const noexcept override;
    bool is_thunk() const noexcept override;

    type::ptr copy() const override;
private:
    type::ptr return_type;
    std::vector<type::ptr> param_types;
};

}

}
