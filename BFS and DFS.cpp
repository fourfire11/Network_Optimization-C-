/*
   This code can be compiled and run ok.

   purpose:
     This is to read the network data from txt/sp format, and output the reached nodes(by BFS)
     and all paths from source node to sink node(by DFS), finding the shorted path.

   input file:
     test1.sp (doesn't consider the cycle case)

   compile:
     g++ -o h34051178_hw2 h34051178_hw2.cpp

   pseudocode:
   use forward and reverse star data structure
   ======
   sub readfileAndIni(), setTrace(), setPoint() had been mentioned in hw1
   ======
   sub setBFS(sourceNode, sinkNode, point[], arc[], trace[], nodes[])
   ------
   put sourceNode into BFS queue
   curNode = sourceNode
   while(BFS is not empty)
     for each out degree arc of curNode isn't reached
       add the head node of arc to a BFS queue
       set head node reached
       length of head node = length of tail node + 1
     end loop
     pop curNode from BFS
     curNode = next node in BFS
   end loop
   print the BFS result
   ======
   sub setDFS(sourceNode, sinkNode, point[], arc[], trace[], nodes[])
   ------
   while(DFS is not empty)
     if reach the sinkNode
       record this path (by sub putNodeToCurpath)
       update the number of path and shorted path
       pop this path (by sub popNode)
     else
       for each out degree arc of curNode isn't reached
         add the head node of arc to a DFS stack
         set head node reached
         length of head node = length of tail node + the length of arc
       end loop
     end if
     pop curNode from DFS
     curNode = top node in DFS
   end loop
   print all path
   ======
   sub putNodeToCurpath(curPath, node)
   ------
   convert type from int to string
   add this node into stack<string> curPath
   ======
   string combineCurpath(curPath, leng)
   ------
   when a path is finished
   transform stack<string> into string and add the length of this path
   ======
   popNode(parNodes[], curPath, nodes, nowNode, lastNode)
   ------
   while(nowNode != the parent of last node in DFS)
     set nowNode is not reached
     erase the length of nowNode in this path
     pop this node in current path
     nowNode = nowNode's parent
   end loop
   ======

   coded by Yu-En Shen, ID: h34051178, email: fourfire11@gmail.com
   date: 2020.04.14
*/
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <stdio.h>
#include <set>
#include <queue>
#include <stack>
#include <vector>
using namespace std;
struct Node{
    int ID,
        arcN;
    stack<float> leng;
    bool isReach;
};
void readfileAndIni(string &proName, int &nodeNum, int &arcNum, int **&point, float **&arc, int *&trace, Node *&nodes);
void setPoint(int nodeNum, int arcNum, int **&point, float **arc, int *trace);
void setTrace(int arcIndex, float **arc, int *&trace);
void setBFS(vector<int> &reachedNode, int nodeNum, int sourceNode, int sinkNode, int **point, float **arc, int *trace, Node *&nodes);
void setDFS(int nodeNum, int sourceNode, int sinkNode, int **point, float **arc, int *trace, Node *&nodes);
void putNodeToCurpath(stack<string> &curPath, int node);
string combineCurpath(stack<string> &curPath, int leng);
void popNode(stack<Node> DFS, stack<int> *&parNodes, stack<string> &curPath, Node *&nodes, int parNode);
bool isCycle(stack<int> *parNodes, int sourceNode, int nowNode, int chiNode);

