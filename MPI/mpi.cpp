//compile: mpic++ -o mpi mpi.cpp
//run: mpiexec -n 2 ./mpi

#include <iostream>
#include <random>
#include <chrono>
#include "/usr/include/mpich/mpi.h"

void sort(int * array, int N);

int ProcNum;
int ProcRank;

int main(int argc, char* argv[])
{
    int errCode;
    if ((errCode = MPI_Init(&argc, &argv)) != 0)
        return errCode;

    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);

    if (ProcNum != 2 && ProcNum % 4 != 0)
    {
        std::cout << "bad procNum!\n";
        return 0;
    }
    
    int *array;
    int N = 100000000;
    int *partArray = new int[N/ProcNum];

    auto start = std::chrono::steady_clock::now();

    std::random_device rd;  
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<> dis(1, 100000);

    for(int i = 0; i < N/ProcNum; ++i)
        partArray[i] = dis(gen);
    
    array = new int[N];
    MPI_Allgather(partArray, N/ProcNum, MPI_INT, array, N/ProcNum, MPI_INT, MPI_COMM_WORLD); 
    
    sort(array, N);
    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    if (ProcRank == 0) 
        std::cout << "MPI: " << elapsed.count() << "\n";

    if (ProcRank == 0)
    {
    //check for errors
        for (int i = 0; i < N-1; ++i)
            if(array[i]>array[i+1]){
            std::cout << i << " not sorted!!!\n"; 
            break;
        }
    }
    
    delete [] array;
    delete [] partArray;

    MPI_Finalize();
    return 0;
}

void sort(int * array, int N)
{
    //find min and max
    int min, max;
    
    if (ProcRank == 0)
    {
        min = array[0];
        for(int i = 0; i < N; ++i)
        {
            if (min > array[i])
                min = array[i];
        }
    }
    MPI_Bcast(&min, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if(ProcRank == 1)
    {
        max = array[0];
        for(int i = 0; i < N; ++i)
        {
            if (max < array[i])
                max = array[i];
        }
    }
    MPI_Bcast(&max, 1, MPI_INT, 1, MPI_COMM_WORLD);

    int range = max - min + 1;
    if(ProcRank == 0)
    {
        //initializing array for counting
        int counts[range];
        for (int i = 0; i < range; ++i) counts[i] = 0;

        //counting
        for(int i = 0; i < N; ++i)
            counts[array[i] - min]++;

        //rewriting the array
        int j = 0;
        for(int i = 0; i < range; ++i)
            for(int k = 0; k < counts[i]; ++k)
                array[j++] = i + min;
    }
    
}
