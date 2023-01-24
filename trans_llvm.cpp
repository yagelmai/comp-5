#include "trans_llvm.hpp"


int COUNT_REG = 0;
int COUNT_FUNC = 0;

string genReg(){
    string reg = "%reg" + std::to_string(COUNT_REG++);
    return reg;
}

string genString(){
    string str = "@.string" + std::to_string(COUNT_REG++);
    return str;
}

// backpatch functions from bp.cpp
std::string genLabel(){
    return CodeBuffer::instance().genLabel();
}

int emit(const std::string &command){
    return CodeBuffer::instance().emit(command);
}

vector<pair<int,BranchLabelIndex>> makelist(pair<int,BranchLabelIndex> item){
    return CodeBuffer::makelist(item);
}

vector<pair<int,BranchLabelIndex>> merge(const vector<pair<int,BranchLabelIndex>> &l1,const vector<pair<int,
        BranchLabelIndex>> &l2){
    return CodeBuffer::merge(l1, l2);
}

void bpatch(const vector<pair<int,BranchLabelIndex>>& address_list, const std::string &label){
    CodeBuffer::instance().bpatch(address_list, label);
}

void printCodeBuffer(){
    CodeBuffer::instance().printCodeBuffer();
}

void printGlobalBuffer(){
    CodeBuffer::instance().printGlobalBuffer();
}

void emitGlobal(const string& dataLine){
    CodeBuffer::instance().emitGlobal(dataLine);
}


// Initializing llvm (will be called at start of parsing)
void create_declarations() {
    // Global string variables
    emitGlobal("@.int_specifier = constant [4 x i8] c\"%d\\0A\\00\"");
    emitGlobal("@.str_specifier = constant [4 x i8] c\"%s\\0A\\00\"");
    emitGlobal("@.zero_div_error = constant [23 x i8] c\"Error division by zero\\00\"");

    // Declaring printf and exit
    emit("declare i32 @printf(i8*, ...)");
    emit("declare void @exit(i32)");

    // Defining printi for getting numers
    emit("define void @printi(i32) {");
    emit("%spec_ptr = getelementptr [4 x i8], [4 x i8]* @.int_specifier, i32 0, i32 0");
    emit("call i32 (i8*, ...) @printf(i8* %spec_ptr, i32 %0)");
    emit("ret void");
    emit("}");

    // Defining print for getting strings
    emit("define void @print(i8*) {");
    emit("%spec_ptr = getelementptr [4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0");
    emit("call i32 (i8*, ...) @printf(i8* %spec_ptr, i8* %0)");
    emit("ret void");
    emit("}");
}

void zero_division_handle(const string& divisor){
    string div_branch_res = genReg();
    emit(div_branch_res + " = icmp eq i32 " + divisor + ", 0");
    int branch_loc_1 = emit("br i1 " + div_branch_res + ", label @, label @");
    string true_label = genLabel();
    emit("call void @print(i8* getelementptr ([23 x i8], [23 x i8]* @.zero_div_error, i32 0, i32 0))");
    emit("call void @exit(i32 1)");
    int branch_loc_2 = emit("br label @");
    string false_label = genLabel();
    bpatch(makelist({branch_loc_1, FIRST}), true_label);
    bpatch(makelist({branch_loc_1, SECOND}), false_label);
    bpatch(makelist({branch_loc_2, FIRST}), false_label);
}

void create_binop_expression(const string& op, Expression* first_operand, Expression* second_operand,
                             Expression* binop_type){
    string binop_result_reg = binop_type->reg;
    string op1_reg = first_operand->reg;
    string op2_reg = second_operand->reg;
    Types type_binop = binop_type->type;
    string binop_command;
    if(type_binop == TYPE_INT){
        if(op == "+"){
            binop_command = binop_result_reg + " = add i32 " + op1_reg + ", " + op2_reg;
            emit(binop_command);
        }
        else if(op == "-"){
            binop_command = binop_result_reg + " = sub i32 " + op1_reg + ", " + op2_reg;
            emit(binop_command);
        }
        else if(op == "*"){
            binop_command = binop_result_reg + " = mul i32 " + op1_reg + ", " + op2_reg;
            emit(binop_command);
        }
        else if(op == "/"){
            binop_command = binop_result_reg + " = sdiv i32 " + op1_reg + ", " + op2_reg;
            zero_division_handle(op2_reg);
            emit(binop_command);
        }
    }
    // Otherwise, we are in the case of byte and need to use trunc and zext.
    else{
        string intermediate_reg = genReg();
        string truncated_reg = genReg();
        if(op == "+"){
            binop_command = intermediate_reg + " = add i32 " + op1_reg + ", " + op2_reg;
            emit(binop_command);
        }
        else if(op == "-"){
            binop_command = intermediate_reg + " = sub i32 " + op1_reg + ", " + op2_reg;
            emit(binop_command);
        }
        if(op == "*"){
            binop_command = intermediate_reg + " = mul i32 " + op1_reg + ", " + op2_reg;
            emit(binop_command);
        }
        else if(op == "/"){
            binop_command = intermediate_reg + " = sdiv i32 " + op1_reg + ", " + op2_reg;
            zero_division_handle(op2_reg);
            emit(binop_command);
        }
        string trunc = truncated_reg + " = trunc i32 " + intermediate_reg + " to i8";
        emit(trunc);
        string zext = binop_result_reg + " = zext i8 " + truncated_reg + " to i32";
        emit(zext);
    }
}