int main(){
    string proName;
    int nodeNum,    //number of node
        arcNum,     //number of arc
        **point,    //2D-array to record point and rpoint of each node
        *trace,     //record trace of each arc
        sourceNode,
        sinkNode; //we will print all arcs connecting to this node
    float **arc;    //2D-array to record tail node, head node, and length of each arc
    Node *nodes;
    queue<string> allPath;
    vector<int> reachedNode;

    readfileAndIni(proName, nodeNum, arcNum, point, arc, trace, nodes);

    //ask user to input source node, and determine this node whether valid or not
    /*while(1){
        cout << "Please input a source node: ";
        cin >> sourceNode;
        if(sourceNode<=nodeNum){
            break;
        }
        cout << "!!Warning!!: node " << nodeNum << " does not exist" << endl;
    }
    while(1){
        cout << "Please input a sink node: ";
        cin >> sinkNode;
        if(sinkNode<=nodeNum){
            break;
        }
        cout << "!!Warning!!: node " << nodeNum << " does not exist" << endl;
    }*/
    sourceNode=2;
    sinkNode=6;
    setBFS(reachedNode, nodeNum, sourceNode, sinkNode, point, arc, trace, nodes);
    if(reachedNode.size()!=0) setDFS(nodeNum, sourceNode, sinkNode, point, arc, trace, nodes);

    for(int i=0; i<=nodeNum; i++) delete[] point[i];
    delete [] point;
    for(int i=0; i<arcNum; i++) delete[] arc[i];
    delete [] arc;
    delete [] trace;
    delete [] nodes;

    return 0;
}

void readfileAndIni(string &proName, int &nodeNum, int &arcNum, int **&point, float **&arc, int *&trace, Node *&nodes){
    string tempName,  //string to get file name
           temp;      //string to get each row of data
    int arcIndex=0;   //the index of current arc
    fstream dataFile; //the txt or sp file
    while(1){
        //cout << "Please input network filename: ";
        //cin >> tempName;
        tempName="test.sp";
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
            nodeNum=atoi(temp.c_str());
            temp = temp.substr(temp.find_first_not_of("0123456789"));
            arcNum=atoi(temp.c_str());
            point = new int* [nodeNum+1]; //add one node to record ending point
            for(int i=0; i<=nodeNum; i++) point[i] = new int[2]; //2 column include point and rpoint
            for(int i=0; i<=nodeNum; i++) for(int j=0; j<2; j++) point[i][j]=0;

            arc = new float* [arcNum];
            for(int i=0; i<arcNum; i++) arc[i] = new float[3]; //3 column include tail node, head node, and length
            for(int i=0; i<arcNum; i++) for(int j=0; j<3; j++) arc[i][j]=0;

            trace = new int [arcNum];
            for(int i=0; i<arcNum; i++) trace[i]=i;

            nodes = new Node [nodeNum];
            for(int i=0; i<nodeNum; i++) nodes[i].ID=i;
            break;
        case 'a':
            int startIndex, //the start index of each number
                endIndex=0; //the end index of each number
            string number;  //temporary record the number string

            //3 include tail node, head node, and length
            for(int i=0; i<3; i++){
                startIndex=temp.find_first_of("0123456789",endIndex);
                endIndex=temp.find_first_not_of("0123456789",startIndex);
                number = number.assign(temp, startIndex, endIndex-1);
                arc[arcIndex][i] = atoi(number.c_str());
            }

            //if the head node of current arc is smaller than the previous one(in trace array)
            //change the index sequence of trace array
            if((arcIndex>0)&&(arc[arcIndex][1]<arc[trace[arcIndex-1]][1])) setTrace(arcIndex, arc, trace);

            arcIndex++;
            break;
        }
    }while(!dataFile.eof());
    dataFile.close();
    setPoint(nodeNum, arcNum, point, arc, trace);
}

void setTrace(int arcIndex, float **arc, int *&trace){
    int tempNum = trace[arcIndex]; //record the outgoing index of current arc
    int tempIndex = arcIndex;      //initial the index of searching

    //move original elements backward until the head node of current arc is smaller than one of trace[tempIndex]
    do{
        trace[tempIndex]=trace[tempIndex-1];
        tempIndex--;
    }while((arc[arcIndex][1]<arc[trace[tempIndex]][1])&&(tempIndex!=0));


    if (arc[arcIndex][1]<arc[trace[tempIndex]][1]) trace[tempIndex] = tempNum;
    else trace[tempIndex+1] = tempNum;
    //else meaning the case that loop is broken by tempIndex=0,
    //but in fact the head node of current arc isn't smaller than trace[0]
}

