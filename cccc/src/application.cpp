#include <application.hpp>
#include <file_loader.hpp>

#include <basic_parser.hpp>

#include <lex/lexer.hpp>
#include <lex/dummy_parser.hpp>

#include <statements/function.hpp>
#include <parse/parser.hpp>

#include <sema/analyzer.hpp>

#include <ir/irconstructer.hpp>
#include <ir/IRScope.hpp>

#include <util>

#include <iostream>

namespace mhc4
{

application::application(const cmd_info& info) noexcept
    : info(info)
{  }

int application::compile()
{
    const std::string path = info.file;

    file_loader my_file_loader(path);
    const char* content = static_cast<const char*>(my_file_loader);

    const bool dummy = info.tokenize;
    lexer lex(path, content, my_file_loader.len());
    basic_parser::ptr my_parser;
    if(dummy)
        my_parser = util::make_unique<dummy_parser>(lex);
    else
        my_parser = util::make_unique<parser>(lex);

    try
    {
        my_parser->parse();
        if(dummy)
            return EXIT_SUCCESS;

        auto& pars = *(static_cast<parser*>(my_parser.get()));
        const auto& ast = pars.ast();

        semantics::analyze(ast);
        if(info.print_ast)
        {
            for(auto it = ast.cbegin(); it != ast.cend(); ++it)
            {
                const auto& node = *it;
                node->print_statement(0);

                if(std::next(it) != ast.cend())
                {
                    std::cout << std::endl;
                }
                if(dynamic_cast<const statements::function*>(node.get()) != nullptr)
                {
                    std::cout << std::endl;
                }
            }
        }
        if(info.parse)
            return EXIT_SUCCESS;
        if(info.compile)
        {
            irconstructer::construct(path, ast, info.optimize);
            return EXIT_SUCCESS;
        }
    }
    catch(empty_parse_exception& ex)
    {
        std::cerr << path << ":1:1: error: " << ex.what() << std::endl;
    }
    catch(std::invalid_argument& ex)
    {
        std::cerr << ex.what() << std::endl;
    }
    catch(std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
    }
    return EXIT_FAILURE;
}

}
