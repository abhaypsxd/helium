#pragma once

#include <string>
#include <sstream>
#include <unordered_map>
#include <assert.h>


#include "parser.hpp"

class Generator{
private:

    void emit_print_int() {
    static bool emitted = false;
    if (emitted) return;
    emitted = true;

    m_output <<
        "\nprint_int:\n"
        "    push rbx\n"
        "    push rcx\n"
        "    push rdx\n"
        "    push rsi\n"
        "    sub rsp, 32\n"              // reserve buffer space
        "    mov rsi, rsp\n"             // rsi = buffer start
        "    mov rcx, rsi\n"
        "    add rcx, 32\n"              // rcx = buffer end
        "    mov rax, rdi\n"             // rax = number to print
        "    mov rbx, 10\n"              // base 10\n"
        ".print_loop:\n"
        "    xor rdx, rdx\n"
        "    div rbx\n"                  // divide rax by 10
        "    add dl, '0'\n"              // convert remainder to ASCII
        "    dec rcx\n"
        "    mov [rcx], dl\n"
        "    test rax, rax\n"
        "    jnz .print_loop\n"
        "    mov rdx, rsp\n"
        "    add rdx, 32\n"              // end of buffer
        "    sub rdx, rcx\n"             // rdx = number of bytes
        "    mov rsi, rcx\n"             // rsi = string start
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    syscall\n"
        "    ; newline\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    lea rsi, [rel newline]\n"
        "    mov rdx, 1\n"
        "    syscall\n"
        "    add rsp, 32\n"
        "    pop rsi\n"
        "    pop rdx\n"
        "    pop rcx\n"
        "    pop rbx\n"
        "    ret\n"
        "newline: db 0xA\n";
}



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

    void gen_bin_expr(const Node::BinExpr* bin_expr){
        struct BinExprVisitor{
            Generator* gen;
            void operator()(const Node::BinExprAdd* bin_expr_add)const{
                gen->gen_expr(bin_expr_add->lhs);
                gen->gen_expr(bin_expr_add->rhs);
                gen->pop("rbx");
                gen->pop("rax");
                gen->m_output<<"    add rax, rbx\n";
                gen->push("rax");
            }
            void operator()(const Node::BinExprSub* bin_expr_sub) const{
                gen->gen_expr(bin_expr_sub->lhs);
                gen->gen_expr(bin_expr_sub->rhs);
                gen->pop("rbx");
                gen->pop("rax");
                gen->m_output<<"    sub rax, rbx\n";
                gen->push("rax");
            }
            void operator()(const Node::BinExprMulti* bin_expr_multi) const{
                gen->gen_expr(bin_expr_multi->lhs);
                gen->gen_expr(bin_expr_multi->rhs);
                gen->pop("rbx");
                gen->pop("rax");
                gen->m_output<<"    imul rax, rbx\n";
                gen->push("rax");
            }
            void operator()(const Node::BinExprDiv* bin_expr_div){
                gen->gen_expr(bin_expr_div->lhs);
                gen->gen_expr(bin_expr_div->rhs);
                gen->pop("rbx");
                gen->pop("rax");
                gen->m_output<<"    xor rax, rbx\n";
                gen->m_output<<"    div rbx\n";
                gen->push("rax");
            }
            void operator()(const Node::BinExprMod* bin_expr_mod){
                gen->gen_expr(bin_expr_mod->lhs);
                gen->gen_expr(bin_expr_mod->rhs);
                gen->pop("rbx");
                gen->pop("rax");
                gen->m_output<<"    xor rax, rbx\n";
                gen->m_output<<"    div rbx\n";
                gen->push("rdx");
            }
        };
        BinExprVisitor visitor({.gen = this});
        std::visit(visitor, bin_expr->var);
    }

    void gen_term(const Node::Term* term){
        struct TermVisitor{
            Generator* gen;
            void operator()(const Node::TermIntLit* term_int_lit)const{
                gen->m_output<<"    mov rax, "<<term_int_lit->int_lit.value.value()<<"\n";
                gen->push("rax");
            }
            void operator()(const Node::TermIdent* term_ident){
                if(!gen->m_vars.contains(term_ident->ident.value.value())){
                    std::cerr<<"Undeclared Identifier: "<<term_ident->ident.value.value()<<std::endl;
                    exit(EXIT_FAILURE);
                }
                const auto& var = gen->m_vars.at(term_ident->ident.value.value());
                std::stringstream offset;
                offset<<"QWORD [rsp + "<<(gen->m_stack_size-var.stack_loc-1)*8<<"]";
                gen->push(offset.str());
            }
            
        };
        TermVisitor visitor({.gen = this});
        std::visit(visitor,term->var);
    }

    void gen_expr(const Node::Expr* expr) {

        struct ExprVisitor{

            Generator* gen;
            // explicit ExprVisitor(Generator* gen):gen(gen){}

            void operator()(const Node::Term* term)const{
                gen->gen_term(term);
            }
            void operator()(const Node::BinExpr* bin_expr) const {
                gen->gen_bin_expr(bin_expr);
            }
        };
        ExprVisitor visitor{.gen = this};
        std::visit(visitor, expr->var);
    }

    void gen_stmt(const Node::Stmt* stmt) {
        struct StmtVisitor{
            Generator* gen;


            void operator()(const Node::StmtExit* stmt_exit)const{
                gen->gen_expr(stmt_exit->expr);
                gen->m_output<<"    mov rax, 60\n";
                gen->pop("rdi");
                // gen->m_output<<"    pop rdi\n";
                gen->m_output<<"    syscall\n";
            }
            void operator()(const Node::StmtLet* stmt_let){
                if(gen->m_vars.contains(stmt_let->ident.value.value())){
                    std::cerr<<"Identifier already used: "<<stmt_let->ident.value.value()<<std::endl;
                    exit(EXIT_FAILURE);
                }
                gen->m_vars.insert({stmt_let->ident.value.value(), Var{.stack_loc = gen->m_stack_size}});
                gen->gen_expr(stmt_let->expr);
            }
            void operator()(const Node::StmtPrint* stmt_print){
                gen->gen_expr(stmt_print->expr);
                gen->pop("rdi");
                gen->m_output<<"    call print_int\n";
            }
        };

        StmtVisitor visitor{.gen = this};
        std::visit(visitor, stmt->var);
    }

    [[nodiscard]] std::string gen_prog() {
        m_output << "section .text\n";
        m_output << "global _start\n_start:\n";

        for(const Node::Stmt* stmt:m_prog.stmts){
            gen_stmt(stmt);
        }

        emit_print_int();
        m_output<<"    mov rax, 60\n";
        m_output<<"    mov rdi, 0\n";
        m_output<<"    syscall\n";


        return m_output.str();
    }

};