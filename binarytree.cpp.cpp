/******************************************************************************

                              Online C++ Compiler.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <iostream>
#include <stdio.h>
#include <queue>

using namespace std;

typedef struct node
{
    int data;
    struct node* left_ptr;
    struct node* righ_ptr;
}node;

node* push_node(int, node*);
void pop_node(int,node*);
void inorder_traversal(node*);
int searchlist(int, node*);
int BFS(node*);

int main()
{
    
    node* head = NULL;
    head = push_node(10, head);
    push_node(20, head);
    push_node(5, head);
    push_node(50,head);
    push_node(7,head);
    push_node(4,head);
    
    //pop_node(50,head);
    
    //inorder_traversal(head);
    int level = BFS(head);
    cout << "number of level" << level;
    
    if(auto a = searchlist(10,head))
    {
        cout << "Found at height:" << a;
    }
    else
    {
        cout << "Not found";
    }
    
    free(head);

    return 0;
}

int searchlist(int x, node *temp)
{
    static int height = 1;
    
    if(temp ->data == x)
    {
        return height;
    }
    else if(x > temp->data)
    {
        cout << temp->data << "next" << temp->righ_ptr->data;
        height++;
        height = searchlist(x,temp->righ_ptr);
    }
    else
    {
        cout << temp->left_ptr->data;
        height++;
        searchlist(x,temp->left_ptr);
    }
    return height;
}
void pop_node(int a, node* head)
{
    
}

node* push_node(int a, node* head)
{
    if(head == NULL)
    {
        node *temp = (node*)malloc(sizeof(node));
        if(temp == NULL)
        return NULL;
        temp ->data = a;
        //cout << temp->data << endl;
        temp ->left_ptr = NULL;
        temp ->righ_ptr = NULL;
        return temp;
    }
    else
    {
        if( a > head->data)
        {
            head->righ_ptr = push_node(a, head->righ_ptr);
        }
        else if(a < head->data)
        {
           head->left_ptr = push_node(a, head ->left_ptr);
        }
        else
        {
            cout << "Data is same as head";
        }
        return head;
    }
    
}
int BFS(node *tree)
{
    static queue<node*> bfs;
    static int count = 0;
    cout << tree->data << ' ';
    if(tree->left_ptr != NULL)
    bfs.push(tree->left_ptr);
    if(tree->righ_ptr != NULL)
    bfs.push(tree->righ_ptr);
    if(count)
    bfs.pop();
    count++;
    if(!bfs.empty())
    BFS(bfs.front());
    
    return count;
}

void inorder_traversal(node * tree)
{
if (tree!=NULL) 
{ 
    inorder_traversal(tree->left_ptr);
    cout << tree->data << endl;
    inorder_traversal(tree->righ_ptr);
}
}