void setPoint(int nodeNum, int arcNum, int **&point, float **arc, int *trace){
    int index; //record the arc index in outgoing
    for(int pointType=0; pointType<2; pointType++){
        for(int i=0; i<=nodeNum; i++){
            for(int arcIndex=0; arcIndex<arcNum; arcIndex++){
                if(pointType==0) index=arcIndex;
                else index=trace[arcIndex]; //transform the incoming index into the outgoing one

                //if current node is smaller than the tail/head node of current arc,
                //meaning this node doesn't be a tail/head node of any arc
                if(i<arc[index][pointType]-1){ //subtracted by 1 because the point index is starting from 0
                    point[i][pointType]=-1;
                    break;
                //current arcIndex is the start index of node i
                }else if(i==arc[index][pointType]-1){
                    point[i][pointType]=arcIndex;
                    break;
                }
            }
            //if the tail/head node is bigger than all tail/head node of arc,
            //meaning this node must be pointer to the end of arc array
            if(pointType==0) if(i>=arc[arcNum-1][pointType]) point[i][pointType]=arcNum;
            if(pointType==1) if(i>=arc[trace[arcNum-1]][pointType]) point[i][pointType]=arcNum;
        }
        //node doesn't be a tail/head node of any arc,
        //meaning the starting index of this node will be equal to the one of the next node
        for(int i=nodeNum-1; i>=0; i--) if(point[i][pointType]==-1) point[i][pointType]=point[i+1][pointType];
    }
}

void setBFS(vector<int> &reachedNode, int nodeNum, int sourceNode, int sinkNode, int **point, float **arc, int *trace, Node *&nodes){
    queue<Node> BFS;
    BFS.push(nodes[sourceNode-1]); //push source node into BFS
    nodes[sourceNode-1].isReach = 1; //update the reached status of BFS
    int nowNode = sourceNode-1; //record the index of node is in process
    reachedNode.push_back(nowNode); //record the node reached, if size of reachNode=0, meaning there is no reachable path

    while(!BFS.empty()){ //null BFS means all reachable node reached
        for(int i=point[nowNode][0]; i<point[nowNode+1][0]; i++){
            int chiNode=arc[i][1]-1;
            if(nodes[chiNode].isReach==0){ //if this node haven't been reached
                BFS.push(nodes[chiNode]);
                nodes[chiNode].isReach = 1; //update the reached status of BFS
                nodes[chiNode].arcN = nodes[nowNode].arcN + 1;
                reachedNode.push_back(chiNode);
            }
        }
        BFS.pop();
        nowNode=BFS.front().ID;
    }

    if(nodes[sinkNode-1].isReach==0){ //sink node doesn't reachable
        cout << "There is no reachable path." << endl;
        reachedNode.clear();
    }else{
        cout << "BFS: ";
        for(vector<int>:: iterator it = reachedNode.begin(); it!=reachedNode.end(); it++)
            cout << (*it)+1 << "[" << nodes[(*it)].arcN << "] ";
    }
    cout << '\n';
}

