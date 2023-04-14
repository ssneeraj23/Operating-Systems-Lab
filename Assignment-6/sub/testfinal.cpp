// C++ program for Merge Sort
#include <iostream>
#include <bits/stdc++.h>
using namespace std;

#define n 1000
#define MEM_SIZE 500 * 1024 * 1024
#define PAGE_SIZE 2048             // page size
#define PAGE_TABLE_SIZE 250 * 1024 // total number of pages in memory

// 170 elements + 8 bytes wastage in a page
typedef struct
{
    int prev;
    int data;
    int next;
} element;

typedef struct
{
    string id;
    vector<int> pages_allocated;
    // int start_page;
    // int size;
} entry;

element *mem_start;
vector<int> page_table(PAGE_TABLE_SIZE, 1);
int no_of_free_pages = PAGE_TABLE_SIZE;
vector<entry> allocation_stack;
vector<entry> global_stack;

// The basic idea is to create a stack to remember the scope of the memory
//  the demilimter between scopes is -1 if free is run without arguments we free until we encounter a -1
//  we will mark the memory so that they are to be freed when the scope is over (mark and sweep)
//  we will use a linked list to keep track of the memory
//  freeElem will have 3 prototypes one for specific address, one for all the scope and one for the whole list



int createMemory(int size)
{
    // dynamic size to be considered
    mem_start = (element *)malloc(MEM_SIZE);
    if (mem_start == NULL)
    {
        cout << "Error: memory of size" << MEM_SIZE << "for memory allocation failed\n"
             << endl;
        return 1;
    }

    return 0;
}

void initScope()
{
    entry temp;
    temp.id = "--";
    temp.pages_allocated.clear();
    allocation_stack.push_back(temp);
    return;
}

int createList(string name, int size, int isglobal = 0)
{
    int pages_needed = (size / 170) + 1;
    if (pages_needed > no_of_free_pages)
    {
        cout << "Error: Not enough memory to allocate" << endl;
        return -1;
    }
    entry temp;
    temp.id = name;
    temp.pages_allocated.clear();
    no_of_free_pages -= pages_needed;
    for (int i = 0; i < PAGE_TABLE_SIZE; ++i)
    {
        if (page_table[i] == 1)
        {
            --pages_needed;
            page_table[i] = 0;
            temp.pages_allocated.push_back(i);
            if (pages_needed == 0)
                break;
        }
    }
    // for(int i=0;i<temp.pages_allocated.size();++i)
    //{
    //  for(int offset=0;j<SIZE_OF_PAGE/sizeof(element);++j)
    //   {
    //  int index_in_mem = 2048*pages_allocated[i]+offset;
    //   }
    // }
    if (isglobal == 1)
        global_stack.push_back(temp);
    else
        allocation_stack.push_back(temp);
    return 1; // depicting success
}

int readVal(string listname, int offset, int isglobal = 0)
{
    vector<int> pages;
    if (isglobal == 1)
    {
        for (int x = global_stack.size() - 1; x>=0; x--)
        {
            if (global_stack[x].id == listname)
            {
                pages = global_stack[x].pages_allocated;
                break;
            }
        }

        if (pages.size() == 0)
        {
            cout << "Error: List not found in this scope" << endl;
            return -1;
        }
    }
    else
    {
        for (int x = allocation_stack.size() - 1; allocation_stack[x].id != "--"; x--)
        {
            if (allocation_stack[x].id == listname)
            {
                pages = allocation_stack[x].pages_allocated;
                break;
            }
        }

        if (pages.size() == 0)
        {
            cout << "Error: List not found in this scope" << endl;
            return -1;
        }
    }

    int contained_page = (offset / 170);
    offset %= 170;
    contained_page = pages[contained_page];
    char *start_ad = (char *)mem_start;
    start_ad = start_ad + (contained_page)*2048;
    element *page_start = (element *)start_ad;
    element *req_element = page_start + offset;
    return req_element->data;
}

int assignVal(string listname, int offset, int value,int isglobal=0)
{
    vector<int> pages;

    if (isglobal == 1)
    {
        for (int x = global_stack.size() - 1; x>=0; x--)
        {
            if (global_stack[x].id == listname)
            {
                pages = global_stack[x].pages_allocated;
                break;
            }
        }

        if (pages.size() == 0)
        {
            cout << "Error: List not found in this scope" << endl;
            return -1;
        }
    }
    else
    {
        for (int x = allocation_stack.size() - 1; allocation_stack[x].id != "--"; x--)
        {
            if (allocation_stack[x].id == listname)
            {
                pages = allocation_stack[x].pages_allocated;
                break;
            }
        }

        if (pages.size() == 0)
        {
            cout << "Error: List not found in this scope" << endl;
            return -1;
        }
    }

    int contained_page = (offset / 170);
    offset %= 170;
    contained_page = pages[contained_page];
    char *start_ad = (char *)mem_start;
    start_ad = start_ad + (contained_page)*2048;
    element *page_start = (element *)start_ad;
    element *req_element = page_start + offset;
    req_element->data = value;
    return 1; // to indicate sucsess
}

