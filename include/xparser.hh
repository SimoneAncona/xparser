/**
 * @file xparser.hh
 * @author Simone Ancona
 * @brief A simple parser for C++
 * @version 1.0
 * @date 2023-07-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "jpp.hh"
#include "ast.hh"
#include "rel.hh"
#include <regex>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <stdexcept>
#include <regex>
#include <stack>

namespace Xpp
{
    struct Rule
    {
        std::string name;
        std::vector<RuleExpression> expressions;
    };

    struct TerminalRule
    {
        std::string name;
        std::string regex;
    };

    struct Token
    {
        TerminalRule from;
        size_t index;
        size_t column;
        size_t line;
        std::string value;
    };

    enum SyntaxErrorType
    {
        EXPECTED_TOKEN,
        UNEXPECTED_TOKEN
    };

    struct SyntaxError
    {
        SyntaxErrorType type;
        std::string message;
        size_t index;
        size_t column;
        size_t line;
    };

    bool token_compare(Token, Token);

    class Parser
    {
    private:
        Jpp::Json grammar;
        std::vector<Rule> rules;
        std::vector<TerminalRule> terminals = {{"integer", "[-|+]?\\d+"}, {"identifier", "[_a-zA-Z][_a-zA-Z0-9]*"}, {"real", "[+|-]?\\d+(\\.\\d+)?"}};
        std::stack<SyntaxError> error_stack;

        size_t index;

        void generate_from_json();
        void generate_terminal_rules(std::map<std::string, Jpp::Json>);
        void generate_rules(std::map<std::string, Jpp::Json>);
        std::vector<RuleExpression> parse_expressions(std::map<std::string, Jpp::Json>);
        std::string get_string_from_file(const std::ifstream &);
        std::vector<Token> tokenize(const std::string &);
        Xpp::AST parse(const std::vector<Token> &);
        std::vector<Token> get_tokens(const std::string &, TerminalRule);
        std::pair<size_t, size_t> get_column_line(std::string_view, long long);
        void analyze_rule(Xpp::AST &, const std::vector<Token> &, const Rule &);
        void analyze_expression(Xpp::AST &, const std::vector<Token> &, RuleExpression &);
        void analyze_alternative(Xpp::AST &, const std::vector<Token> &, const ExpressionElement &);
        void analyze_reference(Xpp::AST &, const std::vector<Token> &, const ExpressionElement &);
        void analyze_zero_or_one(Xpp::AST &, const std::vector<Token> &, const ExpressionElement &);
        void analyze_zero_or_more(Xpp::AST &, const std::vector<Token> &, const ExpressionElement &);
        void analyze_one_or_more(Xpp::AST &, const std::vector<Token> &, const ExpressionElement &);
        void analyze_constant(Xpp::AST &, const std::vector<Token> &, const ExpressionElement &);

    public:
        /**
         * @brief Construct a new Parser object
         *
         */
        Parser() = default;

        /**
         * @brief Construct a new Parser object specifying the grammar with a JSON object
         *
         */
        Parser(const Jpp::Json &);

        /**
         * @brief Construct a new Parser object specifying the grammar with a JSON string
         *
         */
        Parser(const std::string &);

        /**
         * @brief Construct a new Parser object specifying the input file stream
         *
         */
        Parser(const std::ifstream &);

        /**
         * @brief Destroy the Parser object
         *
         */
        ~Parser() = default;

        /**
         * @brief Get the ast object
         *
         * @return AST
         */
        AST generate_ast(const std::string &);

        /**
         * @brief Get the error stack
         *
         * @return std::stack<SyntaxError>&
         */
        std::stack<SyntaxError> &get_error_stack() noexcept;

        /**
         * @brief Get the last error
         *
         * @return SyntaxError
         */
        SyntaxError get_last_error();
    };
};