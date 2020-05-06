/*
   This code can be compiled and run ok.

   purpose:
     This is to read the network data from txt/sp format, and output 1-ALL shortest paths of source node.

   input file:
     test1.sp (there is no negative cycle)

   compile:
     g++ -o h34051178_hw3_1_Dijkstra_min heap h34051178_hw3_1_Dijkstra_min heap.cpp

   pseudocode:
   use forward and reverse star data structure
   ======
   some adjusting about the network storage compares to the past homeworks
   1. Adjusting the storage way of arcs from 2-D array to struct Arc, so I added function (float readArc(string s, int &pos))
   to read arc.
   2. I also adjusted the storage point  of from 2-D array to Arc *point and *rpoint in struct Node.
   3. I changed the starting index of nodes from 0 to 1, so the index in nodes are same as the ID of node.
   And I also tried to set point and rpoint with the method similar to counting sort, so that the code can be more readable.
   ======
   Arc* popArc() (in struct binaryTree)
   ------
   record the original root of tree
   move the last node to treeArray[1](the index of root)
   while (parent > any child)
      swap parent and the smaller child
   end loop
   ======
   Arc* pushArc() (in struct binaryTree)
   ------
   insert new node to the last of treeArray
   while (new node > its parent)
      swap new node and its parent
   end loop
   ======
   void setDijkstra(int sourceNode, int nodeNum, int arcNum, Node *&nodes, Arc *arcs, binaryTree &biTree);
   ------
   set source node's distance label=0
   while(treeArray isn't empty)
      push source node relate arcs into binary tree
      while(can't reduce the distance label of root's head node)
         pop the root of tree
      end
      update the distance label of root's head node
   end
   ======
   void printResult(int sourceNode, int nodeNum, Node *nodes)
   ------
   for all node
      print distance label of current node
      print precede nodes of current node
   end
   ======
   coded by Yu-En Shen, ID: h34051178, email: fourfire11@gmail.com
   date: 2020.05.01
*/
#define MAXN 9999
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
using namespace std;

struct Arc{
    int tNode, hNode;
    float length;
    int index;
};

struct Node{
    int ID,
        arcN,
        preNodeID;
    Arc *point,*rpoint;
    float disLabel;
    bool isReach;
};

struct binaryTree{
    // record all node in binary treetree
    // treeArray[1] is the root
    // the parent of node in treeArray[i] is i/2
    // the left child of node in treeArray[i] is 2*i
    // the right child of node in treeArray[i] is 2*i+1
    vector<Arc*> treeArray;

    binaryTree(){                    // default constructor conserves the space of heap[0]
        treeArray.resize(1);         // so that adding element will start at index 1
    }

    Arc* getRoot(){
        return treeArray[1];
    }

    Arc* popArc(){
        Arc* oriRoot = treeArray[1]; // record original root and return it in the end of function
        Arc* lastNode = *--treeArray.end(); //
        treeArray[1] = lastNode;
        int tempIndex = 1;

        do{
            if(tempIndex*2+1>=treeArray.size()) break;
            // change the index with the child which has smaller length
            if(treeArray[tempIndex*2]->length <= treeArray[tempIndex*2+1]->length) // left child is smaller
                swapIndex(tempIndex, tempIndex*2);
            else // right child is smaller
                swapIndex(tempIndex, tempIndex*2+1);
        }while((treeArray[tempIndex]->length > treeArray[tempIndex*2]->length) || (treeArray[tempIndex]->length > treeArray[tempIndex*2+1]->length));

        treeArray.erase(--treeArray.end());
        return oriRoot;
    }

    void swapIndex(int indexA, int indexB){
        swap(treeArray[indexA],treeArray[indexB]);
        treeArray[indexA]->index = indexA;
        treeArray[indexB]->index = indexB;
    }

    void pushArc(Arc *newNode){
        newNode->index=treeArray.size();
        treeArray.push_back(newNode);

        while((newNode->index/2>0)&&((newNode->length)<(treeArray[newNode->index/2]->length))){//若新node的ID較其parent小，則兩者交換位置
            int tempIndex = newNode->index;
            swapIndex(tempIndex,tempIndex/2);
        }
    }

