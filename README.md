# Building

```sh
mkdir build
cd build
cmake -USE_GUI=OFF ..
make
```

# Running the server

```sh
./bin/Server
```

The server binds to `0.0.0.0:5000`.

# Running the client

```sh
./bin/Client username serverip serverport
```

## Available commands

`FOLLOW username`

`SEND message`

When you send a message, it broadcasts it to the users who are following you.
