#include <iostream>
#include <vector>
using namespace std;
struct arcNode{
    //int tNode, hNode;
    //float length;
    int num,index;

    /*arcNode(int t, int h, float l){
        tNode=t;
        hNode=h;
        length=l;
    }*/
};

struct binaryTree{
    vector<arcNode*> treeArray;
    binaryTree(){               // default constructor會把heap[0]給預留
        treeArray.resize(1);         // 之後若新增HeapNode, 會從heap[1]開始新增
    }

    void swapIndex(int indexA, int indexB){
        swap(treeArray[indexA],treeArray[indexB]);
        treeArray[indexA]->index = indexA;
        treeArray[indexB]->index = indexB;
    }

    void pushNode(arcNode *newNode){
        newNode->index=treeArray.size();
        treeArray.push_back(newNode);

        while((newNode->index/2>0)&&((newNode->num)<(treeArray[newNode->index/2]->num))){//若新node的ID較其parent小，則兩者交換位置
            int tempIndex = newNode->index;
            swapIndex(tempIndex,tempIndex/2);
        }
    }

    arcNode* getRoot(){
        return treeArray[1];
    }

    arcNode* popNode(){
        arcNode* oriRoot = treeArray[1];
        arcNode* lastNode = *--treeArray.end();
        treeArray[1] = lastNode;
        int tempIndex = 1;
        while((treeArray[tempIndex]->num > treeArray[tempIndex*2]->num) || (treeArray[tempIndex]->num > treeArray[tempIndex*2+1]->num)){
            if(treeArray[tempIndex*2]->num <= treeArray[tempIndex*2+1]->num) //若左小孩較小，則與左小孩換位置
                swapIndex(tempIndex, tempIndex*2);
            else
                swapIndex(tempIndex, tempIndex*2+1);
        }

        return oriRoot;
    }

    void printTree(){
        for(vector<arcNode*>::iterator it = ++treeArray.begin(); it != treeArray.end(); it++)
            cout << (*it)->num;
    }
};

int main(){
    arcNode a,b,c,d;

    a.num=5;
    b.num=3;
    c.num=1;
    d.num=8;

    binaryTree t;

    t.pushNode(&a);
    t.pushNode(&b);
    t.pushNode(&c);
    t.pushNode(&d);


    cout << (t.popNode()->num);
    cout << (t.popNode()->num);
    cout << (t.popNode()->num);
    cout << (t.popNode()->num);

    return 0;
}
