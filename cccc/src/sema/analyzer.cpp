#include <sema/analyzer.hpp>
#include <sema/scope.hpp>

namespace mhc4
{

namespace semantics
{

void analyze(const std::vector<statement::ptr>& ast)
{
    semantics::scope initial = scope::create();

    for(auto it = ast.cbegin(); it != ast.cend(); ++it)
    {
        const auto& node = *it;

        node->infer(initial);
    }
}

void throw_error(const source_location& loc, const std::string& err)
{
    throw std::invalid_argument(loc.filepath() + ":" +
                                std::to_string(loc.line()) + ":" +
                                std::to_string(loc.column()) + ": " +
                                "error: " + err);
}

void throw_error(token tok, const std::string& err)
{
    throw_error(tok.get_location(), err);
}

}

}
