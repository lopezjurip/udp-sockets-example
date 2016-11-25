# T5

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