void create_relop_expression(const string& op, Expression* first_operand, Expression* second_operand,
                             Expression* relop_type){
    string relop_llvm;
    string op1_reg = first_operand->reg;
    string op2_reg = second_operand->reg;
    if(op == "=="){
        relop_llvm = "eq";
    } else if(op == "!="){
        relop_llvm = "ne";
    } else if(op == "<"){
        relop_llvm = "slt";
    } else if(op == ">"){
        relop_llvm = "sgt";
    } else if(op == "<="){
        relop_llvm = "sle";
    } else if(op == ">="){
        relop_llvm = "sge";
    }

    string condition_reg = genReg();
    emit(condition_reg + " = icmp " + relop_llvm + " i32 " + op1_reg + ", " + op2_reg);
    int branch_loc = emit("br i1 " + condition_reg + ", label @, label @");
    relop_type->true_list = makelist({branch_loc, FIRST});
    relop_type->false_list = makelist({branch_loc, SECOND});
}

string create_bool_exp(Expression* expression){
    string false_label = genLabel();
    string empty_branch = "br label @";
    bpatch(expression -> false_list, false_label);
    int addr_case_false = emit(empty_branch);
    string true_label = genLabel();
    bpatch(expression -> true_list, true_label);
    int addr_case_true = emit(empty_branch);
    string label_of_phi = genLabel();
    bpatch(makelist({addr_case_true, FIRST}), label_of_phi);
    bpatch(makelist({addr_case_false, FIRST}), label_of_phi);
    string reg_of_phi = genReg();
    emit(reg_of_phi + " = phi i1 [0, %" + false_label+"], [1, %" + true_label + "]");
    string final_bool_reg = genReg();
    emit(final_bool_reg + " = zext i1 " + reg_of_phi + " to i32");
    string curr_lbl = expression->expression_label_specific;
    if(curr_lbl.empty() == false) emit("br label %" + curr_lbl);
    return final_bool_reg;
}

void create_new_variable(int offset) {
    string var_stack = genReg();
    string used_stack;
    int real_offset;
    if(offset < 0) {
        real_offset = (offset * (-1)) - 1;
        used_stack = "params";
    } else{
        real_offset = offset;
        used_stack = "vars";
    }
    string str_cnt = std::to_string(COUNT_FUNC);
    string str_offset = std::to_string(real_offset);
    emit(var_stack + " = getelementptr inbounds i32, i32* %fun" + str_cnt + used_stack + ", i32 " + str_offset);
    emit("store i32 0, i32* " + var_stack); // default value for bool and numbers is zero (false = 0 also)
}

void create_expression(Expression* exp, int offset){
    string arg_stack = genReg();
    string count_func_str = std::to_string(COUNT_FUNC);
    string used_stack;
    int real_offset;
    if(offset < 0) {
        real_offset = (offset * (-1)) - 1;
        used_stack = "params";
    } else{
        real_offset = offset;
        used_stack = "vars";
    }
    string offset_str = std::to_string(real_offset);

    string assign_var = arg_stack + " = getelementptr inbounds i32, i32* %fun" + count_func_str + used_stack + ", i32 " + offset_str;
    if(exp->type == TYPE_BOOL){
        string bool_final_res = create_bool_exp(exp);
        emit(assign_var);
        emit("store i32 " + bool_final_res + ", i32* " + arg_stack);
    }
    else{
        emit(assign_var);
        emit("store i32 " + exp->reg + ", i32* " + arg_stack);
    }
}

