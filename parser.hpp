#ifndef _PARSER_H
#define _PARSER_H
#include <vector>
#include <string>
#include <list>
#include <stack>
#include "bp.hpp"
extern int yylineno;
using namespace std;
#define YYSTYPE Node*
#include <iostream>

enum Types
{
    TYPE_UNDEFINED,
    TYPE_VOID,
    TYPE_BOOL,
    TYPE_BYTE,
    TYPE_INT, 
    TYPE_STRING
};


class Node
{
public:
    //added
    vector<pair<int,BranchLabelIndex>> next_list;
    vector<pair<int,BranchLabelIndex>> true_list;
    vector<pair<int,BranchLabelIndex>> false_list;
    vector<pair<int,BranchLabelIndex>> continue_list;
    vector<pair<int,BranchLabelIndex>> break_list;
    string reg;
    string expression_label_specific;
    //
    std::string value;
    Types type;
    Node(){};
    Node(string token_value) : value()
    {
        value = token_value;
        type = TYPE_UNDEFINED;
    }
    Node(string token_value, Types type) : value(), type()
    {
        this->value = token_value;
        this->type = type;
    }
    Node(Types type):type(type){};
    std::string getValue(){
        return value;
    }
};
class FormalDecl;
class Formals;
class FormalsList;
class Id : public Node
{
public:
    Id(string value) : Node(value) {}
};

class Expression: public Node
{
public:
    Expression(Types type):Node(type){};
    
};

class ExpressionList: public Node
{
public:
    std::vector<Expression*> exp_list;
    ExpressionList():Node(TYPE_UNDEFINED){
        //std::cout<<"ExpressionList()"<< std::endl;
    };
    void addExpression(Expression* exp){
        // std::cout<<"addExpression()"<< std::endl;
        exp_list.emplace_back(exp);
        // std::cout<<exp_list.size()<< std::endl;
        for(Expression* curr_exp : exp_list)
        {
            if(curr_exp->value == exp->value){

                //error formal already in function
            }
        }
    }

};
class Number : public Node
{
    public:
    int num;
    Number(Types type, string num_s):Node(type) {
        //std::cout<<"Number()"<< std::endl;
        num=std::stoi(num_s);
    };
};
class Type : public Node
{
public:
Type(Types v_type):Node(v_type){};
};

class FormalDecl : public Node
{
public:
    FormalDecl(Type *type, Id *symbol);
};

class Formals : public Node
{
public:
    vector<FormalDecl *> declaration;
    Formals() = default;
    Formals(FormalsList *f_list);
};

class FormalsList : public Formals
{
public:
    FormalsList(FormalDecl *f_dec);
    FormalsList(FormalDecl *f_dec, FormalsList *f_list);
};



class EntryT
{
    Types returnValue;
    string name;
    vector<Types> types;
    bool isFunc;
    int offset;
    

public:
    EntryT();
    explicit EntryT(string nm, Types t, int offset) // for single symbol
    {
        this->name = nm;
        this->types = vector<Types>(1, t);
        this->returnValue = TYPE_UNDEFINED;
        this->offset = offset;
        this->isFunc = false;
    }

    explicit EntryT(string &nm, vector<Types> &typs, Types rValue, int offset) // for function
    {
        this->name = nm;
        this->types = typs;
        this->returnValue = rValue;
        this->offset = offset;
        this->isFunc = true;
    }

    ~EntryT() = default;

    string &getName() { return this->name; }
    vector<Types> &getTypes() { return this->types; }
    Types getReturnValue() { return this->returnValue; }
    int getOffset() { return this->offset; }
    bool getIsFunc() { return this->isFunc; }
};


class Table
{
    vector<EntryT *> symbols;

public:
    Table()
    {
        symbols = vector<EntryT *>();
    }

    ~Table() = default;

    vector<EntryT *> &getEntries()
    {
        return this->symbols;
    }
};

//added
struct CaseDecl : public Node{
    int case_value;
    string quad_label; // to here we jump if the case is correct

    CaseDecl(int case_val, const string& quad_lbl) : case_value(case_val), quad_label(quad_lbl){}

};

struct CaseList : public Node{
    stack<CaseDecl*> case_list;
    bool has_default;

    CaseList(CaseDecl* case_decl, bool has_default){
        case_list.push(case_decl); // in here we save the quads and values of each one. it's a stack.
        this->has_default = has_default;
    }

    void addCase(CaseDecl* case_decl){
        case_list.push(case_decl);
    }
};
//


#endif //_PARSER_H