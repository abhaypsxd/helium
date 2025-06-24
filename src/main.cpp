#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <optional>
#include <vector>

#include "tokenization.hpp"
#include "parser.hpp"
#include "generation.hpp"

using namespace std;


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

    Parser parser(move(tokens));
    optional<Node::Prog> prog = parser.parse_prog();

    if(!prog.has_value()){
        cerr<<"Invalid Program"<<endl;
        exit(EXIT_FAILURE);
    }

    Generator generator(prog.value());
    {
        fstream file("./out.asm", ios::out);
        file<<generator.gen_prog();
    }

    system("nasm -felf64 out.asm");
    system("ld -o out out.o");

    return EXIT_SUCCESS;
}