# # Primary 127.0.0.1:5000
# PEERS=127.0.0.1:5001:s,127.0.0.1:5002:s,127.0.0.1:5003:s BIND_IP=0.0.0.0 BIND_PORT=5000 PRIMARY=true ./bin/Server

# # Secondary 1 127.0.0.1:5001
# PEERS=127.0.0.1:5000:p,127.0.0.1:5002:s,127.0.0.1:5003:s BIND_IP=0.0.0.0 BIND_PORT=5001 PRIMARY=false ./bin/Server

# # Secondary 2 127.0.0.1:5002
# PEERS=127.0.0.1:5000:p,127.0.0.1:5001:s,127.0.0.1:5003:s BIND_IP=0.0.0.0 BIND_PORT=5002 PRIMARY=false ./bin/Server

# # Secondary 3 127.0.0.1:5003
# PEERS=127.0.0.1:5000:p,127.0.0.1:5001:s,127.0.0.1:5002:s BIND_IP=0.0.0.0 BIND_PORT=5003 PRIMARY=false ./bin/Server


# # Primary 127.0.0.1:6000
# PEERS=127.0.0.1:6001:s BIND_IP=0.0.0.0 BIND_PORT=6000 PRIMARY=true ./bin/Server

# # Secondary 1 127.0.0.1:6001

# valgrind --leak-check=full --track-origins=yes ./bin/Server


#primary
PEERS=127.0.0.1:5001:s,127.0.0.1:5000:s BIND_IP=0.0.0.0 BIND_PORT=5002 PRIMARY=true ./bin/Server

PEERS=127.0.0.1:5002:p,127.0.0.1:5000:s BIND_IP=0.0.0.0 BIND_PORT=5001 PRIMARY=false ./bin/Server
PEERS=127.0.0.1:5002:p,127.0.0.1:5001:s BIND_IP=0.0.0.0 BIND_PORT=5000 PRIMARY=false ./bin/Server