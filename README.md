# SISOP2
## Compile
```
make
```
## Run server
port_1: Server port for clients to connect.

port_2: Server port for receiving/sending client specific messages.
```
./server <port_1> <port_2>
```
ex:
```
./server 6000 7000
```
## Run client
user: username.

addr: server address.

port: server port for clients to connect.
```
./client <user> <addr> <port>
```
ex:
```
./client usuario localhost 6000
```
