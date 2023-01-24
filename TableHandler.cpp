

#include "TableHandler.hpp"
#include "hw3_output.hpp"
#include <iostream>
#include "assert.h"
#include "global_symbs.hpp"
using namespace std;



TableEntry* GetFromTable(string& id){

    //std:: cout << "Got Here With " << id << "  " <<  maps_v2.size() << endl;
    for (auto it = maps_v2.rbegin(); it != maps_v2.rend(); ++it){
        //std:: cout <<  "It size is: " << it->size() << endl;
        for(auto search_it = it -> begin(); search_it != it -> end(); ++search_it){
            TableEntry* temp_entry = *search_it;
            //cout << "Im here" << temp_entry -> id << endl;
            if(temp_entry -> id == id)
                return temp_entry;
        }
    }
    /*
    for (auto it = maps.rbegin(); it != maps.rend(); ++it){
        auto map_it = (*it).find(id);
        if(map_it != (*it).end()){
            return map_it -> second;
        }
    }*/

    return nullptr;
}


TableEntry* GetFunctionFromTable(string& id){
    TableEntry* res = GetFromTable(id);
    if(res == nullptr || (res -> param_types).empty())
        return nullptr;
    return res;
    /*
    for (auto it = maps_v2.rbegin(); it != maps_v2.rend(); ++it){
        for(auto search_it = it -> begin(); search_it != it -> end(); ++search_it){
            TableEntry* temp_entry = *search_it;
            if(temp_entry -> id == id){
                if((temp_entry -> param_types).empty())
                    return nullptr;
                return temp_entry;
            }
        }
    }
    for (auto it = maps.rbegin(); it != maps.rend(); ++it){
        auto map_it = (*it).find(id);
        if(map_it != (*it).end()){
            if (!map_it -> second -> param_types.empty()) // Only not empty for functions
                return map_it -> second;
            else
                return nullptr;
        }
    }*/
    return nullptr;
}


int InsertVariableToTable(string& id, Types& type){ //Variable Insertion
    if(GetFromTable(id) != nullptr)
        return 1; // Error: Shadowing

    TableEntry* new_one = new TableEntry();
    new_one -> offset = offsets.top();
    new_one -> type = type;
    new_one -> id = id;

    maps_v2.back().push_back(new_one);
    //maps.back().insert(pair<string, TableEntry*>(id, new_one)); // insert an entry to the last table
    //std::cout << "Inserted Id: "<< id << " From Type: " << type << std::endl;
    offsets.top()++;
    return 0;
}


