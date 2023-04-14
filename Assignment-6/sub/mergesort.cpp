#include"goodmalloc.hpp"
#define n 1000
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
    if(x==-1)
    {
        cout<<"OOPS! Can't allocate memory\n";
        exit(-1);
    }
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