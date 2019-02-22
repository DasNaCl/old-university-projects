#ifdef __WE_TEST_NOW_WITH_GOOGLETEST__

#pragma once

#include <lex/token.hpp>

namespace mhc4
{

class test_suite
{
public:
    test_suite(const std::string& f_path);

    std::vector<token> lex_all(const char* to_lex, size_t len) const;
    std::vector<token> lex_all(const std::string& to_lex) const;

    void parse(const char* to_parse, std::size_t len) const;
    void parse_body(const char* to_parse, std::size_t len) const;

    void parse(const std::string& str) const;
    void parse_body(const std::string& str) const;

    void parse_file(const std::string& path) const;

    void analyze(const std::string& to_analyze) const;
    void analyze_body(const std::string& to_analyze) const;

    bool expect_err_loc(std::exception& e, int row, int colomn, bool in_body=false) const;

    token get_string_token(const std::string& str, int row, int colomn) const;
    token get_identifier_token(const std::string& str, int row, int colomn) const;
    token get_charconst_token(const std::string& str, int row, int colomn) const;
    token get_numeric_token(const std::string& str, int row, int colomn) const;
    token get_punctuator_token(const std::string& str, int row, int colomn) const;
    token get_keyword_token(const std::string& str, int row, int colomn) const;

private:
    std::string f_path;
};

}

#endif
