# UDP sockets in C

## Gotchas

* **If the client fails to connect to the server or keeps waiting a response, kill the client and start it again (do not kill the server). That fixes the problem.**

* **It only sends strings, not DNS-like binary messages.**

## Build

This creates `./server` and `./client` on the current directory:

```sh
make
```

## Run Server

Run default server:

```sh
./server
```

Run with custom `.dns` file:

```sh
./server custom.dns
```

## Run Client

Run default client:

```sh
./client
```

Run with custom server IP:

```sh
./client 192.168.1.10
```

## Clean project

Remove binaries and temporal files:

```sh
make clean
```
