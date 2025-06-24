#pragma once

#include <string>
#include <sstream>
#include <unordered_map>

#include "parser.hpp"

class Generator{
private:

    void push(const std::string& reg){
        m_output<<"    push "<<reg<<"\n";
        m_stack_size++;
    }

    void pop(const std::string& reg){
        m_output<<"    pop "<<reg<<"\n";
        m_stack_size--;
    }

    const Node::Prog m_prog;
    std::stringstream m_output;
    size_t m_stack_size = 0;

    struct Var{
        size_t stack_loc;
    };

    std::unordered_map<std::string, Var>m_vars{};


public:
    inline explicit Generator(Node::Prog prog):m_prog(std::move(prog)){

    }

    void gen_expr(const Node::Expr& expr) {

        struct ExprVisitor{

            Generator* gen;
            // explicit ExprVisitor(Generator* gen):gen(gen){}

            void operator()(const Node::ExprIntLit& expr_int_lit)const{
                gen->m_output<<"    mov rax, "<<expr_int_lit.int_lit.value.value()<<"\n";
                gen->push("rax");
                // gen->m_output<<"    push rax\n";
            }
            void operator()(const Node::ExprIdent& exp_ident){
                if(!gen->m_vars.contains(exp_ident.ident.value.value())){
                    std::cerr<<"Undeclared Identifier: "<<exp_ident.ident.value.value()<<std::endl;
                    exit(EXIT_FAILURE);
                }
                const auto& var = gen->m_vars.at(exp_ident.ident.value.value());
                std::stringstream offset;
                offset<<"QWORD [rsp + "<<(gen->m_stack_size-var.stack_loc-1)*8<<"]\n";
                gen->push(offset.str());
            }
        };
        ExprVisitor visitor{.gen = this};
        std::visit(visitor, expr.var);
    }

    void gen_stmt(const Node::Stmt& stmt) {
        struct StmtVisitor{
            Generator* gen;


            void operator()(const Node::StmtExit& stmt_exit)const{
                gen->gen_expr(stmt_exit.expr);
                gen->m_output<<"    mov rax, 60\n";
                gen->pop("rdi");
                // gen->m_output<<"    pop rdi\n";
                gen->m_output<<"    syscall\n";
            }
            void operator()(const Node::StmtLet& stmt_let){
                if(gen->m_vars.contains(stmt_let.ident.value.value())){
                    std::cerr<<"Identifier already used: "<<stmt_let.ident.value.value()<<std::endl;
                    exit(EXIT_FAILURE);
                }
                gen->m_vars.insert({stmt_let.ident.value.value(), Var{.stack_loc = gen->m_stack_size}});
                gen->gen_expr(stmt_let.expr);
            } 
        };

        StmtVisitor visitor{.gen = this};
        std::visit(visitor, stmt.var);
    }

    [[nodiscard]] std::string gen_prog() {
        m_output << "section .text\n";
        m_output << "global _start\n_start:\n";

        for(const Node::Stmt& stmt:m_prog.stmts){
            gen_stmt(stmt);
        }
        m_output<<"    mov rax, 60\n";
        m_output<<"    mov rdi, 0\n";
        m_output<<"    syscall\n";


        return m_output.str();
    }

};