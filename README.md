# Biggestwar Server
Biggestwar videogame server

## How to start
### Compile
```bash
sudo apt-get install libpq-dev libwebsocketpp-dev libboost-dev libpng-dev libjpeg-dev libboost-filesystem-dev
make clean
make
```

### Make de database
The database is a postgres in a docker, for run the database do:  postgres_docker
```bash
cd /data/postgres_docker
sudo sh make_container.sh
sudo sh start.sh
```

### Run it!
```bash
sh run.sh
```

## Some useful commands
### Handle the world servers and the main server
```bash
./serverAdmin admin
```

### Close all the process (login and world process)
```bash
./serverAdmin closeall
```
### Run a world server process
```bash
./serverWorld <uri> <port>
```
For run a local process:
```bash
./serverWorld ws://127.0.0.1 8889
```
### Run the login process
```bash
./serverLogin <port>
```
Example:
```bash
./serverLogin 8888
```

## Structure Summary

**Makefile:** For compile the three process server (serverWorld, serverLogin, serverAdmin)

**src/:** The source directory

**run.sh:** Execute the server, run the main process and also run a world process.

**src/config.h:** All the server and game configuration

**src/configPrivate.h:** The sensitive configuration, I didnt upload :)

**obj/:** Where is keeping the object compile files

**semfiles/:** Directory where there are files for generate a lot of semaphores

**data/postgres_docker:** Docker postgres server

**data/postgres_docker/scripts/notexecute/model.sql:** SQL for make the database server and add the initial data

**data/replays/:** The folder where is keeping the replay data of each game, these replays are used for train a AI agent.

**borrarsemaforos.sh:** Delete the user's sempahores, you need to modify it with your username.

**enablecoredump.sh:** Enable the core dump generation
