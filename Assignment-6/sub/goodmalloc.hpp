#ifndef	GOOD_MALLOC
#define	GOOD_MALLOC
#include <iostream>
#include <bits/stdc++.h>
using namespace std;

int createMemory(int size);
void initScope();
int createList(string name, int size, int isglobal = 0);
int readVal(string listname, int offset, int isglobal = 0);
int assignVal(string listname, int offset, int value,int isglobal=0);
void freeElem(string id = "?");



#endif


