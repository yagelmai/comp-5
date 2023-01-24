#ifndef _TRANS_LLVM
#define _TRANS_LLVM

#include "bp.hpp"
#include <string>
#include "parser.hpp"
#include "global_symbs.hpp"

extern int COUNT_REG;
extern int COUNT_FUNC;

string genReg();

string genString();

// backpatch functions
std::string genLabel();

int emit(const std::string &command);

vector<pair<int,BranchLabelIndex>> makelist(pair<int,BranchLabelIndex> item);

vector<pair<int,BranchLabelIndex>> merge(const vector<pair<int,BranchLabelIndex>> &l1,const vector<pair<int,
        BranchLabelIndex>> &l2);

void bpatch(const vector<pair<int,BranchLabelIndex>>& address_list, const std::string &label);
void printCodeBuffer();
void emitGlobal(const string& dataLine);

void printGlobalBuffer();

// Initializing llvm (will be called at start of parsing)
void create_declarations();

void zero_division_handle(const string& divisor);

void create_binop_expression(const string& op, Expression* first_operand, Expression* second_operand, Expression* binop_type);

void create_relop_expression(const string& op, Expression* first_operand, Expression* second_operand, Expression* relop_type);

string create_bool_exp(Expression* exp);

void create_new_variable(int offset);

void create_expression(Expression* exp, int offset);

string create_function_invocation(Expression* exp_fun_call, const string& name_function, const string& func_return_type, ExpressionList* param_lst);

string create_function_invocation_parameterless(Expression* exp_fun_call, const string& name_function, const string& func_return_type);

void create_id_expression(Expression* exp_id, int offset);

void create_str_expression(Expression* str_exp);

int create_switch_case(CaseList* case_lst, Expression* exp);


#endif //_TRANS_LLVM