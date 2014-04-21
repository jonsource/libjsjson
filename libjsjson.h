#ifndef JSON_H_INCLUDED
#define JSON_H_INCLUDED

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>

#define ROOT 0
#define OBJECT 1
#define ARRAY 2
#define STRING 3
#define NUMBER 4
#define FLOAT 5
#define COMMENT 6
#define NLL 7

#define START 1
#define MIDDLE 2
#define ESCAPE 3
#define MIDDLE_NEXT 4
#define KEY 5
#define KEY_AFTER 6
#define VAL 7
#define VAL_AFTER 8


#define N_NONE 0
#define N_OBJECT 1
#define N_ARRAY 2
#define N_STRING 3
#define N_INT 4
#define N_FLOAT 5

class Node;

typedef std::map< std::string, Node *> NObject;
typedef NObject::iterator NOi;
typedef std::vector< Node *> NArray;
typedef NArray::iterator NAi;

class Node {
    public:

        virtual std::string serialize();
        virtual void add(Node *);
        virtual void add(std::string, Node *);
        virtual int getType();
        virtual void * getValue();
        virtual bool isInt();
        virtual bool isString();
        virtual bool isArray();
        virtual bool isObject();
        virtual bool isFloat();
        virtual int getInt();
        virtual std::string getString();
        virtual NObject * getObject();
        virtual NArray * getArray();
        virtual double getFloat();
        virtual bool hasMember(int);
        virtual bool hasMember(std::string);
        virtual Node * getMember(int);
        virtual Node * getMember(std::string);
};

class JsonReader {
    private:
        FILE *file;
        int strpos=0;
        bool newline;
        std::string *buff;
        int ln;
        int car;
        std::string *input;
        bool readingFile=true;
        char qt = '"';
        bool isNum(char c);
        bool isAlpha(char c);
        bool isQuote(char c);
        bool isWhite(char c);
        char getChar();
        void stepBack();
        Node* readChars(int type,int state);
    public:
        void useSingleQuotes();
        void useDoubleQuotes();
        Node * read(std::string fn);
        Node * parse(std::string *s);

};

class ObjectNode: public Node {
    public:
        NObject *value;
        ObjectNode();
        ObjectNode(std::string in);
        void add(std::string key, Node *n);
        std::string serialize();
        int getType();
        NObject *getObject();
        bool isObject();
        bool hasMember(std::string s);
        Node * getMember(std::string s);
};

class ArrayNode: public Node {
    public:
        NArray * value;
        ArrayNode();
        ArrayNode(std::string in);
        void add(Node *n);
        std::string serialize();
        int getType();
        bool isArray();
        bool hasMember(int i);
        Node * getMember(int i);
        NArray * getArray();
};

class StringNode: public Node {
    public:
        StringNode(std::string s);
        std::string value;
        std::string serialize();
        int getType();
        std::string getString();
};

class IntNode: public Node {
    public:
        IntNode(std::string s);
        IntNode(int s);
        int value;
        std::string serialize();
        int getType();
        int getInt();
};

class FloatNode: public Node {
    public:
        FloatNode(std::string s);
        FloatNode(double s);
        double value;
        std::string serialize();
        int getType();
        double getFloat();
};

#endif // JSON_H_INCLUDED
