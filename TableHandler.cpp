// #include "TableHandler.hpp"
// #include "hw3_output.hpp"
// #include <iostream>
// #include "assert.h"
// #include "global_symbs.hpp"
// using namespace std;

// // std::list<InnerSymbs>  copy_from_list_to_vec(std::list<InnerSymbs> our_maps){
// //     std::list<InnerSymbs>  res;

// //     return res;
// // }

// list<Symbol>* GetFromTable(string& id, std::list<InnerSymbs> our_maps){
//     // std::list<InnerSymbs> our_maps = our_maps;
//     for (auto it = our_maps.rbegin(); it != our_maps.rend(); ++it){
//         // it2 = it->symbols;
//         for(auto search_it = it->symbols.begin(); search_it != it->symbols.end(); ++search_it){
//             Symbol temp_entry = *search_it;
//             if(temp_entry.name == id)
//                 return &it->symbols;
//         }
//     }
//     return nullptr;
// }


// list<Symbol>* GetFunctionFromTable(string& id, std::list<InnerSymbs>  our_maps){
//     list<Symbol>* res = GetFromTable(id, our_maps);
//     if(res == nullptr || (res->begin()->getTypes()) == TYPE_UNDEFINED)
//         return nullptr;
//     return res;
//     /*
//     for (auto it = our_maps.rbegin(); it != our_maps.rend(); ++it){
//         for(auto search_it = it -> begin(); search_it != it -> end(); ++search_it){
//             InnerSymbs* temp_entry = *search_it;
//             if(temp_entry -> id == id){
//                 if((temp_entry -> param_types).empty())
//                     return nullptr;
//                 return temp_entry;
//             }
//         }
//     }
//     for (auto it = maps.rbegin(); it != maps.rend(); ++it){
//         auto map_it = (*it).find(id);
//         if(map_it != (*it).end()){
//             if (!map_it -> second -> param_types.empty()) // Only not empty for functions
//                 return map_it -> second;
//             else
//                 return nullptr;
//         }
//     }*/
//     return nullptr;
// }


// int InsertVariableToTable(string& id, Types& type, std::list<InnerSymbs>  our_maps){ //Variable Insertion
//     if(GetFromTable(id, our_maps) != nullptr)
//         return 1; // Error: Shadowing

//     // InnerSymbs new_one = InnerSymbs();
//     // my added to add symbol to our map NOT SURE ABOUT THE FUNCTION RETURN! (I WROTE FALSE MEANWHILE)
//     Symbol the_symb = Symbol(id, type,false ,offsets.top());
//     // new_one.addSymbol(the_symb);

//     our_maps.back().addSymbol(the_symb);
//     //maps.back().insert(pair<string, InnerSymbs*>(id, new_one)); // insert an entry to the last table
//     //std::cout << "Inserted Id: "<< id << " From Type: " << type << std::endl;
//     offsets.top()++;
//     return 0;
// }


// int InsertFunctionToTable(string& id, Types& retType, vector<pair<Types, string>>& params_list, std::list<InnerSymbs>  our_maps){ //Function Insertion
//     //string id = string(_id);
//     //std::cout << "got here with " << id << std::endl;
//     if(GetFromTable(id, our_maps) != nullptr)
//         return -1; // Error: Shadowing


//     if(id.compare("main") == 0){
//         if(retType == TYPE_VOID && params_list.empty()){
//             isMainDefined = true;
//         }
//     }
//     vector<Types> params; // PARAMETER TYPES
//     for(auto it = params_list.begin(); it != params_list.end(); ++it){
//         params.push_back(it -> first); // Hope this won't crash
//     }
//     if(params.empty())
//         params.push_back(TYPE_VOID);


//     InnerSymbs* entry = new InnerSymbs();
//     // entry -> offset = 0;
//     // entry -> type = retType;
//     // currentRetType = retType; // THIS IS HERE FOR HANDLING THE RETURN STATEMENT
//     // entry -> param_types = params;
//     // entry -> id = id;

//         // InnerSymbs new_one = InnerSymbs();
//     // my added to add symbol to our map NOT SURE ABOUT THE FUNCTION RETURN! (I WROTE FALSE MEANWHILE)
//     Symbol the_symb = Symbol(id, retType, false , 0);
//     // new_one.addSymbol(the_symb);

//     our_maps.back().addSymbol(the_symb);

//     //std::cout << "made a function entry" << std::endl;
//     // our_maps.back().push_back(entry);
//     //maps.back().insert(pair<string, InnerSymbs*>(id, entry));

//     //std::cout << "inserted function entry" << std::endl;

//     //Starting to deal with function parameters
//     CreateNewTable(our_maps);
//     int pos = -1;
//     for(auto it = params_list.begin(); it != params_list.end(); ++it){
//         string varId = it -> second;
//         InnerSymbs* parameter_entry = new InnerSymbs();

//         if(GetFromTable(varId, our_maps) != nullptr)
//             return 1 + it - params_list.begin();
//         Symbol the_symb = Symbol(varId, it -> first, false , pos--);
//         // new_one.addSymbol(the_symb);