    void printTree(){
        for(vector<Arc*>::iterator it = ++treeArray.begin(); it != treeArray.end(); it++)
            cout << (*it)->tNode << " " << (*it)->hNode << " " << (*it)->length << endl;
    }
};

void readfileAndIni(string &proName, int &nodeNum, int &arcNum, int *&trace, Arc *&arcs, Node *&nodes);
void setPoint(int nodeNum, int arcNum, Node *nodes, Arc *arcs, int *trace);
void setTrace(int arcIndex, Arc *arc, int *&trace);
void setDijkstra(int sourceNode, int nodeNum, int arcNum, Node *&nodes, Arc *arcs, binaryTree &biTree);
void printResult(int sourceNode, int nodeNum, Node *nodes);
float readArc(string s, int &pos);

int main(){
    string proName;
    int nodeNum,    //number of node
        arcNum,     //number of arc
        *trace,     //record trace of each arc
        sourceNode;
    Node *nodes;
    Arc *arcs;
    binaryTree biTree;

    readfileAndIni(proName, nodeNum, arcNum, trace, arcs, nodes);

    //ask user to input source node, and determine this node whether valid or not
    while(1){
        cout << "Please input a source node: ";
        cin >> sourceNode;
        if((sourceNode>=1)&&(sourceNode<=nodeNum)){
            break;
        }
        cout << "!!Warning!!: node " << nodeNum << " does not exist" << endl;
    }
    setDijkstra(sourceNode, nodeNum, arcNum, nodes, arcs, biTree);
    printResult(sourceNode, nodeNum, nodes);

    delete [] arcs;
    delete [] trace;
    delete [] nodes;

    return 0;
}

float readArc(string s, int &pos){
    int startIndex = s.find_first_of("0123456789", pos);
    pos = s.find_first_not_of("0123456789", startIndex);
    s = s.assign(s, startIndex, pos-1);
    return atof(s.c_str());
}

void readfileAndIni(string &proName, int &nodeNum, int &arcNum, int *&trace, Arc *&arcs, Node *&nodes){
    string tempName,  //string to get file name
           temp;      //string to get each row of data
    int arcIndex=0;   //the index of current arc
    fstream dataFile; //the txt or sp file
    while(1){
        cout << "Please input network filename: ";
        cin >> tempName;
        const char* fName = tempName.c_str();
        dataFile.open( fName, ios::in);
        if(dataFile.is_open()){
            break;
        }
        cout << "the file not exist !!" << endl;
    }

    do{
        getline(dataFile, temp);
        if(temp=="") continue; //to avoid the case that can't get temp.at(0) in blank row
        switch(temp.at(0)){
        case 't':
            proName = temp.substr(2);
            break;
        case 'p':
            temp = temp.substr(temp.find_first_of("0123456789"));
            nodeNum = atoi(temp.c_str());
            temp = temp.substr(temp.find_first_not_of("0123456789"));
            arcNum = atoi(temp.c_str());
            trace = new int [arcNum];
            for(int i=0; i<arcNum; i++) trace[i]=i;

            arcs = new Arc [arcNum+1];
            nodes = new Node [nodeNum+1];
            for(int i=1; i<=nodeNum; i++) nodes[i].ID=i, nodes[i].disLabel=MAXN;
            break;
        case 'a':
            int endIndex=0; //the end index of each number

            arcs[arcIndex].tNode = (int)readArc(temp, endIndex);
            arcs[arcIndex].hNode = (int)readArc(temp, endIndex);
            arcs[arcIndex].length = readArc(temp, endIndex);

            //if the head node of current arc is smaller than the previous one(in trace array)
            //change the index sequence of trace array
            if((arcIndex>0)&&(arcs[arcIndex].hNode<arcs[trace[arcIndex-1]].hNode)) setTrace(arcIndex, arcs, trace);

            arcIndex++;
            break;
        }
    }while(!dataFile.eof());
    dataFile.close();
    setPoint(nodeNum, arcNum, nodes, arcs, trace);
}