int InsertFunctionToTable(string& id, Types& retType, vector<pair<Types, string>>& params_list){ //Function Insertion
    //string id = string(_id);
    //std::cout << "got here with " << id << std::endl;
    if(GetFromTable(id) != nullptr)
        return -1; // Error: Shadowing


    if(id.compare("main") == 0){
        if(retType == TYPE_VOID && params_list.empty()){
            isMainDefined = true;
        }
    }
    vector<Types> params; // PARAMETER TYPES
    for(auto it = params_list.begin(); it != params_list.end(); ++it){
        params.push_back(it -> first); // Hope this won't crash
    }
    if(params.empty())
        params.push_back(TYPE_VOID);


    TableEntry* entry = new TableEntry();
    entry -> offset = 0;
    entry -> type = retType;
    currentRetType = retType; // THIS IS HERE FOR HANDLING THE RETURN STATEMENT
    entry -> param_types = params;
    entry -> id = id;

    //std::cout << "made a function entry" << std::endl;
    maps_v2.back().push_back(entry);
    //maps.back().insert(pair<string, TableEntry*>(id, entry));

    //std::cout << "inserted function entry" << std::endl;

    //Starting to deal with function parameters
    CreateNewTable();
    int pos = -1;
    for(auto it = params_list.begin(); it != params_list.end(); ++it){
        string varId = it -> second;
        TableEntry* parameter_entry = new TableEntry();
        parameter_entry -> type = Types(it -> first);
        parameter_entry -> offset = pos--;
        parameter_entry -> id = varId;
        if(GetFromTable(varId) != nullptr)
            return 1 + it - params_list.begin();
        maps_v2.back().push_back(parameter_entry);
        //maps.back().insert(pair<string, TableEntry*>(varId, parameter_entry));
//        std::cout << "inserted a parameter entry id: " << string(it -> second) << " offset: " <<parameter_entry -> offset<< std::endl;
    }

    // Handling llvm side
    COUNT_FUNC = COUNT_FUNC + 1;
    string count_func_str = std::to_string(COUNT_FUNC);
    string arguments; // instantiate as empty string
    if(!(params.size() == 1 && params[0] == TYPE_VOID)){ //only if this is a function with parameters, add them
        for(int i = 0; i<params.size(); i++){
            string i_str = std::to_string(i);
            arguments += "i32 %fun" + count_func_str + "param" + i_str;
            if (i != params.size() - 1) arguments += ", ";
        }
    }
    string return_for_llvm_code;
    if(retType == TYPE_VOID){
        return_for_llvm_code = "void";
    } else{
        return_for_llvm_code = "i32";
    }
    emit("define " + return_for_llvm_code + " @" + id + "(" + arguments + ") " + "{");
    string args_stack = "%fun" + count_func_str + "params";
    string vars_stack = "%fun" + count_func_str + "vars";

    if(!(params.size() == 1 && params[0] == TYPE_VOID)) {
        emit(args_stack + " = alloca i32, i32 " + to_string(params.size())); //args stack, if there are any
    }
    emit(vars_stack + " = alloca i32, i32 50"); //assuming no more than 50 local variables
    // Inserting all params into stack
    if(!(params.size() == 1 && params[0] == TYPE_VOID)) {
        for (int i = 0; i < params.size(); i++) {
            string new_reg = genReg();
            string i_str = std::to_string(i);
            emit(new_reg + " = getelementptr inbounds i32, i32* " + args_stack + ", i32 " + i_str);
            emit("store i32 %fun" + count_func_str + "param" + i_str + ", i32* " + new_reg);
        }
    }
    // int a(int x, int a)
    return 0;
}


void CreateNewTable(){
    //maps.push_back(map<string, TableEntry*>());
    maps_v2.push_back(vector<TableEntry*>());
    offsets.push(offsets.empty() ? 0 : offsets.top());
}


void CollapseTable(){
    //assert(!maps.empty());
    //assert(offsets.size() == maps.size());
    //maps.pop_back();
    maps_v2.pop_back();
    offsets.pop();
}


void PrintDeclaredVariablesFromScope(){
    TableEntry* temp_entry;
    for(auto it  = maps_v2.back().begin(); it != maps_v2.back().end(); ++it){
        temp_entry = *it;
        if(temp_entry -> param_types.empty()){

        }
//            output::printID(temp_entry->id, temp_entry -> offset, temp_entry -> type);
        else {//THAT'S A FUNCTION
            if(*temp_entry -> param_types.begin() == TYPE_VOID)
                temp_entry -> param_types.pop_back();

            // the printing convention is () -> int for example, and not (VOID) -> int

//            output::printID(temp_entry -> id, temp_entry -> offset,
//                            output::makeFunctionType(temp_entry -> type,
//                                                     temp_entry -> param_types));

        }
//        delete temp_entry; // Because memory management is important
    }
}


void MakeGlobalScope(){
    CreateNewTable();

    // INSERTING PRINT AND PRINTI
    TableEntry* print_func = new TableEntry();
    print_func -> id = "print";
    print_func -> type = TYPE_VOID;
    print_func -> param_types.emplace_back(TYPE_STRING);
    print_func -> offset = 0;

    TableEntry* printi_func = new TableEntry();
    printi_func -> id = "printi";
    printi_func -> type = TYPE_VOID;
    printi_func -> param_types.emplace_back(TYPE_INT);
    printi_func -> offset = 0;


    maps_v2.back().push_back(print_func);
    maps_v2.back().push_back(printi_func);

    //maps.back().insert(pair<string, TableEntry*>("print", print_func));
    //maps.back().insert(pair<string, TableEntry*>("printi", printi_func));
}