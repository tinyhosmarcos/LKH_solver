# Large-TSP-batches-solver
Solving large batches of traveling salesman problems with parallel and distributed computing


## Serial part
g++ -o solver.exe serial_solver.cpp

.\solver  

## Parallel part
g++ --o solver.exe parallel_solver.cpp -pthread

.\solver

## Distributed
g++ -o server.exe serverTCP.cpp -lpthread -lstdc++fs  
g++ -o client.exe clientTCP.cpp -lpthread -lstdc++fs  

##Client 
 Instruccions install solver:    
 tar xvfz LKH-2.0.9.tgz  
 cd LKH-2.0.9  
 make  

./server.exe PORT  (./server.exe 8000)  
./client.exe PORT IP NUM_CORES (./client.exe 8000 127.0.0.1 4)  
