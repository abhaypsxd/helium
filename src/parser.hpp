#pragma once

#include <vector>
#include <optional>
#include <string>
#include <variant>
#include <unordered_map>

#include "tokenization.hpp"
#include "arena.hpp"

namespace Node {
    struct TermIntLit { Token int_lit; };
    struct TermIdent  { Token ident;   };
    struct Expr;
    
    struct BinExprAdd{
        Expr* lhs;
        Expr* rhs;
    };
    struct BinExprMulti{
        Expr* lhs;
        Expr* rhs;
    };
    struct BinExprSub{
        Expr* lhs;
        Expr* rhs;
    };
    struct BinExprDiv{
        Expr* lhs;
        Expr* rhs;
    };
    struct BinExprMod{
        Expr* lhs;
        Expr* rhs;
    };
    
    struct BinExpr{
        std::variant<BinExprAdd*, BinExprSub*, BinExprMulti*, BinExprDiv*, BinExprMod*>var;
    };

    struct Term{
        std::variant<TermIntLit*, TermIdent*>var;
    };

    struct Expr {
        std::variant<Term*,  BinExpr*> var;
    };


    struct StmtExit    { Expr* expr;     };
    struct StmtLet     { Token ident; Expr* expr; };
    struct StmtPrint   { Expr* expr;};

    struct Stmt {
        std::variant<StmtExit*, StmtLet*, StmtPrint*> var;
    };
    struct Prog {
        std::vector<Stmt*> stmts;
    };
}


enum class Assoc {Left, Right};
struct BinOpInfo{
    int precedence;
    Assoc assoc;
};

const std::unordered_map<TokenType, BinOpInfo>binop_info={

    {TokenType::plus, {100, Assoc::Left}},
    {TokenType::minus, {100, Assoc::Left}},
    {TokenType::multi, {200, Assoc::Left}},
    {TokenType::div, {200, Assoc::Left}},
    {TokenType::mod, {200, Assoc::Left}},

};


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

    inline std::optional<Token> try_consume(TokenType type){
        if(peek().has_value()&&peek().value().type==type){
            return consume();
        }
        else{
            return {};
        }
    }

    inline Token try_consume(TokenType type, const std::string& err_msg){
        if(peek().has_value()&&peek().value().type==type){
            return consume();
        }
        else{
            std::cerr<<err_msg<<std::endl;
            exit(EXIT_FAILURE);
        }
    }