//         our_maps.back().addSymbol(the_symb);     
//         //maps.back().insert(pair<string, InnerSymbs*>(varId, parameter_entry));
// //        std::cout << "inserted a parameter entry id: " << string(it -> second) << " offset: " <<parameter_entry -> offset<< std::endl;
//     }

//     // Handling llvm side
//     COUNT_FUNC = COUNT_FUNC + 1;
//     string count_func_str = std::to_string(COUNT_FUNC);
//     string arguments; // instantiate as empty string
//     if(!(params.size() == 1 && params[0] == TYPE_VOID)){ //only if this is a function with parameters, add them
//         for(int i = 0; i<params.size(); i++){
//             string i_str = std::to_string(i);
//             arguments += "i32 %fun" + count_func_str + "param" + i_str;
//             if (i != params.size() - 1) arguments += ", ";
//         }
//     }
//     string return_for_llvm_code;
//     if(retType == TYPE_VOID){
//         return_for_llvm_code = "void";
//     } else{
//         return_for_llvm_code = "i32";
//     }
//     emit("define " + return_for_llvm_code + " @" + id + "(" + arguments + ") " + "{");
//     string args_stack = "%fun" + count_func_str + "params";
//     string vars_stack = "%fun" + count_func_str + "vars";

//     if(!(params.size() == 1 && params[0] == TYPE_VOID)) {
//         emit(args_stack + " = alloca i32, i32 " + to_string(params.size())); //args stack, if there are any
//     }
//     emit(vars_stack + " = alloca i32, i32 50"); //assuming no more than 50 local variables
//     // Inserting all params into stack
//     if(!(params.size() == 1 && params[0] == TYPE_VOID)) {
//         for (int i = 0; i < params.size(); i++) {
//             string new_reg = genReg();
//             string i_str = std::to_string(i);
//             emit(new_reg + " = getelementptr inbounds i32, i32* " + args_stack + ", i32 " + i_str);
//             emit("store i32 %fun" + count_func_str + "param" + i_str + ", i32* " + new_reg);
//         }
//     }
//     // int a(int x, int a)
//     return 0;
// }


// void CreateNewTable(std::list<InnerSymbs>  our_maps){
//     //maps.push_back(map<string, InnerSymbs*>());
//     our_maps.push_back(InnerSymbs());
//     offsets.push(offsets.empty() ? 0 : offsets.top());
// }


// void CollapseTable(std::list<InnerSymbs>  our_maps){
//     //assert(!maps.empty());
//     //assert(offsets.size() == maps.size());
//     //maps.pop_back();
//     our_maps.pop_back();
//     offsets.pop();
// }


// void PrintDeclaredVariablesFromScope(std::list<InnerSymbs>  our_maps){
//     InnerSymbs temp_entry;
//     for(auto it  = our_maps.begin(); it != our_maps.end(); ++it){
//         temp_entry = *it;
//         // if(temp_entry.symbols.begin()->getTypes() == TYPE_UNDEFINED){

//         // }
// //            output::printID(temp_entry->id, temp_entry -> offset, temp_entry -> type);
//         if(temp_entry.symbols.begin()->getTypes() != TYPE_UNDEFINED) {//THAT'S A FUNCTION
//             if(temp_entry.symbols.begin()->getTypes() == TYPE_VOID)
//                 temp_entry.symbols.pop_back();

//             // the printing convention is () -> int for example, and not (VOID) -> int

// //            output::printID(temp_entry -> id, temp_entry -> offset,
// //                            output::makeFunctionType(temp_entry -> type,
// //                                                     temp_entry -> param_types));

//         }
// //        delete temp_entry; // Because memory management is important
//     }
// }


// void MakeGlobalScope(std::list<InnerSymbs>  our_maps){
//     CreateNewTable(our_maps);

//     // INSERTING PRINT AND PRINTI
//     // my added to add symbol to our map NOT SURE ABOUT THE FUNCTION RETURN! (I WROTE FALSE MEANWHILE)
//     InnerSymbs print_func = InnerSymbs();
//     Symbol the_symb = Symbol("print", TYPE_VOID, false , 0);
//     // print_func -> id = "print";
//     // print_func -> type = TYPE_VOID;
//     print_func.symbols.emplace_back(the_symb);
//     // print_func -> offset = 0;

//     // InnerSymbs* printi_func = new InnerSymbs();
//     // printi_func -> id = "printi";
//     // printi_func -> type = TYPE_VOID;
//     // printi_func -> param_types.emplace_back(TYPE_INT);
//     // printi_func -> offset = 0;

//     // my added to add symbol to our map NOT SURE ABOUT THE FUNCTION RETURN! (I WROTE FALSE MEANWHILE)
//     InnerSymbs printi_func = InnerSymbs();
//     Symbol the_symb2 = Symbol("print", TYPE_VOID, false , 0);
//     // print_func -> id = "print";
//     // print_func -> type = TYPE_VOID;
//     print_func.symbols.emplace_back(the_symb2);
//     // print_func -> offset = 0;


//     our_maps.push_back(print_func);
//     our_maps.push_back(printi_func);

//     //maps.back().insert(pair<string, InnerSymbs*>("print", print_func));
//     //maps.back().insert(pair<string, InnerSymbs*>("printi", printi_func));
// }