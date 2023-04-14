#include <iostream>
#include <bits/stdc++.h>

using namespace std;

#define MEM_SIZE 500*1024*1024
#define PAGE_TABLE_SIZE 1024


#define INT 4
#define CHAR 1
#define SHORTINT 2

typedef struct {
    int prev;
    int data;
    int next;
}element;
element* memory;

typedef struct{
    int data_type;  //type of data
    int type;       //type of symbol is it pointer or variable
    int size;
}dataType;


typedef struct{
    int address;
    //int scope;
    const char* id;
    dataType type;
} symbol;

symbol* STACK;
vector<symbol> symbol_table;

typedef struct {
    u_int memory_index : 32;
    u_int marked : 1; // when it is marked we can allocate it to another variable
    u_int valid : 1; // valid bit to detect segmentation fault
}page_table_entry;

vector<page_table_entry> page_table;

int number_of_blocks = MEM_SIZE*1024*1024/sizeof(element);

//The basic idea is to create a stack to remember the scope of the memory  
// the demilimter between scopes is -1 if free is run without arguments we free until we encounter a -1
// we will mark the memory so that they are to be freed when the scope is over (mark and sweep)
// we will use a linked list to keep track of the memory
// freeElem will have 3 prototypes one for specific address, one for all the scope and one for the whole list


int createMemory()
{
    memory = (element*)malloc(MEM_SIZE);
    if (memory == NULL)
    {
        cout << "Error: memory of size" <<MEM_SIZE<<"for memory allocation failed\n" << endl;
        return 1;
    }
    for(int i = 0; i < number_of_blocks; i++)
    {
        memory[i].prev = i-1;
        memory[i].data = 0;
        memory[i].next = i+1;
    }
    if(PAGE_TABLE_SIZE*1024*1024*sizeof(page_table_entry) > page_table.max_size())
    {
        cout << "Error: can't" <<PAGE_TABLE_SIZE*sizeof(page_table_entry)<<"for pagetable allocation failed\n" << endl;
        return 1;
    }
    page_table = vector<page_table_entry>(PAGE_TABLE_SIZE);
    
    for(int i = 0; i < PAGE_TABLE_SIZE; i++)
    {
        page_table[i].marked = 0;
        page_table[i].valid = 0;
    }

    return 0;
}

void initScope()
{
    symbol* temp = new symbol;
    temp->address = -1;
    symbol_table.push_back(*temp);

}

int createList(const char* id,int data_type ,ssize_t size){

}

int createVar(int data_type)
{

}