public:
    inline explicit Parser(std::vector<Token> tokens):m_tokens(std::move(tokens)), m_allocator(1024*1024*4){// 4 Megabytes.

    }

    std::optional<Node::BinExpr*> parse_bin_expr(){
        if(auto lhs = parse_expr()){
            
        }
        else{
            return {};
        }
    }

    std::optional<Node::Term*>parse_term(){
        
        if(auto int_lit = try_consume(TokenType::int_lit)){ 
            auto node_term_int_lit = m_allocator.alloc<Node::TermIntLit>();
            node_term_int_lit->int_lit = int_lit.value();
            auto node_term = m_allocator.alloc<Node::Term>();
            node_term->var = node_term_int_lit;
            return node_term;
        }
        else if(auto ident = try_consume(TokenType::ident)){
            auto node_term_ident = m_allocator.alloc<Node::TermIdent>();
            node_term_ident->ident = ident.value();
            auto node_term = m_allocator.alloc<Node::Term>();
            node_term->var = node_term_ident;
            return node_term;
        }
        else{
            return {};
        }
    }

    std::optional<Node::Expr*> parse_expr_prec(int min_prec = 0) {
        std::optional<Node::Expr*> lhs_expr;

        // Case 1: Parentheses
        if (try_consume(TokenType::open_paren)) {
            lhs_expr = parse_expr_prec(0);
            if (!lhs_expr) {
                std::cerr << "Expected expression inside parentheses\n";
                exit(EXIT_FAILURE);
            }
            try_consume(TokenType::closed_paren, "Expected ')' after expression.");
        }
        // Case 2: A regular term (int literal or identifier)
        else if (auto lhs_term = parse_term()) {
            lhs_expr = m_allocator.alloc<Node::Expr>();
            lhs_expr.value()->var = lhs_term.value();
        }
        else {
            return {};
        }

        // Now handle binary operators (left or right associative)
        while (true) {
            auto op_token = peek();
            if (!op_token.has_value() || binop_info.find(op_token->type) == binop_info.end()) break;

            const auto& [prec, assoc] = binop_info.at(op_token->type);
            if (prec < min_prec) break;

            consume();
            int next_min_prec = (assoc == Assoc::Left) ? prec + 1 : prec;
            auto rhs_expr = parse_expr_prec(next_min_prec);
            if (!rhs_expr) {
                std::cerr << "Expected expression after operator\n";
                exit(EXIT_FAILURE);
            }

            auto bin_expr = m_allocator.alloc<Node::BinExpr>();
            if (op_token->type == TokenType::plus) {
                auto bin_expr_add = m_allocator.alloc<Node::BinExprAdd>();
                bin_expr_add->lhs = lhs_expr.value();
                bin_expr_add->rhs = rhs_expr.value();
                bin_expr->var = bin_expr_add;
            }
            else if (op_token->type == TokenType::minus) {
                auto bin_expr_sub = m_allocator.alloc<Node::BinExprSub>();
                bin_expr_sub->lhs = lhs_expr.value();
                bin_expr_sub->rhs = rhs_expr.value();
                bin_expr->var = bin_expr_sub;
            }
            else if (op_token->type == TokenType::multi) {
                auto bin_expr_multi = m_allocator.alloc<Node::BinExprMulti>();
                bin_expr_multi->lhs = lhs_expr.value();
                bin_expr_multi->rhs = rhs_expr.value();
                bin_expr->var = bin_expr_multi;
            }
            else if (op_token->type == TokenType::div) {
                auto bin_expr_div = m_allocator.alloc<Node::BinExprDiv>();
                bin_expr_div->lhs = lhs_expr.value();
                bin_expr_div->rhs = rhs_expr.value();
                bin_expr->var = bin_expr_div;
            }
            else if (op_token->type == TokenType::mod) {
                auto bin_expr_mod = m_allocator.alloc<Node::BinExprMod>();
                bin_expr_mod->lhs = lhs_expr.value();
                bin_expr_mod->rhs = rhs_expr.value();
                bin_expr->var = bin_expr_mod;
            }

            auto new_expr = m_allocator.alloc<Node::Expr>();
            new_expr->var = bin_expr;
            lhs_expr = new_expr;
        }

        return lhs_expr;
    }


    std::optional<Node::Expr*>parse_expr(){
        // if(auto term = parse_term()){
        //     if(try_consume(TokenType::plus).has_value()){
        //         auto bin_expr = m_allocator.alloc<Node::BinExpr>();
        //         auto bin_expr_add = m_allocator.alloc<Node::BinExprAdd>();
        //         auto lhs_expr = m_allocator.alloc<Node::Expr>();
        //         lhs_expr->var = term.value();

        //         bin_expr_add->lhs = lhs_expr;
                
        //         // consume();
        //         if(auto rhs = parse_expr()){
        //             bin_expr_add->rhs=rhs.value();
        //             bin_expr->var = bin_expr_add;
        //             auto expr = m_allocator.alloc<Node::Expr>();
        //             expr->var = bin_expr;
        //             return expr;
        //         }
        //         else{
        //             std::cerr<<"Expected expression"<<std::endl;
        //             exit(EXIT_FAILURE);
        //         }
        //     }
        //     else if(try_consume(TokenType::minus).has_value()){
        //         auto bin_expr = m_allocator.alloc<Node::BinExpr>();
        //         auto bin_expr_sub = m_allocator.alloc<Node::BinExprSub>();
        //         auto lhs_expr = m_allocator.alloc<Node::Expr>();
        //         lhs_expr->var = term.value();

        //         bin_expr_sub->lhs = lhs_expr;
                
        //         // consume();
        //         if(auto rhs = parse_expr()){
        //             bin_expr_sub->rhs=rhs.value();
        //             bin_expr->var = bin_expr_sub;
        //             auto expr = m_allocator.alloc<Node::Expr>();
        //             expr->var = bin_expr;
        //             return expr;
        //         }
        //         else{
        //             std::cerr<<"Expected expression"<<std::endl;
        //             exit(EXIT_FAILURE);
        //         }
        //     }
        //     else{
        //         auto expr = m_allocator.alloc<Node::Expr>();
        //         expr->var = term.value();
        //         return expr;
        //     }
        // }
        // else{
        //     return {};
        // }
        return parse_expr_prec(0);

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
        try_consume(TokenType::closed_paren, "Expected ')' after expression.");
        try_consume(TokenType::semi, "Expected ';' after exit statement.");

        auto stmt = m_allocator.alloc<Node::Stmt>();
        stmt->var = stmt_exit;
        return stmt;
    }

    if (token0 && token0->type == TokenType::print &&
        token1 && token1->type == TokenType::open_paren) {

        consume(); // 'print'
        consume(); // '('

        auto stmt_print = m_allocator.alloc<Node::StmtPrint>();

        if (auto expr = parse_expr()) {
            stmt_print->expr = expr.value();
        } else {
            std::cerr << "Invalid expression inside print()." << std::endl;
            exit(EXIT_FAILURE);
        }
        try_consume(TokenType::closed_paren, "Expected ')' after expression.");
        try_consume(TokenType::semi, "Expected ';' after print statement.");

        auto stmt = m_allocator.alloc<Node::Stmt>();
        stmt->var = stmt_print;
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
        }
        else {
            std::cerr << "Invalid expression in let statement." << std::endl;
            exit(EXIT_FAILURE);
        }

        try_consume(TokenType::semi, "Expected ';' after let statement.");

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