/*
   This code can be compiled and run ok.

   purpose:
     This is to read the network data from txt/sp format, and output the arcs connecting to the source node.

   input file:
     test1.sp, test2.sp

   compile:
     g++ -o h34051178_hw1 h34051178_hw1.cpp

   pseudocode:
   use forward and reverse star data structure
   ======
   sub readfileAndIni()
   ------
   read data from txt/sp format
   build and initial point[],arc[]=0
   build and initial trace[i]=i
   call setTrace and setPoint
   ======
   sub setTrace(arc,arc[],trace[])
   ------
   (initial trace[i]=i in sub readfileAndIni)
   if the tail node of arc < trace[i]
   then move this arc forward in trace array
   ======
   sub setPoint(point[],arc[],trace[])
   ------
   for i=0 to nodeNum
     for j=0 to arcNum
       if i < the tail/head node of arc j
       then set point[i]/rpoint[i] = point[i+1]/rpoint[i+1] and break this loop
       if i = the tail/head node of arc j
       then set point[i]/rpoint[i]=j and break this loop
     end loop
   end loop
   ======
   sub print(sourceNode,point[],arc[],trace[])
   ------
   for each arc connecting to source node
     add arc to a set
   end loop
   print all element in the set
   ======

   coded by Yu-En Shen, ID: h34051178, email: fourfire11@gmail.com
   date: 2020.03.24
*/
#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <stdio.h>
#include <set>
using namespace std;
void readfileAndIni(string &proName, int &nodeNum, int &arcNum, int **&point, float **&arc, int *&trace);
void setPoint(int nodeNum, int arcNum, int **&point, float **arc, int *trace);
void setTrace(int arcIndex, float **arc, int *&trace);
void print(int sourceNode, int **point, float **arc, int *trace);

int main(){
    string proName;

    int nodeNum,    //number of node
        arcNum,     //number of arc
        **point,    //2D-array to record point and rpoint of each node
        *trace,     //record trace of each arc
        sourceNode; //we will print all arcs connecting to this node
    float **arc;    //2D-array to record tail node, head node, and length of each arc

    readfileAndIni(proName, nodeNum, arcNum, point, arc, trace);

    //ask user to input source node, and determine this node whether valid or not
    cout << "Please input a source node: ";
    cin >> sourceNode;
    if(sourceNode>nodeNum) cout << "!!Warning!!: node " << nodeNum << " does not exist"<< endl;
    else print(sourceNode, point, arc, trace);

    for(int i=0; i<=nodeNum+1; i++) delete[] point[i];
    delete [] point;
    for(int i=0; i<arcNum; i++) delete[] arc[i];
    delete [] arc;
    delete [] trace;

    system("pause");
    return 0;
}

void print(int sourceNode, int **point, float **arc, int *trace){
    set<int> printNum; //storing arc index connecting to source node
    int index;         //record the arc index in outgoing

    for(int pointType=1; pointType>=0; pointType--){
        for(int i=point[sourceNode-1][pointType]; i<point[sourceNode][pointType]; i++){
            if(pointType==0) index=i; //put outgoing arcs into printNum
            else index=trace[i]; //transform the incoming index into the outgoing one, and put incoming arcs into printNum
            printNum.insert(index);
        }
    }

    //print all element in printNum
    for(set<int>::const_iterator iter = printNum.begin() ; iter != printNum.end() ; ++iter){
        cout << "arc[" << (*iter+1) << "]: " << fixed << setprecision(0) << arc[*iter][0] << "-" << arc[*iter][1] << " : " << fixed << setprecision(2) << arc[*iter][2] << endl;
    }
}

void readfileAndIni(string &proName, int &nodeNum, int &arcNum, int **&point, float **&arc, int *&trace){
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
            for(int i=0; i<arcNum;i++) trace[i]=i;
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

