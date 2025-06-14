#pragma once

#include <string>
#include <sstream>

#include "parser.hpp"

class Generator{
private:
    const Node::Exit m_root;


public:
    inline explicit Generator(Node::Exit root):m_root(std::move(root)){

    }

    [[nodiscard]] std::string generate() const {
        std::stringstream output;
        output << "section .text\n";
        output << "global _start\n_start:\n";
        output<<"    mov rax, 60\n";
        output<<"    mov rdi, "<<m_root.expr.int_lit.value.value()<<"\n";
        output<<"    syscall";
        return output.str();
    }

};