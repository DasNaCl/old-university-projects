#pragma once

#include <statements/statement.hpp>

#include <lex/token.hpp>

#include <stdexcept>
#include <vector>
#include <string>

namespace mhc4
{

namespace semantics
{

class broken_semantic_exception : public std::invalid_argument
{
public:
    broken_semantic_exception(const source_location& loc, const std::string& message)
        : invalid_argument((loc.filepath() + ":" +
                            std::to_string(loc.line()) + ":" +
                            std::to_string(loc.column()) + ": error: " +
                            message).c_str())
    {  }
};

void analyze(const std::vector<statement::ptr>& ast);
void throw_error(token tok, const std::string& err);
void throw_error(const source_location& tok, const std::string& err);

}

}