string create_function_invocation(Expression* exp_fun_call, const string& name_function, const Types& func_return_type, ExpressionList* param_lst){
    int branch_loc = emit("br label @");
    param_lst->next_list = makelist({branch_loc, FIRST});
    // Initial initialization for void return case
    string return_register = "";
    string return_type = "void";
    string assign_call = "";
    string str_handle_arguments = "";

    if(func_return_type != TYPE_VOID){
        return_type = "i32";
        return_register = genReg();
        assign_call = return_register + " = ";
    }
    for(int i = 0; i < param_lst->exp_list.size(); i++){
        unsigned int curr_position = param_lst->exp_list.size()-i-1;
        string arg_type;
        string arg_register;
        if(name_function == "print"){
            arg_type = "i8*";
            arg_register = param_lst->exp_list[curr_position]->reg;
        }
        else{
            if(param_lst->exp_list[curr_position]->type == TYPE_BOOL){
                arg_type = "i32";
                arg_register = create_bool_exp(param_lst->exp_list[curr_position]);
            }
            else{
                arg_type = "i32";
                arg_register = param_lst->exp_list[curr_position]->reg;
            }
        }
        str_handle_arguments += arg_type + " " + arg_register;

        if(i < param_lst->exp_list.size()-1){
            str_handle_arguments += ", ";
        }
    }
    branch_loc = emit("br label @");
    string branch_lbl = genLabel();
    emit(assign_call + "call " + return_type + " @" + name_function + "(" + str_handle_arguments + ")");
    bpatch(merge(makelist({branch_loc, FIRST}), param_lst->next_list), branch_lbl);
    if(func_return_type == TYPE_BOOL){
        string cond_reg = genReg();
        emit(cond_reg + " = icmp eq i32 0, " + return_register);
        branch_loc = emit("br i1 " + cond_reg +", label @, label @");
        exp_fun_call->true_list = makelist({branch_loc,SECOND});
        exp_fun_call->false_list = makelist({branch_loc,FIRST});
        return "";
    }
    return return_register;
}

string create_function_invocation_parameterless(Expression* exp_fun_call, const string& name_function, const Types& func_return_type){

    // Initial initialization for void return case
    string return_register = "";
    string return_type = "void";
    string assign_call = "";

    int branch_loc = emit("br label @");
    string branch_lbl = genLabel();

    if(func_return_type != TYPE_VOID){
        return_type = "i32";
        return_register = genReg();
        assign_call = return_register + " = ";
    }

    emit(assign_call + "call " + return_type + " @" + name_function + "()");
    bpatch(makelist({branch_loc, FIRST}), branch_lbl);
    if(func_return_type == TYPE_BOOL){
        string branch = genReg();
        emit(branch + " = icmp eq i32 0, " + return_register);
        int address = emit("br i1 " + branch +", label @, label @");
        exp_fun_call->false_list = makelist({address,FIRST});
        exp_fun_call->true_list = makelist({address,SECOND});
        return "";
    }
    return return_register;
}

void create_id_expression(Expression* exp_id, int offset){
    string arg_stack = genReg();
    string used_stack;
    int real_offset;
    if(offset < 0) {
        real_offset = (offset * (-1)) - 1;
        used_stack = "params";
    } else{
        real_offset = offset;
        used_stack = "vars";
    }
    string intermediate_reg = genReg();
    string count_func_str = std::to_string(COUNT_FUNC);
    string offset_str = std::to_string(real_offset);
    emit(arg_stack + " = getelementptr inbounds i32, i32* %fun" + count_func_str + used_stack +", i32 " + offset_str);
    emit(intermediate_reg + " = load i32, i32* " + arg_stack);
    Types type_id = exp_id->type;
    if(type_id != TYPE_BOOL){
        exp_id->reg = intermediate_reg;
    }
    else{ // type is bool
        string check_condition_reg = genReg();
        string cond_check = check_condition_reg + " = icmp eq i32 0, " + intermediate_reg;
        emit(cond_check);
        string branch_missing = "br i1 " + check_condition_reg + ", label @, label @";
        int missing_branch_loc = emit(branch_missing);
        exp_id->true_list = makelist({missing_branch_loc, SECOND});
        exp_id->false_list = makelist({missing_branch_loc, FIRST});
    }
}

void create_str_expression(Expression* str_exp){
    string define_str = genString();
    string assign_str = genReg();
    str_exp->reg = assign_str;
    string val_str = str_exp->value;
    string str_actual_len = to_string(val_str.length() - 1);
    string actual_string = val_str.substr(1, val_str.length() - 2); //without quotation mark
    string llvm_create_str = define_str + " = constant [" + str_actual_len + " x i8] c\"" +
                             actual_string + "\\00\"";
    emitGlobal(llvm_create_str);
    string llvm_assign_str = assign_str + " = getelementptr [" + str_actual_len + " x i8] , ["+ str_actual_len + " x i8]* "
                             + define_str + ", i32 0, i32 0";
    emit(llvm_assign_str);
}

int create_switch_case(CaseList* case_lst, Expression* exp) { // returns the address of the switch command
    string start_switch = "switch i32 " + exp->reg + ", label ";
    string cases_code = "[ ";
    stack<CaseDecl *> stack_cases = case_lst->case_list;
    while (!stack_cases.empty()) {
        if (case_lst->has_default && stack_cases.size() == 1) {
            CaseDecl *default_case = stack_cases.top();
            stack_cases.pop();
            start_switch += "%" + default_case->quad_label + " ";
            continue;
        }
        CaseDecl *curr_case = stack_cases.top();
        stack_cases.pop();
        cases_code += "i32 " + to_string(curr_case->case_value) + ", label %" + curr_case->quad_label + " ";

    }
    if (!case_lst->has_default) {
        start_switch += "@ ";
    }
    cases_code += "]";
    string switch_code = start_switch + cases_code;
    int address = emit(switch_code);
    return address;
}
