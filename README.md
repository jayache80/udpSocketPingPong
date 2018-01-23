# udpSocketPingPong

Demonstrate undefined behavior when multiple applications bind to the same UDP
port and are all waiting to receive data (with a blocking `recvfrom()` call)
