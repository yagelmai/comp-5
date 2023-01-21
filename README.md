this directory should solve hw3 in compliation and create compiler of FanC languge.

# global_symbs #

we should have one DS that i call him [global_symbs](global_symbs.hpp) is should hold all the global symbols. 
in global val we have 3 classes:
1. GlobalSymbs
2. InnerSymbs
3. Symbol
## GlobalSymbs ##
contain all the symbols with the next attributes:

    1. std::list<InnerSymbs> symbolTables //the list of lists of symbols 
    2. std::stack<int> offset;            //stack containthe offset of every symbol
    3. static bool in_while;              //when we out of the last while its false
the hpp file contain the **DS** and the cpp file contain the **constructor of the DS** and the **actions** we can do before reduce.
### symbol Tables ###
contain list of inner symbols. that represent all the inner symbols (inner symbols - the symbols of one scope).

### inner symbols ###
contain vector of symbols. this variable represent all the symbols inside one scope.

### symbols ###
class that represent symbols this class holds:

    string name;        // the name of the symbol
    Types type;         // the type of the symbol
    string value;       // the value of the symbol
    int offset;         // the offset of the symbol
    bool isFunc;        // if the symbol is function its true, otherwise its false
for example for the next 3 line:
1. int a = 5;
2. float b;
3. int func(FORMALS)

we will create 3 classes and store them inside symbols like this:
1. name = a, type = INT, vale = 5, offset = 1, isFunc = false.
2. name = b, type = FLOAT, vale = undefined, offset = 2, isFunc = false.
3. name = func, type = int, vale = FORMALS, offset = -1, isFunc = true.
when FORMALS represent the content of the function.

# parser.hpp #
contain all the classes of the semantic we need (Number, statement, etc...) they all Inheriting from Node when Node is contain value and type.