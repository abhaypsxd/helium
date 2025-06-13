#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <optional>
#include <vector>
#include "tokenization.hpp"

using namespace std;

string tokens_to_asm(const vector<Token>&tokens){
    stringstream output;
    output << "section .text\n";
    output << "global _start\n_start:\n";
    for(int i = 0; i<tokens.size(); i++){
        const Token& token = tokens.at(i);
        if(token.type==TokenType::exit){
            if(i+1<tokens.size()&&tokens.at(i+1).type==TokenType::int_lit){
                if(i+2<tokens.size()&&tokens.at(i+2).type==TokenType::semi){
                    Token number = tokens.at(i+1);
                    output<<"    mov rax, 60\n";
                    output<<"    mov rdi, "<<tokens.at(i+1).value.value()<<"\n";
                    output<<"    syscall";
                }
            }
        }
    }
    return output.str();
}

int main(int argc, char* argv[]){
    if(argc!=2){
        cerr<<"Incorrect usage."<<endl;
        cerr<<"use helium <input.hy>"<<endl;
        return EXIT_FAILURE;
    }

    string contents;
    stringstream input_stream;
    {
        fstream input_file(argv[1], ios::in);
        input_stream<<input_file.rdbuf();
        contents = input_stream.str();
    }
    Tokenizer tokenizer(contents);

    vector<Token>tokens=tokenizer.tokenize();

    {
        fstream file("./out.asm", ios::out);
        file<<tokens_to_asm(tokens);
    }

    system("nasm -felf64 out.asm");
    system("ld -o out out.o");

    return EXIT_SUCCESS;
}