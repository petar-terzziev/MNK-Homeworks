#include <iostream>
#include <random>
#include <algorithm>
#include <queue>
#include <thread>
#include <time.h>

using namespace std;

struct MinHeapNode
{
    // The element to be stored
    int element;

    // index of the array from which the element is taken
    int i;
};

struct comp {

    bool operator () (const MinHeapNode &lhs, const MinHeapNode &rhs){
        return lhs.element > rhs.element;
    }

};

// Merges two subarrays of arr[].
// First subarray is arr[l..m]
//// Second subarray is arr[m+1..r]
//void merge(int arr[], int l, int m, int r)
//{
//    int i, j, k;
//    int n1 = m - l + 1;
//    int n2 = r - m;
//
//    /* create temp arrays */
//    int L[n1], R[n2];
//
//    /* Copy data to temp arrays L[] and R[] */
//    for(i = 0; i < n1; i++)
//        L[i] = arr[l + i];
//    for(j = 0; j < n2; j++)
//        R[j] = arr[m + 1 + j];
//
//    /* Merge the temp arrays back into arr[l..r]*/
//    i = 0; // Initial index of first subarray
//    j = 0; // Initial index of second subarray
//    k = l; // Initial index of merged subarray
//    while (i < n1 && j < n2)
//    {
//        if (L[i] <= R[j])
//            arr[k++] = L[i++];
//        else
//            arr[k++] = R[j++];
//    }
//
//    /* Copy the remaining elements of L[], if there
//       are any */
//    while (i < n1)
//        arr[k++] = L[i++];
//
//    /* Copy the remaining elements of R[], if there
//       are any */
//    while(j < n2)
//        arr[k++] = R[j++];
//}
//
///* l is for left index and r is right index of the
//   sub-array of arr to be sorted */
//void mergeSort(int arr[], int l, int r)
//{
//    if (l < r)
//    {
//        // Same as (l+r)/2, but avoids overflow for
//        // large l and h
//        int m = l + (r - l) / 2;
//
//        // Sort first and second halves
//        mergeSort(arr, l, m);
//        mergeSort(arr, m + 1, r);
//
//        merge(arr, l, m, r);
//    }
//}

FILE* openFile(char* fileName, char* mode)
{
    FILE* fp = fopen(fileName, mode);
    if (fp == nullptr)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }


    return fp;
}

// Merges k sorted files.  Names of files are assumed
// to be 1, 2, 3, ... k
void mergeFiles(char *output_file, int n, int k)
{
    std::vector<FILE*> in(k);
    for (int i = 0; i < k; i++)
    {
        char fileName[2];

        // convert i to string
        snprintf(fileName, sizeof(fileName), "%d", i);

        // Open output files in read mode.
        in[i] = openFile(fileName, "r");
    }

    // FINAL OUTPUT FILE
    FILE *out = openFile(output_file, "w");

    // Create a min heap with k heap nodes.  Every heap node
    // has first element of scratch output file
    std::vector<MinHeapNode> harr (k);
    std::priority_queue<MinHeapNode, std::vector<MinHeapNode>, comp> pq;
    int i;
    for (i = 0; i < k; i++)
    {
        // break if no output file is empty and
        // index i will be no. of input files
        if (fscanf(in[i], "%d ", &harr[i].element) != 1)
            break;

        harr[i].i = i; // Index of scratch output file
        pq.push(harr[i]);
    }

    int count = 0;

    // Now one by one get the minimum element from min
    // heap and replace it with next element.
    // run till all filled input files reach EOF
    while (count != i)
    {
        // Get the minimum element and store it in output file
        MinHeapNode root = pq.top();
        fprintf(out, "%d ", root.element);

        // Find the next element that will replace current
        // root of heap. The next element belongs to same
        // input file as the current min element.
        if (fscanf(in[root.i], "%d ", &root.element) != 1 )
        {
            root.element = std::numeric_limits<int>::max();
            count++;
        }

        // Replace root with next element of input file
        pq.pop();
        pq.push(root);
    }

    // close input and output files
    for (int i = 0; i < k; i++)
        fclose(in[i]);

    fclose(out);
}

// Using a merge-sort algorithm, create the initial runs
// and divide them evenly among the output files
void createInitialRuns(char *input_file, int run_size,
                       int num_ways)
{
    // For big input file
    FILE *in = openFile(input_file, "r");

    // output scratch files
    std::vector<FILE*> out (num_ways);
    char fileName[2];
    for (int i = 0; i < num_ways; i++)
    {
        // convert i to string
        snprintf(fileName, sizeof(fileName), "%d", i);

        // Open output files in write mode.
        out[i] = openFile(fileName, "w");
    }

    // allocate a dynamic array large enough
    // to accommodate runs of size run_size
    std::vector<int> arr (run_size);

    bool more_input = true;
    int next_output_file = 0;

    int i;
    while (more_input)
    {
        // write run_size elements into arr from input file
        for (i = 0; i < run_size; i++)
        {
            if (fscanf(in, "%d ", &arr[i]) != 1)
            {
                more_input = false;
                break;
            }
        }

        std::sort( arr.begin(), arr.end());

        // write the records to the appropriate scratch output file
        // can't assume that the loop runs to run_size
        // since the last run's length may be less than run_size
        for (int j = 0; j < i; j++)
            fprintf(out[next_output_file], "%d ", arr[j]);

        next_output_file++;
    }

    // close input and output files
    for (int i = 0; i < num_ways; i++)
        fclose(out[i]);

    fclose(in);
}

// For sorting data stored on disk
void externalSort(char* input_file,  char *output_file,
                  int num_ways, int run_size)
{
    // read the input file, create the initial runs,
    // and assign the runs to the scratch output files
    createInitialRuns(input_file, run_size, num_ways);

    // Merge the runs using the K-way merging
    mergeFiles(output_file, run_size, num_ways);
}
bool gen_file(){
    std::mt19937 gen(23);
    std::uniform_int_distribution<unsigned> dist(0,200000);
    const int FILE_SiZE = 5; //size in KB
    const int BUFFER_SIZE = 1024;


    FILE *pFile = fopen("somefile.txt", "w");
    if (pFile == nullptr)
        return false;
    for (int i = 0; i < FILE_SiZE; i++)
        fprintf(pFile, "%d", dist(gen));


    fclose(pFile);
    return true;
}

int main() {
    // No. of Partitions of input file.
    int num_ways = 10;

    // The size of each partition
    int run_size = 1;

    char input_file[] = "input.txt";
    char output_file[] = "output.txt";

    FILE* in = openFile(input_file, "w");

    srand(time(nullptr));

    // generate input
    for (int i = 0; i < num_ways * run_size; i++)
        fprintf(in, "%d ", rand());

    fclose(in);

    externalSort(input_file, output_file, num_ways,
                 run_size);

    return 0;


}