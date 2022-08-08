/******************************************************************************

                              Online C++ Compiler.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <stack>
#include<bits/stdc++.h>
#include <list>
#include <chrono>

using namespace std;

int main()
{
    list<int> list1({1,2,3,4});
    list<int> list2({3,4,5,6});
    
    auto begin = std::chrono::high_resolution_clock::now();
   
    list1.merge(list2);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
    printf("Time measured: %.9f seconds.\n", elapsed.count() * 1e-9);
    
    for(int val:list1)
    {
        cout <<val;
    }
    
    return 0;
}


