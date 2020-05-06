/*
   This code can be compiled and run ok.

   purpose:
     This is to read the network data from txt/sp format, and output 1-ALL shortest paths of source node.

   input file:
     test1.sp

   compile:
     g++ -o h34051178_hw3_3_bellmanFord h34051178_hw3_3_bellmanFord.cpp

   pseudocode:
   1. use forward and reverse star data structure
   2. use as same network storage method as hw3_1
   3. void readfileAndIni, readArc, setPoint, setTrace, and printResult are all mentioned by hw3_1 or past homework
   ======
   void bellmanFord
   ------
   initial all node distance = MAXN
   push source node into list
   set source node's distance = 0
   while nodeList isn't empty
      curNode = the front element in nodeList
      for each node connected by curNode
         if using this arc starting from curNode can reduce the distance of the new node
            update the distance of the new node
            set up the precede node of the new node
            push the new node into nodeList
         end
      end
   end
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

void readfileAndIni(string &proName, int &nodeNum, int &arcNum, int &dialSize, int *&trace, Arc *&arcs, Node *&nodes);
void setPoint(int nodeNum, int arcNum, Node *nodes, Arc *arcs, int *trace);
void setTrace(int arcIndex, Arc *arc, int *&trace);
void bellmanFord(int sourceNode, int nodeNum, Node *&nodes, Arc *arcs);
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

    readfileAndIni(proName, nodeNum, arcNum, dialSize, trace, arcs, nodes);

    //ask user to input source node, and determine this node whether valid or not
    while(1){
        cout << "Please input a source node: ";
        cin >> sourceNode;
        if((sourceNode>=1)&&(sourceNode<=nodeNum)){
            break;
        }
        cout << "!!Warning!!: node " << nodeNum << " does not exist" << endl;
    }
    bellmanFord(sourceNode, nodeNum, nodes, arcs);
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

void readfileAndIni(string &proName, int &nodeNum, int &arcNum, int &dialSize, int *&trace, Arc *&arcs, Node *&nodes){
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

void bellmanFord(int sourceNode, int nodeNum, Node *&nodes, Arc *arcs){
    list<Node> nodeList; // record which node will the next to be dealt with
    int curNode; // record the node which is in process
    for(int i=1; i<=nodeNum; i++) nodes[i].disLabel=MAXN;
    // set up the source node
    nodeList.push_back(nodes[sourceNode]);
    nodes[sourceNode].disLabel=0;
    curNode=sourceNode;

    // loop until the nodeList is empty
    do{
        curNode=nodeList.front().ID;
        nodeList.erase(nodeList.begin());
        for(Arc *ptr=nodes[curNode].point; (curNode!=nodeNum)&&(ptr!=nodes[curNode+1].point); ptr++)
            // if using this arc can reduce this node's distance
            if(nodes[ptr->tNode].disLabel+ptr->length < nodes[ptr->hNode].disLabel){
                nodeList.push_back(nodes[ptr->hNode]);
                nodes[ptr->hNode].preNodeID = curNode;
                nodes[ptr->hNode].disLabel = nodes[curNode].disLabel + ptr->length;
            }
    }while(!nodeList.empty());
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

