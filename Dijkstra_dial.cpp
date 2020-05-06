/*
   This code can be compiled and run ok.

   purpose:
     This is to read the network data from txt/sp format, and output 1-ALL shortest paths of source node.

   input file:
     test1.sp (there is no negative cycle and the largest length of arc is small)

   compile:
     g++ -o h34051178_hw3_2_Dijkstra_dial h34051178_hw3_2_Dijkstra_dial.cpp

   pseudocode:
   1. use forward and reverse star data structure
   2. use as same network storage method as hw3_1
   3. void readArc, setPoint, setTrace, setDijkstra ,and printResult are both mentioned by hw3_1
   4. void readfileAndIni is added the dial array
   ======
   void pushIntoDial
   ------
   set the length round down, and push the arc into dial array.
   ======
   Arc* popFromDial
   ------
   find the smallest index of non-empty bucket
   erase the element from the dial
   return the element
   ======
   bool isDialNull
   ------
   if there is a empty bucket, the dial isn't null.
   otherwise, if we can't find non-empty bucket, the dial is null.
   ======
   coded by Yu-En Shen, ID: h34051178, email: fourfire11@gmail.com
   date: 2020.05.03
*/
#define MAXN 9999
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <list>
#include <cmath>
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

void readfileAndIni(string &proName, int &nodeNum, int &arcNum, int &dialSize, int *&trace, Arc *&arcs, Node *&nodes, list<Arc*> *&dial);
void setPoint(int nodeNum, int arcNum, Node *nodes, Arc *arcs, int *trace);
void setTrace(int arcIndex, Arc *arc, int *&trace);
void pushIntoDial(Arc *ptr, list<Arc*> *&dial);
Arc* popFromDial(bool &isDialNull, list<Arc*> *&dial);
bool isDialNull(int dialSize, list<Arc*> *dial);
void setDijkstra(int sourceNode, int nodeNum, int arcNum, int dialSize, Node *&nodes, Arc *arcs, list<Arc*> *&dial);
void printResult(int sourceNode, int nodeNum, Node *nodes);
float readArc(string s, int &pos);

int main(){
    string proName;
    int nodeNum,    //number of node
        arcNum,     //number of arc
        *trace,     //record trace of each arc
        sourceNode,
        dialSize;
    Node *nodes;
    Arc *arcs;
    list<Arc*> *dial;

    readfileAndIni(proName, nodeNum, arcNum, dialSize, trace, arcs, nodes, dial);

    //ask user to input source node, and determine this node whether valid or not
    while(1){
        cout << "Please input a source node: ";
        cin >> sourceNode;
        if((sourceNode>=1)&&(sourceNode<=nodeNum)){
            break;
        }
        cout << "!!Warning!!: node " << nodeNum << " does not exist" << endl;
    }
    setDijkstra(sourceNode, nodeNum, arcNum, dialSize, nodes, arcs, dial);
    printResult(sourceNode, nodeNum, nodes);

    delete [] arcs;
    delete [] trace;
    delete [] nodes;
    delete [] dial;

    return 0;
}

float readArc(string s, int &pos){
    int startIndex = s.find_first_of("0123456789", pos);
    pos = s.find_first_not_of("0123456789", startIndex);
    s = s.assign(s, startIndex, pos-1);
    return atof(s.c_str());
}

void readfileAndIni(string &proName, int &nodeNum, int &arcNum, int &dialSize, int *&trace, Arc *&arcs, Node *&nodes, list<Arc*> *&dial){
    string tempName,  //string to get file name
           temp;      //string to get each row of data
    int arcIndex=0;   //the index of current arc
    fstream dataFile; //the txt or sp file
    float maxArcLen=0;

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
            if(arcs[arcIndex].length > maxArcLen) maxArcLen = arcs[arcIndex].length;

            //if the head node of current arc is smaller than the previous one(in trace array)
            //change the index sequence of trace array
            if((arcIndex>0)&&(arcs[arcIndex].hNode<arcs[trace[arcIndex-1]].hNode)) setTrace(arcIndex, arcs, trace);

            arcIndex++;
            break;
        }
    }while(!dataFile.eof());
    dataFile.close();
    setPoint(nodeNum, arcNum, nodes, arcs, trace);
    dialSize=ceil(maxArcLen);
    dial = new list<Arc*> [dialSize+1];
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

void pushIntoDial(Arc *ptr, list<Arc*> *&dial){
    int bucketIndex=floor(ptr->length); //round down the length
    dial[bucketIndex].push_back(ptr);
}

Arc* popFromDial(list<Arc*> *&dial){
    int bucketIndex=0;
    while(dial[bucketIndex].empty()) bucketIndex++; // find the smallest index of non-empty bucket
    Arc* popArc = dial[bucketIndex].front(); // get the first element of first non-empty bucket
    dial[bucketIndex].erase(dial[bucketIndex].begin()); // erase the element from the bucket
    return popArc;
}

bool isDialNull(int dialSize, list<Arc*> *dial){
    for(int i=0; i<=dialSize; i++) if(!dial[i].empty()) return false;
    return true;
}

void setDijkstra(int sourceNode, int nodeNum, int arcNum, int dialSize, Node *&nodes, Arc *arcs, list<Arc*> *&dial){
    int preNode=sourceNode, // record the tail node of current arc
        nextNode;           // record the head node of current arc
    Arc *curArc;            // record current arc
    nodes[sourceNode].disLabel=0;

    do{
        // push all arcs outgoing from preNode into dial
        for(Arc *ptr=nodes[preNode].point; (preNode!=nodeNum)&&(ptr!=nodes[preNode+1].point); ptr++)
            if(nodes[ptr->hNode].disLabel==MAXN) pushIntoDial(ptr, dial);
        if (isDialNull(dialSize, dial)) break; // if dial is empty, meaning the dijkstra algorithm has been finished
        // popping arc until this arc can reduce the distance label of its head node
        do curArc = popFromDial(dial);
        while((nodes[curArc->hNode].disLabel<nodes[curArc->tNode].disLabel+curArc->length)&&(!isDialNull(dialSize, dial)));

        if(nodes[curArc->hNode].disLabel>nodes[curArc->tNode].disLabel+curArc->length){
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

