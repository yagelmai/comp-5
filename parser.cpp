#include "parser.hpp"

static list<Table *> symsTabs = list<Table *>();

EntryT *getEntryT(string id)
{
    for (Table *t : symsTabs)
    {
        for (EntryT *entry : t->getEntries())
        {
            if (! (entry->getName().compare(id)))
            {
                return entry;
            }
        }
    }
    return nullptr;
}


FormalDecl::FormalDecl(Type *type, Id *symbol)
{
    this->value = symbol->value;
    this->type = type->type;
}

FormalsList::FormalsList(FormalDecl *f_dec)
{
    this->type = TYPE_UNDEFINED;
    this->declaration.emplace_back(f_dec);
    this->value = "This is a formals list";
}

FormalsList::FormalsList(FormalDecl *f_dec, FormalsList *f_list)
{
    this->declaration = f_list->declaration;
    this->type = f_list->type;  
    this->declaration.insert(declaration.begin(), f_dec);
    this->value = f_list->value;
}


Formals::Formals(FormalsList *f_list)
{
    this->value = "this is Formals";
    this->declaration = f_list->declaration;
    this->type = TYPE_UNDEFINED;
}
