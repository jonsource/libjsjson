#include "libjsjson.h"
#include "lib/jslib.h"

using namespace std;
using namespace jslib;

void (*dbf)(string,int) = NULL;
char buf[128];

string Node::serialize(){ return "null";};
void Node::add(Node *){ return; };
void Node::add(string, Node *){return;};
int Node::getType(){ return N_NONE;}
void * Node::getValue() { return NULL; }
bool Node::isInt() { return false; }
bool Node::isString() { return false; }
bool Node::isArray() { return false; }
bool Node::isObject() { return false; }
bool Node::isFloat() { return false; }
int Node::getInt() { return NULL; }
string Node::getString() { return NULL; }
NObject * Node::getObject() { return NULL; }
NArray * Node::getArray() { return NULL; }
double Node::getFloat() { return NULL; }
bool Node::hasMember(int) { return false; };
bool Node::hasMember(string) { return false; };
Node* Node::getMember(int) { return NULL; };
Node* Node::getMember(string) { return NULL; };

ObjectNode::ObjectNode()
{
    value = new NObject;
}
ObjectNode::ObjectNode(string in) {
    JsonReader jr;
    Node * n = jr.parse(&in);
    if(n->getType()!=OBJECT)
    {   debug("Bad JSON object definition :"+in);
        exit(1);
    }
    value = n->getObject();
    delete n;
}
void ObjectNode::add(string key, Node *n) {
    (*value)[key]=n;
}
string ObjectNode::serialize() {
    string ret = "{ ";
    bool first = true;
    for(NOi iterator = value->begin(); iterator != value->end(); iterator++) {
        if(first) { first=false;}
        else ret+=", ";
        ret += "\""+iterator->first+"\": "+iterator->second->serialize();
    }
    ret+=" }";
    return ret;
}
int ObjectNode::getType()
{ return N_OBJECT;}
NObject * ObjectNode::getObject() {
    return value;
}
bool ObjectNode::isObject() { return true; }
bool ObjectNode::hasMember(string s) { if(value->count(s)) return true; else return false;}
Node * ObjectNode::getMember(string s)
{   if(!hasMember(s))
    { debug("ObjectOverflow, no member "+s+".");
      exit(1);
    }
    return (*value)[s];
}


ArrayNode::ArrayNode() { value = new NArray; }
ArrayNode::ArrayNode(string in) {
    JsonReader jr;
    Node * n = jr.parse(&in);
    if(n->getType()!=ARRAY)
    {   debug("Bad JSON array definition :"+in);
        exit(1);
    }
    value = n->getArray();
    delete n;
}
void ArrayNode::add(Node *n) {
    value->push_back(n);
}
string ArrayNode::serialize() {
    string ret = "[ ";
    bool first = true;
    for(NAi iterator = value->begin(); iterator != value->end(); iterator++) {
        if(first)
        {   first=false;
            ret += (*iterator)->serialize();
        }
        else ret += ", "+(*iterator)->serialize();
    }
    ret+=" ]";
    return ret;
}
int ArrayNode::getType() { return N_ARRAY;}
NArray * ArrayNode::getArray() { return value; }
bool ArrayNode::isArray() { return true; }
bool ArrayNode::hasMember(int i) {
    if(i>=0 && i<value->size()) return true;
    else return false;
}
Node * ArrayNode::getMember(int i) {
    if(!hasMember(i))
    { debug("ArrayOverflow "+to_str(i)+".");
      exit(1);
    }
    return (*value)[i];
}

StringNode::StringNode(string s) { value=s; }
string StringNode::serialize() {
    return "\""+implode("\\\"",jslib::explode("\"",value))+"\"";
}
int StringNode::getType() { return N_STRING;}
string StringNode::getString() { return value; }

IntNode::IntNode(string s) { value = load_int(s); }
IntNode::IntNode(int s) { value = s; }
string IntNode::serialize() {
    sprintf(buf,"%d",value);
    return string(buf);
}
int IntNode::getType() { return N_INT; }
int IntNode::getInt() { return value; }


FloatNode::FloatNode(string s) { value = load_double(s); }
FloatNode::FloatNode(double s) { value = s; }
string FloatNode::serialize() {
    sprintf(buf,"%f",value);
    return string(buf);
}

