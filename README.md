# SISOP2
## Compile
```
make
```
## Run main server
port_1: Server port for clients/backups to connect.

port_2: Server port for receiving/sending client specific messages.

port_3: Server port for receiving/sending backup servers specific messages.
```
./server <port_1> <port_2> <port_3>
```
ex:
```
./server 8000 8001 8002
```
## Run backup server
port_1: Server port for clients to connect (if it becomes the main server).

port_2: Server port for receiving/sending client specific messages (if it becomes the main server).

port_3: Server port for receiving/sending backup servers specific messages (if it becomes the main server).

port_4: Server port for communicating with main server and other backups (during election).

addr: Main server address.

port: Main server port for backups to connect.
```
./server <port_1> <port_2> <port_3> <port_4> <addr> <port>
```
ex:
```
./server 8003 8004 8005 8006 localhost 8000
```
## Run client
user: Username.

addr: Main server address.

port: Main server port for clients to connect.
```
./client <user> <addr> <port>
```
ex:
```
./client usuario localhost 8000
```
