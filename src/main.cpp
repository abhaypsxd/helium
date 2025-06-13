#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <optional>
#include <vector>
using namespace std;

enum class TokenType{
    _return,
    int_lit,
    semi
};

struct Token{
    TokenType type;
    optional<string> value{};

};

vector<Token> tokenize(const string& str){
    string buf;
    vector<Token>tokens {};
    for(int i = 0; i<str.length(); i++){
        char c = str[i];
        if(isalpha(c)){
            while(isalnum(str[i])){
                buf.push_back(str[i]);
                i++;
            }
            i--;
            if(buf == "return"){
                tokens.push_back({.type = TokenType::_return});
                buf.clear();
                continue;
            }
            else{
                cerr<<"Wrong syntax!"<<endl;
                exit(EXIT_FAILURE);
            }
        }
        else if(isspace(c)){
            continue;
        }
        else if (isdigit(c)){;
            while(isdigit(str[i])){
                buf.push_back(str[i]);
                i++;
            }
            i--;
            tokens.push_back({.type = TokenType::int_lit, .value = buf});
            buf.clear();
            continue;
        }
        else if(c==';'){
            tokens.push_back({.type = TokenType::semi});
        }
        else{
            cerr<<"Wrong syntax!"<<endl;
            exit(EXIT_FAILURE);
        }
    }
    return tokens; 
}

string tokens_to_asm(const vector<Token>&tokens){
    stringstream output;
    output << "section .text\n";
    output << "global _start\n_start:\n";
    for(int i = 0; i<tokens.size(); i++){
        const Token& token = tokens.at(i);
        if(token.type==TokenType::_return){
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
    vector<Token>tokens=tokenize(contents);

    {
        fstream file("./out.asm", ios::out);
        file<<tokens_to_asm(tokens);
    }

    system("nasm -felf64 out.asm");
    system("ld -o out out.o");

    return EXIT_SUCCESS;
}