//compile: g++ -fopenmp omp.cpp -o omp


#include <iostream>
#include <random>
#include <chrono>
void sort(int * array, int N);

int main()
{
    int *array;
    int N = 100000000;
    array = new int[N];
    std::random_device rd;  
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<> dis(1, 100000);

    auto start = std::chrono::steady_clock::now();
    #pragma omp parallel for
    for(int i = 0; i < N; ++i)
        array[i] = dis(gen);
    
    sort(array, N);
    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "OpenMP: " << elapsed.count() << "\n";

    for (int i = 0; i < N-1; ++i)
        if(array[i]>array[i+1])
           { std::cout << i << " not sorted!!!\n"; break;}
    return 0;
}

void sort(int * array, int N)
{
    //find min and max
    int min = array[0], max = array[0];
    
    #pragma omp parallel sections
    {
        #pragma omp section
        {
        for (int i = 0; i < N; ++i) 
            if (min > array[i])
                min = array[i];
        }

        #pragma omp section
        {
        for (int i = 0; i < N; ++i) 
            if (max < array[i])
                max = array[i];
        }
    }
  
    
    //initializing array for counting
    int range = max - min + 1;
    int counts[range];
    for (int i = 0; i < range; ++i) counts[i] = 0;

    //counting    
    //#pragma omp parallel for
    for(int i = 0; i < N; ++i){
        //#pragma omp atomic
        counts[array[i] - min]++;
    }
    
    //rewriting the array
    int j = 0;
    for(int i = 0; i < range; ++i)
        for(int k = 0; k < counts[i]; ++k)
            array[j++] = i + min;
}