void setDFS(int nodeNum, int sourceNode, int sinkNode, int **point, float **arc, int *trace, Node *&nodes){
    vector<string> paths; //record each complete path
    stack<string> curPath; //record current path
    stack<Node> DFS;
    stack<int> *parNodes; //record parent of each node in each path in process
    parNodes = new stack<int> [nodeNum];
    bool isReachable=false;

    for(int i=0; i<nodeNum; i++) nodes[i].isReach=0; // initial reached status of each node

    DFS.push(nodes[sourceNode-1]); //push source node into DFS
    nodes[sourceNode-1].leng.push(0); //record length of source node

    int nowNode, //record the index of node is in process
        pathN=0, //record the number of path
        minIndex; //record the index of min length path
    float minDis=99999999; //record the min length

    while(1){
        nowNode=DFS.top().ID;
        if((isReachable==false)&&(nowNode!=sourceNode-1)){
            popNode(DFS, parNodes, curPath, nodes, nowNode);
            //DFS.pop();
        }
        DFS.pop();


        cout << "DFS node: " << nowNode+1;
        if(nowNode==3)cout<<"parent: " << parNodes[nowNode].top()<< endl;
        nodes[nowNode].isReach = 1; //update the reached status of DFS
        putNodeToCurpath(curPath, nowNode+1); //record this node in curPath

        if(nowNode==sinkNode-1){ //if the sink node is reached
            float curLeng = nodes[nowNode].leng.top(); //record the length of sink node
            if(curLeng<minDis) minDis=nodes[nowNode].leng.top(), minIndex=pathN; // update the shorted path

            string str= combineCurpath(curPath, curLeng); // transform the curPath to string
            paths.push_back(str);
            cout << str << '\n';
            pathN++;
            if(DFS.empty()) break; //null DFS means all path reached
            popNode(DFS, parNodes, curPath, nodes, nowNode); //pop node until the divergent node
            //continue; //reaching sink node implies no other node can be reached in this path
        }else{
            isReachable=false;
            for(int i=point[nowNode][0]; i<point[nowNode+1][0]; i++){
                int chiNode=arc[i][1]-1;

                if((nodes[chiNode].isReach==0)&&(!isCycle(parNodes, sourceNode, nowNode, chiNode))){ //if this node haven't been reached
                    parNodes[chiNode].push(nowNode); //record the parent node of child node
                    DFS.push(nodes[chiNode]);
                    nodes[chiNode].leng.push(nodes[nowNode].leng.top() + arc[i][2]);
                    cout << "chiNode="<<chiNode+1 << " ";
                    isReachable=true;
                }
            }
        }
    }
    cout << sourceNode << "->" << sinkNode << ": " << pathN << "paths\n";
    paths[minIndex].insert(0, "*"); //mark the shorted path
    for(vector<string>::iterator it=paths.begin(); it!=paths.end(); it++) cout << (*it) << '\n';
    delete [] parNodes;
}

void putNodeToCurpath(stack<string> &curPath, int node){
    stringstream tempStr;
    if(curPath.empty()) tempStr << "]: "; //only the beginning of path need to output this
    else tempStr << "-";
    tempStr << node;
    curPath.push(tempStr.str());
    tempStr.str(""); //clear the stringstream
}

string combineCurpath(stack<string> &curPath, int leng) {
    stringstream tempStr;
    tempStr.str(""); //initial stringsteam
    tempStr << "[" << leng; //put length into tempStr
    for(int i=curPath.size()-1; i>=0; i--) tempStr << *(&curPath.top()-i); //put the element in stack into tempStr from button to top
    return tempStr.str();
}

void popNode(stack<Node> DFS, stack<int> *&parNodes, stack<string> &curPath, Node *&nodes, int nowNode){
    int tempNode,
        lastNode=DFS.top().ID,
        endNode;
    if(nowNode==lastNode) endNode=*(&parNodes[lastNode].top()-1);//*(&parNodes.top()-1);
    else endNode=parNodes[DFS.top().ID].top();
    cout << parNodes[DFS.top().ID].top() +1 <<"dfsd";

    cout << "popNode=" << nowNode+1 <<" ,lastNode=" << lastNode+1;
    while(nowNode!=endNode){ // parNode != the parent of last node in DFS
        nodes[nowNode].isReach=0;
        nodes[nowNode].leng.pop();
        curPath.pop();
        tempNode=parNodes[nowNode].top(); //back to parent node
        parNodes[nowNode].pop();
        nowNode=tempNode;
    }
}

bool isCycle(stack<int> *parNodes, int sourceNode, int nowNode, int chiNode){
    while(nowNode!=sourceNode-1){
        if(!parNodes[nowNode].empty())
        if(parNodes[nowNode].top()==chiNode) return true;
        nowNode=parNodes[nowNode].top();
    }
    return false;
}
