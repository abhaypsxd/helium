#pragma once

#include <vector>
#include <optional>
#include <string>
#include <variant>

#include "tokenization.hpp"

namespace Node {
    struct ExprIntLit { Token int_lit; };
    struct ExprIdent  { Token ident;   };
    struct Expr {
        std::variant<ExprIntLit, ExprIdent> var;
    };
    struct StmtExit    { Expr expr;     };
    struct StmtLet     { Token ident; Expr expr; };

    struct Stmt {
        std::variant<StmtExit, StmtLet> var;
    };
    struct Prog {
        std::vector<Stmt> stmts;
    };
}




class Parser{
private:
    const std::vector<Token> m_tokens;
    size_t m_index = 0;

    [[nodiscard]] inline std::optional<Token>peek(int ahead = 0) const {
        if(m_index+ahead>=m_tokens.size()){
            return {};
        }
        else {
            return m_tokens.at(m_index+ahead);
        }
    }

    inline Token consume(){
        return m_tokens.at(m_index++);
    }


public:
    inline explicit Parser(std::vector<Token> tokens):m_tokens(std::move(tokens)){

    }

    std::optional<Node::Expr>parse_expr(){
        if(peek().has_value()&&peek().value().type==TokenType::int_lit){ 
            return Node::Expr{.var = Node::ExprIntLit{.int_lit = consume()}};
        }
        else if(peek().has_value()&&peek().value().type==TokenType::ident){
            return Node::Expr{.var = Node::ExprIdent{.ident = consume()}};
        }
        else return {};

    }



    std::optional<Node::Stmt> parse_stmt(){
        if (peek()->type == TokenType::exit &&
            peek(1).has_value() && peek(1)->type == TokenType::open_paren) {
            
            consume(); // consume 'exit'
            consume(); // consume '('
            Node::StmtExit stmt_exit;

            if (auto node_expr = parse_expr()) {
                stmt_exit = { .expr = node_expr.value() };
            } else {
                std::cerr << "Invalid expression." << std::endl;
                exit(EXIT_FAILURE);
            }

            if (peek().has_value() && peek()->type == TokenType::closed_paren) {
                consume(); // consume ')'
            } else {
                std::cerr << "Expected closing parenthesis." << std::endl;
                exit(EXIT_FAILURE);
            }

            if (peek().has_value() && peek()->type == TokenType::semi) {
                consume(); // consume ';'
            } else {
                std::cerr << "Expected semicolon." << std::endl;
                exit(EXIT_FAILURE);
            }
            return Node::Stmt{.var = stmt_exit};
        }
        else if(
            peek().has_value()&&peek().value().type == TokenType::let
            &&peek(1).has_value()&&peek(1).value().type == TokenType::ident&&peek(2).has_value()
            &&peek(2).value().type == TokenType::eq){
            consume();
            auto stmt_let = Node::StmtLet{.ident = consume()};
            consume();
            if(auto expr = parse_expr()){
                stmt_let.expr = expr.value();
            }
            else{
                std::cerr<<"Invalid Expression"<<std::endl;
                exit(EXIT_FAILURE);
            }
            if(peek().has_value()&&peek().value().type==TokenType::semi){
                consume();
            }
            else{
                std::cerr<<"Expected `;`"<<std::endl;
                exit(EXIT_FAILURE);
            }
            return Node::Stmt{.var = stmt_let};

        }
        else{
            return {};
        }
    }
    std::optional<Node::Prog> parse_prog(){
        Node::Prog prog;
        while(peek().has_value()){
            if(auto stmt = parse_stmt()){
                prog.stmts.push_back(stmt.value());
            }
            else{
                std::cerr<<"Invalid Statement"<<std::endl;
                exit(EXIT_FAILURE);
            }
        }
        return prog;
    }






};