void freeElem(string id = "?")
{
    if (id == "?")
    {
        for (int x = allocation_stack.size() - 1; allocation_stack[x].id != "--"; x--)
        {
            for(auto y:allocation_stack[x].pages_allocated)
            {
                page_table[y]=1;
                ++no_of_free_pages;
            }
            allocation_stack.pop_back();
        }
        allocation_stack.pop_back();
        return;
    }
    for (int x = allocation_stack.size() - 1; allocation_stack[x].id != "--"; x--)
    {
        if (allocation_stack[x].id == id)
        {
            for(auto y:allocation_stack[x].pages_allocated)
            {
                page_table[y]=1;
                ++no_of_free_pages;
            }
            allocation_stack.erase(allocation_stack.begin()+x);
            return;
        }
    }
}


// createList("temp", n);

void merge( int const left1, int const mid, int const right1)
{
    initScope();
    auto const left = mid - left1 + 1;
    auto const right = right1 - mid;
    printf("left : %d mid : %d right: %d\n", left1, mid, right1);

    // Create temp arrays
    createList("leftArray", left);
    createList("rightArray", right);

    // Copy data to temp arrays leftArray[] and rightArray[]
    for (auto i = 0; i < left; i++){
        int input_left_i=readVal("input",left1+i,1);
        assignVal("leftArray",i,input_left_i);

    }

    printf("Left Array : \n");
    for(int i=0; i<left ; i++){
        printf("%d\n", readVal("leftArray",i));
    }
        
    for (auto j = 0; j < right; j++){
        int input_right_j=readVal("input",mid + 1 + j,1);
        assignVal("rightArray",j,input_right_j);
    }

    printf("Right Array : \n");
    for(int i=0; i<right ; i++){
        printf("%d\n", readVal("rightArray",i));
    }

    auto indexOfleft = 0,   // Initial index of first sub-array
        indexOfright = 0;    // Initial index of second sub-array
    int indexOfMergedArray = left1; // Initial index of merged array

    // Merge the temp arrays back into array[left..right]
    while (indexOfleft < left && indexOfright < right)
    {
        int left_val=readVal("leftArray",indexOfleft),right_val=readVal("rightArray",indexOfright);
        if (left_val <= right_val)
        {
            assignVal("input",indexOfMergedArray,left_val,1);
            indexOfleft++;
        }
        else
        {
            assignVal("input",indexOfMergedArray,right_val,1);
            indexOfright++;
        }
        indexOfMergedArray++;
    }
    // Copy the remaining elements of
    // left[], if there are any
    while (indexOfleft < left)
    {
        int left_val=readVal("leftArray",indexOfleft);
        assignVal("input",indexOfMergedArray,left_val,1);
        indexOfleft++;
        indexOfMergedArray++;
    }
    // Copy the remaining elements of
    // right[], if there are any
    while (indexOfright < right)
    {
        int right_val=readVal("rightArray",indexOfright);
        assignVal("input",indexOfMergedArray,right_val,1);
        indexOfright++;
        indexOfMergedArray++;
    }
    printf("Merged list : \n");
    for(int i=0; i<n ; i++){
        printf("%d\n", readVal("input",i,1));
    }
    // with arguments
    freeElem("leftArray");
    freeElem("rightArray");
    // // without arguments
    // freeElem();
}

// begin is for left index and end is
// right index of the sub-array
// of arr to be sorted */
void mergeSort( int const begin, int const end)
{
    if (begin >= end)
        return; // Returns recursively

    auto mid = begin + (end - begin) / 2;
     printf("begin : %d mid : %d end : %d\n", begin, mid, end);
    mergeSort(begin, mid);
    mergeSort( mid + 1, end);
    merge( begin, mid, end);
}

// Driver code
int main()
{
    initScope();
    createList("input", n,1);
    int x = createMemory(250 * 1024 * 1024);
    srand(time(NULL));
    for (int i = 0; i < n; i++)
    {
        int rand_int = rand() % 100000 + 1;
        assignVal("input", i, rand_int,1);
    }
    printf("Initial list : \n");
    for(int i=0; i<n ; i++){
        printf("%d\n", readVal("input",i,1));
    }
    mergeSort(0, n - 1);

    FILE* fp = fopen("output.txt","w");
    for(int i=0; i<n ; i++){
        fprintf(fp,"%d\n", readVal("input",i,1));
    }

    // freeing without arguments
    freeElem();
    // freeing with arguments
    freeElem("input");
    // freeElem("temp");

    return 0;
}

