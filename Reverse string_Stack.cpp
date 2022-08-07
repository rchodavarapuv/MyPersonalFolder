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

using namespace std;

int main()
{
    stack<string> rev_stack;
    string str= "Hello world";
    string word;
    
    stringstream ss(str);
    while(ss >> word)
    {
        rev_stack.push(word);
    }
    
    while(!rev_stack.empty())
    {
        word = rev_stack.top();
        cout << word << "\t";
        rev_stack.pop();
    }
    
    
    return 0;
}


