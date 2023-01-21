#include "global_symbs.hpp"
#include "hw3_output.hpp"
#include <iterator>

GlobalSymbs::GlobalSymbs()
{
    symbolTables = std::list<InnerSymbs>();
    offset = stack<int>();
    offset.push(0);
    in_while = 0;
    current_function_offset=-1;
   // std::cout<<"GlobalSymbs()"<< std::endl;
}

bool GlobalSymbs::isExist(std::string id)
{
    //std::cout<<"isExist()"<< std::endl;
    for (InnerSymbs iner_symb : symbolTables)
    {
        for (Symbol symb : iner_symb.getEntries())
        {
            if (symb.getName().compare(id) == 0)
            {
                return true;
            }
        }
    }
    return false;
}
Types GlobalSymbs::findID(std::string id)
{
    //std::cout<<"isExist()"<< std::endl;
    for (InnerSymbs iner_symb : symbolTables)
    {
        for (Symbol symb : iner_symb.getEntries())
        {
            if (symb.getName().compare(id) == 0)
            {
                return symb.type;
            }
        }
    }
    output::errorUndef(yylineno, id);
    exit(0);
    return TYPE_UNDEFINED;
}
void GlobalSymbs::checkVariables(std::string id, ExpressionList* El)
{

    vector<std::string> argtypes;
    for(Function f : all_functions ){
        if (f.name == id)
        {   
            if (El == nullptr && f.symbols.size() != 0)
            {
                for(Symbol sym: f.symbols){
                //std::cout<<"printFunctions()3"<< std::endl;
                argtypes.emplace_back(this->typeToString(sym.type));
                }
                output::errorPrototypeMismatch(yylineno, id, argtypes);
                exit(0);
            }
            if (El == nullptr)
            {
                return;
            }
            // std::cout << El->exp_list.size() << std::endl;
            // std::cout << f.symbols.size() << std::endl;
            if (El->exp_list.size() != f.symbols.size())
            {
                for(Symbol sym: f.symbols){
                    //std::cout<<"printFunctions()3"<< std::endl;
                    argtypes.emplace_back(this->typeToString(sym.type));
                }
                output::errorPrototypeMismatch(yylineno, id, argtypes);
                exit(0);
            }
            for(int i=0; i < El->exp_list.size(); i++){

                if (El->exp_list[i]->type != f.symbols[i].type)
                {
                    if ((f.symbols[i].type == TYPE_INT && El->exp_list[i]->type == TYPE_BYTE))
                    {
                        continue;
                    }
                    for(Symbol sym: f.symbols){
                        //std::cout<<"printFunctions()3"<< std::endl;
                        // std::cout << this->typeToString(El->exp_list[i]->type) << std::endl;
                        argtypes.emplace_back(this->typeToString(sym.type));
                    }
                    output::errorPrototypeMismatch(yylineno, id, argtypes);
                    exit(0);
                }
            }
        }
    }
    

    
}
void GlobalSymbs::checkFunctionType(Types type)
{
    
}
void GlobalSymbs::checkIfBool(Types type)
{
    
}
void GlobalSymbs::addSymbol(Types type,std::string name)
{
    //std::cout<<"addSymbol()"<< std::endl;
    if (isExist(name))
    {
        output::errorDef(yylineno, name);
        exit(0);
    }
    this->symbolTables.back().getEntries().push_back(Symbol(name, type, false,this->getOffset()));//needs fix
    this->offset.top()++;
    //std::cout<<"symbol " << name << "added"<< std::endl;
}

bool GlobalSymbs::checkInWhile()
{
    //std::cout<<"checkInWhile()"<< std::endl;
    return in_while>0;
}

Types GlobalSymbs::getVarType(std::string id){
    //std::cout<<"getVarType()"<< std::endl;
    for (InnerSymbs iner_symb : symbolTables)
    {
        for (Symbol symb : iner_symb.getEntries())
        {
            if (symb.getName().compare(id) == 0)
            {
                return symb.type;
            }
        }
    }
    return TYPE_UNDEFINED;
}

Types GlobalSymbs::getFunctionType(std::string id){
    //std::cout<<"getFunctionType()"<< std::endl;
    for(Function fun: this->all_functions)
    {
        if(fun.name==id)
        {
            return fun.return_type;
        }
    }
    return TYPE_UNDEFINED;
}

