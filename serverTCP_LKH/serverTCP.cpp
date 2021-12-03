/* Server code in C++ */
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <thread>
#include <map>
#include <sstream>
#include <mutex>
#include <experimental/filesystem>
using namespace std;
namespace fs = std::experimental::filesystem::v1;

#define PASSWORDSERVER "ucsp"


//global variables
map<int ,int> listMachines;
map<int,int> UserAttemps;
mutex mutexListMachines;



void threadConnection(int ConnectFD,vector<string> paths){
    cout<<"[LOG]: connectFD"<<ConnectFD<<endl;
    fstream file;
    int n,coresNumber;

    string message_client(3,0);
    n=read(ConnectFD,&message_client[0],1); //1 bytes
    //num cores 
    cout<<"[LOG]: NUM CORES "<<message_client<<"\n";
    stringstream geek(message_client); //mesage_client es cores NUmber
    geek>>coresNumber; //String to int

    lock_guard<mutex> guard(mutexListMachines);
    listMachines[ConnectFD]=coresNumber;
    mutexListMachines.unlock();

    //esperando dos esclavos
    while(listMachines.size()<2) {
        sleep(1);
    }
    
    int sumCores=8; // dos maquinas
    int numTSP_condition;

    double percentajeTSP=(double)listMachines[ConnectFD]/sumCores;
    cout<<"[LOG] : Percentaje TSP. "<<percentajeTSP<<endl;
    
    numTSP_condition=paths.size()*percentajeTSP;
    cout<<"[LOG] : Num TSP for machine. "<<numTSP_condition<<endl;

    //send numTSP_CONDITION
    n = write(ConnectFD,to_string(numTSP_condition).c_str(),1);


    if(ConnectFD==listMachines.begin()->first){
        for(vector<string>:: iterator it=paths.begin();it!=paths.end() and numTSP_condition>0;++it ){    
            --numTSP_condition;
            cout<<"[LOG] name_file:" <<*it<<endl;
            file.open(*it,ios::in | ios::binary);
            if(file.is_open()){
                cout<<"[LOG] : File is ready to Transmit.\n";
            }
            else{
                cout<<"[ERROR] : File loading failed, Exititng.\n";
                exit(EXIT_FAILURE);
            }
            //two files sending
            std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            cout<<"[LOG] : Transmission Data Size "<<contents.length()<<" Bytes.\n";

            cout<<"[LOG] : Sending...\n";

            int bytes_sent = send(ConnectFD , contents.c_str() , 6000 , 0 );
            cout<<"[LOG] : Transmitted Data Size "<<bytes_sent<<" Bytes.\n";

            cout<<"[LOG] : File Transfer Complete.\n";
            file.close();
        }
    }
    
    else{
        for(vector<string>:: reverse_iterator it=paths.rbegin();it!=paths.rend() and numTSP_condition>0;++it ){    
            --numTSP_condition;
            cout<<"[LOG] name_file:" <<*it<<endl;
            file.open(*it,ios::in | ios::binary);
            if(file.is_open()){
                cout<<"[LOG] : File is ready to Transmit.\n";
            }
            else{
                cout<<"[ERROR] : File loading failed, Exititng.\n";
                exit(EXIT_FAILURE);
            }
            //two
            std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            cout<<"[LOG] : Transmission Data Size "<<contents.length()<<" Bytes.\n";

            cout<<"[LOG] : Sending...\n";

            int bytes_sent = send(ConnectFD , contents.c_str() , 6000 , 0 );
            cout<<"[LOG] : Transmitted Data Size "<<bytes_sent<<" Bytes.\n";

            cout<<"[LOG] : File Transfer Complete.\n";
            file.close();
        }
    }
    //read score
    fstream score_file;
    score_file.open("final_score_"+to_string(ConnectFD)+".txt", ios::out | ios::trunc | ios::binary);
    if(score_file.is_open()){
        cout<<"[LOG] : File Creted.\n";
    }
    else{
        cout<<"[ERROR] : File creation failed, Exititng.\n";
        exit(EXIT_FAILURE);
    }
    char buffer[6000] = {};
    int valread = read(ConnectFD , buffer, 6000);
    cout<<"[LOG] : Data received "<<valread<<" bytes\n";
    cout<<"[LOG] : Saving data to file.\n";

    score_file<<buffer;
    cout<<"[LOG] : File Saved.\n";
    score_file.close();

    shutdown(ConnectFD, SHUT_RDWR);
    close(ConnectFD);
}

int main(int argc, char** argv){
    int port;
    stringstream geek(argv[1]);
    geek>>port;   
    struct sockaddr_in stSockAddr;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(-1 == SocketFD){
        perror("can not create socket");
        exit(EXIT_FAILURE);
    }
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(port);
    stSockAddr.sin_addr.s_addr = INADDR_ANY;
    if(-1 == bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in))){
        perror("error bind failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    if(-1 == listen(SocketFD, 10)){
        perror("error listen failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

     // Parallalel Solver
    string path = "dataset"; // Folder containing the cvs. files
    vector<string> paths; // Store all cvs. files paths
    // Collect the paths of all csv files within dataset
    for (const auto & entry : fs::directory_iterator(path)){
        //cout << "Entrypath"<<entry.path() << endl;
        string path_string = entry.path().string();
        paths.push_back(path_string);
    }    
    cout<<"[LOG]: Read Dataset size:"<<paths.size()<<endl;
    for(;;){
        int ConnectFD = accept(SocketFD, NULL, NULL);
        thread(threadConnection,ConnectFD,paths).detach();
    }
}
