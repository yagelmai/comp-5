

// #ifndef COMPI3_TABLEHANDLER_H
// #define COMPI3_TABLEHANDLER_H

// #include <string>
// #include <vector>
// #include <map>
// #include <stack>
// #include "hw3_output.hpp"
// #include "bp.hpp"
// #include "trans_llvm.hpp"
// #include "global_symbs.hpp"


// using namespace std;

// class TableEntry{
//     public:
//         string id;
//         Types type;
//         int offset;
//         vector<Types> param_types; // use only if this is a function entry.
//         // If a function gets no parameters, this will consist of only "void"
// };

// // int, float, bool -> int


// //stack<size_t> offsets;
// //vector<vector<TableEntry*>> maps_v2;

// extern stack<size_t> offsets;
// // extern vector<vector<TableEntry*>> maps_v2; // Scopes list


// //extern vector<map<string, TableEntry*>> maps;
// extern int currentFunctionSituation;
// extern Types currentRetType; // For handling return statements
// extern int whileLoopCounter; // For handling nested while loops and check for continue/break;
// extern int switchCounter; // For handling nested switch and check for break;
// extern bool isMainDefined;



// // list<Symbol>* GetFromTable(string&, std::list<InnerSymbs>  our_maps);
// // int InsertVariableToTable(string&, string&, std::list<InnerSymbs>  our_maps);
// // int InsertFunctionToTable(string&, Types&, vector<pair<string, string>>&, std::list<InnerSymbs>  our_maps);
// // list<Symbol>* GetFunctionFromTable(string&, std::list<InnerSymbs>  our_maps);
// // void CollapseTable(std::list<InnerSymbs>  our_maps);
// // void PrintDeclaredVariablesFromScope(std::list<InnerSymbs>  our_maps);
// // void CreateNewTable(std::list<InnerSymbs>  our_maps);
// // void MakeGlobalScope(std::list<InnerSymbs>  our_maps);


// #endif //COMPI3_TABLEHANDLER_H
