#include <iostream>
#include "libjsjson.h"

using namespace std;

void testOutp(string str, bool f)
{   Node * root;
    JsonReader jr;
    if(f) root = jr.read(str);
    else {
        jr.useSingleQuotes();
        root = jr.parse(&str);
    }

    if(root->isArray())
    {   for(int i=0; i<10; i++)
        {   if(root->hasMember(i)) printf("\nRoot has member %d of type %d",i,root->getMember(i)->getType());
        }
    }
    else
    {   NObject * o = root->getObject();
        for(NOi it = o->begin(); it!=o->end(); it++)
        {   int type=it->second->getType();
            printf("\nRoot has member %s of type %d",it->first.c_str(),type);
            if(type == N_ARRAY || type==N_OBJECT) { printf("\n  ** %s",it->second->serialize().c_str()); }
            if(type == N_INT) { printf("\n  ** %d",it->second->getInt()); }
            if(type == N_FLOAT) { printf("\n  ** %f",it->second->getFloat()); }
            if(type == N_STRING) { printf("\n  ** %s",it->second->getString().c_str()); }
            string s = it->first;
            //printf("\n  *-* %s %s %s", s.c_str(),string("g").c_str(), root->getMember(s)->serialize().c_str());
            //printf("\n  *-* %s %d", s.c_str(), root->hasMember(string("g")));
        }
    }
}

void testInp()
{   Node * root = new ArrayNode;
    Node * obj = new ObjectNode;
    obj->add("prvni",new StringNode("lala"));
    obj->add("druhy",new StringNode("tratra"));
    root->add(obj);
    root->add(new FloatNode(2.3));
    root->add(new ArrayNode("[3,8]"));
    root->add(new ObjectNode("{\"smoulu\":8}"));
    printf("\n%s",root->serialize().c_str());
}

int main()
{
    testOutp("pokus.json",true);
    testInp();
    //testOutp("[ 2, {'rate':2,'frame':'pet','ichor':[3,4]} ]",false);
    return 0;
}
