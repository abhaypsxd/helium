#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <optional>
#include <vector>


enum class TokenType{exit, int_lit, semi, open_paren, closed_paren, ident, let, eq, plus, minus, multi, div, mod};
struct Token{
    TokenType type;
    std::optional<std::string> value{};
};

class Tokenizer{
private:
    const std::string m_src;
    size_t m_index = 0;
    
    [[nodiscard]] inline std::optional<char>peek(int ahead = 0) const {
        if(m_index+ahead>=m_src.length()){
            return {};
        }
        else {
            return m_src.at(m_index+ahead);
        }
    }

    inline char consume(){
        return m_src.at(m_index++);
    }

public:
    inline explicit Tokenizer(std::string&src):m_src(std::move(src)){

    }

    inline std::vector<Token>tokenize(){
        std::string buf;
        std::vector<Token>tokens {};

        while(peek().has_value()){
            if(std::isalpha(peek().value())){
                buf.push_back(consume());
                while(peek().has_value()&&std::isalnum(peek().value())){
                    buf.push_back(consume());
                }
                if(buf=="exit"){
                    tokens.push_back({.type = TokenType::exit});
                    buf.clear();
                    continue;
                }
                else if(buf=="let"){
                    tokens.push_back({.type = TokenType::let});
                    buf.clear();
                    continue;
                }
                else{
                    tokens.push_back({.type = TokenType::ident, .value = buf});
                    buf.clear();
                    continue;
                }
            }
            else if(std::isdigit(peek().value())){
                buf.push_back(consume());
                while(peek().has_value()&&std::isdigit(peek().value())){
                    buf.push_back(consume());
                }
                tokens.push_back({.type = TokenType::int_lit, .value = buf});
                buf.clear();
                continue;
            }
            else if(peek().value()=='('){
                consume();
                tokens.push_back({.type = TokenType::open_paren});
            }
            else if(peek().value()==')'){
                consume();
                tokens.push_back({.type = TokenType::closed_paren});
            }
            else if(peek().value()=='='){
                consume();
                tokens.push_back({.type = TokenType::eq});
            }
            else if(peek().value()=='+'){
                consume();
                tokens.push_back({.type = TokenType::plus});
            }
            else if(peek().value()=='-'){
                consume();
                tokens.push_back({.type  = TokenType::minus});
            }
            else if(peek().value()=='*'){
                consume();
                tokens.push_back({.type  = TokenType::multi});
            }
            else if(peek().value()=='/'){
                consume();
                tokens.push_back({.type  = TokenType::div});
            }
            else if(peek().value()=='%'){
                consume();
                tokens.push_back({.type  = TokenType::mod});
            }
            else if(peek().value()==';'){
                consume();
                tokens.push_back({.type = TokenType::semi});
                continue;
            }
            else if(std::isspace(peek().value())){
                consume();
                continue;
            }
            else{
                
                std::cerr<<"Wrong syntax!"<<std::endl;
                exit(EXIT_FAILURE);
            }
            
        }

        m_index = 0;
        return tokens; 
    }
    
};