void setTrace(int arcIndex, Arc *arcs, int *&trace){
    int tempNum = trace[arcIndex]; //record the outgoing index of current arc
    int tempIndex = arcIndex;      //initial the index of searching

    //move original elements backward until the head node of current arc is smaller than one of trace[tempIndex]
    do{
        trace[tempIndex]=trace[tempIndex-1];
        tempIndex--;
    }while((arcs[arcIndex].hNode<arcs[trace[tempIndex]].hNode)&&(tempIndex!=0));

    if (arcs[arcIndex].hNode<arcs[trace[tempIndex]].hNode) trace[tempIndex] = tempNum;
    else trace[tempIndex+1] = tempNum;
    //else meaning the case that loop is broken by tempIndex=0,
    //but in fact the head node of current arc isn't smaller than trace[0]
}

void setPoint(int nodeNum, int arcNum, Node *nodes, Arc *arcs, int *trace){
    int index=0;
    int *countNode;
    countNode = new int [nodeNum+1]; // count the times of being tail node/ head node of each node

    //set point
    for(int i=0; i<=nodeNum; i++) countNode[i]=0;
    for(int i=0; i<arcNum; i++) countNode[arcs[i].tNode]++;
    for(int i=1; i<=nodeNum; i++){
        nodes[i].point=&arcs[index];
        index+=countNode[i];
    }

    //set rpoint
    for(int i=0; i<=nodeNum; i++) countNode[i]=0;
    for(int i=0; i<arcNum; i++) countNode[arcs[i].hNode]++;
    index=0;
    for(int i=1; i<=nodeNum; i++){
        nodes[i].rpoint=&arcs[trace[index]];
        index+=countNode[i];
    }

    delete [] countNode;
}

void setDijkstra(int sourceNode, int nodeNum, int arcNum, Node *&nodes, Arc *arcs, binaryTree &biTree){
    int preNode=sourceNode, // record the tail node of current arc
        nextNode;           // record the head node of current arc
    Arc *curArc;            // record current arc
    nodes[sourceNode].disLabel=0;

    do{
        // push all arcs outgoing from preNode into binary tree
        for(Arc *ptr=nodes[preNode].point; (preNode!=nodeNum)&&(ptr!=nodes[preNode+1].point); ptr++)
            if(nodes[ptr->hNode].disLabel==MAXN) biTree.pushArc(ptr);
        if ((biTree.treeArray.size()==1)) break; // if binary tree is empty, meaning the dijkstra algorithm has been finished
        // popping arc until this arc can reduce the distance label of its head node
        do curArc = biTree.popArc();
        while((nodes[curArc->hNode].disLabel<=nodes[curArc->tNode].disLabel+curArc->length)&&(biTree.treeArray.size()!=1));

        if (nodes[curArc->hNode].disLabel>nodes[curArc->tNode].disLabel+curArc->length){
        nextNode = curArc->hNode;
        nodes[nextNode].preNodeID = curArc->tNode; // set precede node of next node
        nodes[nextNode].disLabel = nodes[curArc->tNode].disLabel + curArc->length; // update the distance label of next node
        preNode=nextNode; // update preNode
        }

    }while(1);
}

void printResult(int sourceNode, int nodeNum, Node *nodes){
    for(int i=1; i<=nodeNum; i++){
        if(i==sourceNode) continue; // skip the path from source node to source node
        cout << sourceNode << "->" << i << ": ";
        if(nodes[i].disLabel==MAXN) cout << "[can not reach]\n";
        else{
            int tempNode = i; // record current node which is printed
            cout << "[" << nodes[i].disLabel << "] " << nodes[i].ID;
            while(tempNode!=sourceNode){
                tempNode=nodes[tempNode].preNodeID;
                cout <<"<-"<< tempNode ;
            }
            cout << '\n';
        }
    }
}
