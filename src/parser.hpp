#pragma once

#include <vector>
#include <optional>
#include <string>

#include "tokenization.hpp"

namespace Node{
    struct Expr{
        Token int_lit;
    };

    struct Exit{
        Expr expr;
    };

}


class Parser{
private:
    const std::vector<Token> m_tokens;
    size_t m_index = 0;

    [[nodiscard]] inline std::optional<Token>peak(int ahead = 0) const {
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
        if(peak().has_value()&&peak().value().type==TokenType::int_lit){
            return Node::Expr{.int_lit = consume()};
        }
        else return {};

    }

    std::optional<Node::Exit> parse(){
        std::optional<Node::Exit>exit_node;
        while(peak().has_value()){
            if(peak().value().type==TokenType::exit){
                consume();
                if(auto node_expr = parse_expr()){
                    exit_node = Node::Exit{.expr = node_expr.value()};
                }
                else{
                    std::cerr<<"Invalid Expression."<<std::endl;
                    exit(EXIT_FAILURE);
                }
                if(peak().has_value()&&peak().value().type==TokenType::semi){
                    consume();
                    continue;
                }
                else{
                    std::cerr<<"Invalid Expression."<<std::endl;
                    exit(EXIT_FAILURE);
                }
            }
        }
        m_index=0;
        return exit_node;
    }



};