int FloatNode::getType() { return N_FLOAT; }
double FloatNode::getFloat() { return value; }

bool JsonReader::isNum(char c) { return (c>='0' && c<='9'); }
bool JsonReader::isAlpha(char c) { return (c>='a' && c<='Z'); }
bool JsonReader::isQuote(char c) { return (c==qt); }
bool JsonReader::isWhite(char c) { return (c==' ' || c=='\n' || c=='\t'); }
char JsonReader::getChar()
{   if(readingFile) { return fgetc(file); }
    else {  //debug("\nstrpos "+to_str(strpos)+" "+to_str((*input)[strpos])+" len "+to_str((int)(input->length())));
            if(strpos<input->length()) return (*input)[strpos++];
            else return EOF;
    }
}
void JsonReader::stepBack()
{   --car;
    if(readingFile) { fseek(file,-1,SEEK_CUR); }
    else {  --strpos; }
}

void JsonReader::useSingleQuotes() { qt = '\''; }
void JsonReader::useDoubleQuotes() { qt = '"'; }

Node* JsonReader::read(string fn)
{
    file=fopen(fn.c_str(),"r");
    if(file==NULL)
    {   debug("Cound't open file "+fn);
        exit(1);
    }
    readingFile=true;
    newline = true;
    ln = 0;
    car = 0;
    return readChars(ROOT,START);
}
Node* JsonReader::parse(string *s)
{   input = s;
    readingFile = false;
    //newline = true;
    ln = 0;
    car = 0;
    return readChars(ROOT,START);
}

