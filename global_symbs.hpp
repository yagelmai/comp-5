#ifndef GLOB_SYMBS
#define GLOB_SYMBS

#include <vector>
#include <string>
#include <list>
#include <stack>
#include "parser.hpp"
#include "hw3_output.hpp"
#include <iostream>

using namespace std;
class Symbol
{
    public:
    std::string name;
    Types type;
    bool is_func;
    int offset;
    Symbol(std::string name, Types type, bool is_func,int offset):name(name),type(type),is_func(is_func),offset(offset)
    {
    };

    std::string getName() { return this->name; }
    Types getTypes() { return this->type; }
    int getOffset() { return this->offset; }
    //void addSymbol
};


class InnerSymbs
{
    list<Symbol> symbols;

public:
    InnerSymbs()
    {
        symbols = list<Symbol>();
    };
    void printAllSymbs(){
        while(!symbols.empty()){
            output::printID(symbols.front().name,symbols.front().offset, typeToString(symbols.front().type));
            symbols.pop_front();
        }
    }
    void addSymbol(Symbol sym){
        symbols.push_back(sym);
    }
    list<Symbol> &getEntries()
    {
        return this->symbols;
    }
    std::string typeToString(Types type)
    {
        switch(type){
            case TYPE_BOOL: return "BOOL";
            case TYPE_BYTE: return "BYTE";
            case TYPE_INT: return "INT";
            case TYPE_STRING: return "STRING";
            default: return "ERROR";
        }
    }
};

class Function
{
    
    public:
    vector<Symbol> symbols;
    std::string name;
    Types return_type;
    Function(std::string name,Types return_type):name(name),return_type(return_type){}
    void add_symbol(Symbol sym)
    {
        symbols.emplace_back(sym);//do we have to check if same name appears twice in formals?
    }
};

class GlobalSymbs
{
    public:
        std::list<InnerSymbs> symbolTables;
        std::stack<int> offset;
        std::list<Function> all_functions;
        int in_while;
        std::list<Symbol> current_function_parameters;
        Types current_function_type;
        int current_function_offset;
        GlobalSymbs();
        ~GlobalSymbs() = default;
        bool isExist(std::string id);
        Types findID(std::string id);
        void addSymbol(Types type, std::string name);
        void addFunction(std::string name,Types type);//implement
        void enterWhile();//implement
        void exitWhile();//implement
        void openScope();//implement
        void closeScope();//implement
        void addFormal(Types type, std::string name);//implemnt
        void clearFormals();//implement
        void currentFunctionType(Types type);//implement
        bool checkInWhile();
        void checkVariables(std::string id, ExpressionList* El);
        void comparesTypesCast(Types first,Types second);
        int getOffset();//implement
        void compareTypesAssignment(Types assigned_to, Types assigned_from);
        void compareRelop(Types assigned_to, Types assigned_from);
        void checkFunctionType(Types type);
        void checkIfBool(Types type);
        Types getVarType(std::string id);
        Types getFunctionType(std::string id);
        void printFunctions();
        string typeToString(Types type);
};

#endif /*GLOB_SYMBS*/