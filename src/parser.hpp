#pragma once

#include <vector>
#include <optional>
#include <string>
#include <variant>

#include "tokenization.hpp"
#include "arena.hpp"

namespace Node {
    struct ExprIntLit { Token int_lit; };
    struct ExprIdent  { Token ident;   };
    struct Expr;
    
    struct BinExprAdd{
        Expr* lhs;
        Expr* rhs;
    };
    struct BinExprMulti{
        Expr* lhs;
        Expr* rhs;
    };
    struct BinExpr{
        std::variant<BinExprAdd*,BinExprMulti*>var;
    };
    struct Expr {
        std::variant<ExprIntLit*, ExprIdent*, BinExpr*> var;
    };


    struct StmtExit    { Expr* expr;     };
    struct StmtLet     { Token ident; Expr* expr; };

    struct Stmt {
        std::variant<StmtExit*, StmtLet*> var;
    };
    struct Prog {
        std::vector<Stmt*> stmts;
    };
}




class Parser{
private:
    const std::vector<Token> m_tokens;
    size_t m_index = 0;
    ArenaAllocator m_allocator;

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
    inline explicit Parser(std::vector<Token> tokens):m_tokens(std::move(tokens)), m_allocator(1024*1024*4){// 4 Megabytes.

    }

    std::optional<Node::Expr*>parse_expr(){
        if(peek().has_value()&&peek().value().type==TokenType::int_lit){ 
            auto node_expr_int_lit = m_allocator.alloc<Node::ExprIntLit>();
            node_expr_int_lit->int_lit = consume();
            auto node_expr = m_allocator.alloc<Node::Expr>();
            node_expr->var = node_expr_int_lit;
            return node_expr;
        }
        else if(peek().has_value()&&peek().value().type==TokenType::ident){
            auto node_expr_ident = m_allocator.alloc<Node::ExprIdent>();
            node_expr_ident->ident = consume();
            auto node_expr = m_allocator.alloc<Node::Expr>();
            node_expr->var = node_expr_ident;
            return node_expr;
        }
        else return {};

    }



    std::optional<Node::Stmt*> parse_stmt() {
    auto token0 = peek();
    auto token1 = peek(1);
    auto token2 = peek(2);

    // Parse: exit(<expr>);
    if (token0 && token0->type == TokenType::exit &&
        token1 && token1->type == TokenType::open_paren) {

        consume(); // 'exit'
        consume(); // '('

        auto stmt_exit = m_allocator.alloc<Node::StmtExit>();

        if (auto expr = parse_expr()) {
            stmt_exit->expr = expr.value();
        } else {
            std::cerr << "Invalid expression inside exit()." << std::endl;
            exit(EXIT_FAILURE);
        }

        if (auto token = peek(); token && token->type == TokenType::closed_paren) {
            consume(); // ')'
        } else {
            std::cerr << "Expected ')' after expression." << std::endl;
            exit(EXIT_FAILURE);
        }

        if (auto token = peek(); token && token->type == TokenType::semi) {
            consume(); // ';'
        } else {
            std::cerr << "Expected ';' after exit statement." << std::endl;
            exit(EXIT_FAILURE);
        }

        auto stmt = m_allocator.alloc<Node::Stmt>();
        stmt->var = stmt_exit;
        return stmt;
    }

    // Parse: let <ident> = <expr>;
    if (token0 && token0->type == TokenType::let &&
        token1 && token1->type == TokenType::ident &&
        token2 && token2->type == TokenType::eq) {

        consume(); // 'let'

        auto stmt_let = m_allocator.alloc<Node::StmtLet>();
        stmt_let->ident = consume(); // ident
        consume(); // '='

        if (auto expr = parse_expr()) {
            stmt_let->expr = expr.value();
        } else {
            std::cerr << "Invalid expression in let statement." << std::endl;
            exit(EXIT_FAILURE);
        }

        if (auto token = peek(); token && token->type == TokenType::semi) {
            consume(); // ';'
        } else {
            std::cerr << "Expected ';' after let statement." << std::endl;
            exit(EXIT_FAILURE);
        }

        auto stmt = m_allocator.alloc<Node::Stmt>();
        stmt->var = stmt_let;
        return stmt;
    }

    return {};
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