Node* JsonReader::readChars(int type,int state)
{   string str1;
    string str2;
    Node * ret = NULL;
    char line[512];

    //debug("\nim in "+to_str(type)+" "+to_str(state));
    char c;

    while((c = getChar()) != EOF)
    {   str2 = c;
        //debug("\n("+str2+")            ... line "+to_str(ln)+" char "+to_str(car)+"   "+to_str(type)+":"+to_str(state));
        car++;
        if(newline)
        {   if(c=='#')
            {   readChars(COMMENT,START);
                continue;
            }
        }

        if(c=='\n')
        {   newline=true;
            ln++;
            car = 0;
            if(type==COMMENT)
            {
                //debug("\nend comment line "+to_str(ln)+" char "+to_str(car)+" ("+c+")");
                return NULL;
            }

        }
        else
        {   newline=false;

        }

        if(type==ROOT)
        {   if(state== START)
            {   if(c==' ' || c=='\n' || c=='\t') continue;
                if(c!='[' && c!='{')
                {   debug("Error parsing JSON root, line "+to_str(ln)+" char "+to_str(car)+" ("+c+")",10);
                    exit(1);
                }
                else
                {   if(c=='[') { ret = readChars(ARRAY,START); }
                    else ret = readChars(OBJECT,START);
                }
            }
        }
        else
        {   if(type == ARRAY)
            {   if(state == START)
                {   ret = new ArrayNode();
                    state = MIDDLE;
                }
                if(state == MIDDLE || state==MIDDLE_NEXT)
                {   if(c==' ' || c=='\n' || c=='\t') continue;
                    if(c==',')
                    {   if(state==MIDDLE_NEXT)
                        {   debug("Error parsing JSON array, line "+to_str(ln)+" char "+to_str(car)+" ("+c+")",10);
                            exit(1);
                        }
                        else
                        {   state=MIDDLE_NEXT;
                            continue;
                        }
                    }
                    state=MIDDLE;
                    if(c==']') return ret;
                    else if(c==qt) ret->add(readChars(STRING,START));
                        else if(c=='[') ret->add(readChars(ARRAY,START));
                            else if(c=='{') ret->add(readChars(OBJECT,START));
                                else if(c=='n') ret->add(readChars(NLL,START));
                                    else
                                    {   stepBack();
                                        ret->add(readChars(NUMBER,START));
                                    }
                }
            }
            if(type == OBJECT)
            {   Node * key;
                Node * val;
                if(state == START)
                {   ret = new ObjectNode();
                    state = KEY;
                }
                if(state == KEY)
                {   if(c==' ' || c=='\n' || c=='\t') continue;
                    if(c==qt) key = readChars(STRING,START);
                    else key = readChars(NUMBER,START);
                    //debug("\nkey "+key->serialize());
                    state = KEY_AFTER;
                    continue;
                }
                if(state == KEY_AFTER)
                {   if(c==' ' || c=='\n' || c=='\t') continue;
                    if(c!=':')
                    {   debug("Error parsing JSON key:value pair, : expected , line "+to_str(ln)+" char "+to_str(car)+" ("+c+")",10);
                        exit(1);
                    }
                    state = VAL;
                    continue;
                }
                if(state == VAL)
                {   if(c==' ' || c=='\n' || c=='\t') continue;
                    if((c<'0' || c>'z') && (c!=qt && c!='[' && c!='[' && c!='-'))
                    {   debug("Error parsing JSON key:value pair, value expected , line "+to_str(ln)+" char "+to_str(car)+" ("+c+")",10);
                        exit(1);
                    }
                    else if(c==qt) val = readChars(STRING,START);
                        else if(c=='[') val = readChars(ARRAY,START);
                            else if(c=='{') val = readChars(OBJECT,START);
                                else if(c=='n') val = readChars(NLL,START);
                                    else
                                    {   stepBack();
                                        val = readChars(NUMBER,START);
                                    }
                    //debug("\npair - "+key->serialize()+":"+val->serialize());
                    ret->add(key->getString(),val);
                    state = VAL_AFTER;
                    continue;
                }
                if(state == VAL_AFTER)
                {   if(c==' ' || c=='\n' || c=='\t') continue;
                    if(c==',')
                    {   state = KEY;
                        continue;
                    }
                    else if(c=='}')
                        {
                            return ret;
                        }
                        else
                        {   debug("Error parsing JSON object, end or key:value expected , line "+to_str(ln)+" char "+to_str(car)+" ("+c+")",10);
                            exit(1);
                        }
                }


            }
            if(type == NLL)
            {
                if((state==START && c!='u') || (state==MIDDLE && c!='l') || (state==MIDDLE_NEXT && c!='l'))
                {   debug("Error parsing JSON null token, line "+to_str(ln)+" char "+to_str(car)+" ("+c+")",10);
                    exit(1);
                }
                if(state==MIDDLE_NEXT)
                {   return new Node;

                }
                if(state == MIDDLE) state=MIDDLE_NEXT;
                if(state == START) state=MIDDLE;
                continue;
            }
            if(type == NUMBER || type == FLOAT)
            {   //debug("\nparsing "+to_str((char)c)+" in number");
                if(state == START)
                {   if(!isNum(c) && !isWhite(c) && !(c=='-' || c==',' || c=='.' || c==']' || c=='}'))
                    {   debug("Error parsing JSON number, line "+to_str(ln)+" char "+to_str(car)+" ("+c+")",10);
                        exit(1);
                    }
                }
                else
                {   if(!isNum(c) && !isWhite(c) && !(c==',' || c=='.' || c==']' || c=='}'))
                    {   debug("Error parsing JSON number, line "+to_str(ln)+" char "+to_str(car)+" ("+c+")",10);
                        exit(1);
                    }
                }
                if(state==START) { str1=""; state = MIDDLE; }
                if(c=='.' && type==FLOAT)
                {   debug("Error parsing JSON float, line "+to_str(ln)+" char "+to_str(car)+" ("+c+")",10);
                    exit(1);
                }
                if(c=='.') { type=FLOAT; }
                if(c==',' || c==']' || c=='}' || c==' ' || c=='\n' || c=='\t') {
                    //debug("\nfinal number "+str1);
                    stepBack();
                    if(type==FLOAT) return new FloatNode(str1);
                    else return new IntNode(str1);
                }
                str1=str1+c;

            }
            if(type == STRING)
            {   if(state==START) { str1=""; state = MIDDLE; }
                if(state==ESCAPE)
                {   state=MIDDLE;
                    if(c=='n') str1+='\n';
                    if(c=='t') str1+='\t';
                    if(c==qt) str1+=qt;
                    if(c=='\\') str1+='\\';
                }
                else
                {   if(c=='\\')
                    {   state=ESCAPE;
                    }
                    else
                    {
                        if(c==qt)
                        {   //debug("\nfinal string "+str1);
                            return new StringNode(str1);
                        }
                        str1=str1+c;
                    }
                }
            }

        }

    }
    return ret;
}
