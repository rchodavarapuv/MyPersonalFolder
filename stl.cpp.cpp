/******************************************************************************

                              Online C++ Compiler.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <iostream>

using namespace std;
#include <unordered_map>
#include <map>
#include <vector>
#include <set>
#include <functional>

int main()
{
  std::unordered_map<int, string> hash;
  hash[3] ={"three"};
  hash[0] ={"zero"};
  hash[1] ={"one"};
  
  std:map<int,string> tree; 
  tree.insert ({3,"three"});
  tree[2] ={"two"};
  tree[1] = {"one"};
  
  
  for(auto it: hash)
  cout <<it.first <<":"<<it.second<<endl;
  
  for(auto it: tree)
  cout << it.first <<":" <<it.second<<endl;
  
  
  vector<int> avx512;
  for(auto i=0;i<5;i++)
  avx512.push_back(i);
  
  
  for(auto it = avx512.begin(); it!=avx512.end();it++)
  cout << *it;
  for(auto it = avx512.rbegin(); it!=avx512.rend();it++)
  cout << *it;
  
  cout << endl <<avx512.size();
  
  
  
  
  set<int> first;
  first ={1,2,3,1,2,3};
  
  for(const auto &it:first)
  cout << endl << it;
 
  return 0;
}

