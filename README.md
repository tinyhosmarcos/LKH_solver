# Large-TSP-batches-solver
Solving large batches of traveling salesman problems with parallel and distributed computing

## Distributed
g++ -o server.exe serverTCP.cpp -lpthread -lstdc++fs  
g++ -o client.exe clientTCP.cpp -lpthread -lstdc++fs  

./server.exe PORT  (./server.exe 8000)  
./client.exe PORT IP NUM_CORES (./client.exe 8000 127.0.0.1 4)  