void GlobalSymbs::addFunction(std::string name, Types type)
{
    //std::cout<<"addFunction()"<< std::endl;
    //create new Function with current_function_parameters
    //add to all_functions
    //std::cout<<"addFunction()"<< type<< std::endl;
    Function new_f(name, type);
    while(!current_function_parameters.empty())
    {
        //std::cout<<current_function_parameters.back().getName()<< std::endl;
        new_f.add_symbol(current_function_parameters.back());
        current_function_parameters.pop_back();
    }
    all_functions.emplace_back(new_f);
}
void GlobalSymbs::enterWhile()
{
    in_while++;
}
void GlobalSymbs::exitWhile()
{
    in_while--;
}
void GlobalSymbs::openScope()
{
    //std::cout<<"openScope()"<< std::endl;
    //add InnerSymbol entry to symbolTables
    //add int to offset stack(value should be equal to last value)
    InnerSymbs new_is;
    symbolTables.emplace_back(new_is);
    offset.emplace(offset.top());
}
void GlobalSymbs::compareTypesAssignment(Types assigned_to, Types assigned_from){
    if (assigned_to == assigned_from || (assigned_to == TYPE_INT && assigned_from ==TYPE_BYTE))
    {
        return;
    }  
    output::errorMismatch(yylineno);
    exit(0);
}

void GlobalSymbs::compareRelop(Types assigned_to, Types assigned_from){
    if (assigned_to==TYPE_INT && assigned_from==TYPE_INT)
    {
        return;
    }  
    output::errorMismatch(yylineno);
    exit(0);
}

void GlobalSymbs::closeScope()
{
    //std::cout<<"closeScope()"<< std::endl;
    //remove last InnerSymbol entry
    //remove last in from offset stack
    symbolTables.back().printAllSymbs();
    symbolTables.pop_back();
    offset.pop();
}
void GlobalSymbs::addFormal(Types type, std::string name)
{
    //std::cout<<"addFormal()"<< std::endl;
    //add Formal to current_function_parameters
    Symbol new_s(name, type, false,current_function_offset--);
    this->symbolTables.back().getEntries().push_back(new_s);
    current_function_parameters.emplace_back(new_s);
}
void GlobalSymbs::clearFormals()
{
    //empty current_function_parameters
    //std::cout<<"clearFormals()"<< std::endl;
    current_function_parameters.clear();
    current_function_offset=-1;
}
void GlobalSymbs::currentFunctionType(Types type)
{
    current_function_type=type;
}

void GlobalSymbs::comparesTypesCast(Types first,Types second)
{
    //std::cout<<"compareTypesCast()"<< std::endl;
    //check if second type can be cast into first type
    if(first==second) return;
    if(second==TYPE_BYTE && first==TYPE_INT) return;
    if(second==TYPE_INT && first==TYPE_BYTE) return;
    output::errorMismatch(yylineno);
    exit(0);
}
int GlobalSymbs::getOffset()
{
    return offset.top();
}
void GlobalSymbs::printFunctions()
{
    
    //std::cout<<"printFunctions()"<< std::endl;
    for(Function fun: this->all_functions)
    {
        //std::cout<<"printFunctions()2"<< std::endl;
        vector<std::string> argtypes;
        for(Symbol sym: fun.symbols){
            //std::cout<<"printFunctions()3"<< std::endl;
            argtypes.emplace_back(this->typeToString(sym.type));
        }
        output::printID(fun.name,0,output::makeFunctionType(this->typeToString(fun.return_type),argtypes));
    }
}
string GlobalSymbs::typeToString(Types type){
    //std::cout<<"typeToString()"<< std::endl;
    //std::cout<<"typeToString()"<< type << std::endl;
    switch(type){
            case TYPE_BOOL: return "BOOL"; break;
            case TYPE_BYTE: return "BYTE"; break;
            case TYPE_INT: return "INT"; break;
            case TYPE_STRING: return "STRING"; break;
            case TYPE_VOID: return "VOID";break;
            default: return "ERROR";
        }
}
// add a function for "while" counter