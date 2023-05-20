#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
// Event structure
typedef struct {
    int fd;  // File descriptor or event identifier
    int (*callback)(int);  // Callback function to handle the event
} Event;

// Reactor structure
typedef struct {
    fd_set masterSet;  // Master set of file descriptors
    int maxFd;  // Maximum file descriptor value
    Event** events;  // Array of events
    int numEvents;  // Number of registered events
} Reactor;

// Initialize the reactor
void reactorInit(Reactor* reactor) {
    FD_ZERO(&reactor->masterSet);
    reactor->maxFd = -1;
    reactor->events = NULL;
    reactor->numEvents = 0;
}

// Register an event with the reactor
void reactorRegister(Reactor* reactor, int fd, int (*callback)(int)) {
    Event* event = (Event*)malloc(sizeof(Event));
    event->fd = fd;
    event->callback = callback;

    // Add event to the array
    reactor->events = (Event**)realloc(reactor->events, (reactor->numEvents + 1) * sizeof(Event*));
    reactor->events[reactor->numEvents] = event;
    reactor->numEvents++;

    // Update the master set and maxFd
    FD_SET(fd, &reactor->masterSet);
    if (fd > reactor->maxFd) {
        reactor->maxFd = fd;
    }
}
// Example callback function
int eventCallback(int fd) {
    printf("Event occurred on file descriptor %d\n", fd);
    return 0;
}
int eventCallback2(int serverSocket) {
        
        // Accept incoming connection
        struct sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if (clientSocket == -1) {
            perror("accept");
            return -1;
        }

        // Do something with the client socket
        printf("Accepted connection from %s:%d\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
        // ... Handle the client socket

        // Register the client socket event
        return clientSocket;
}
// Run the reactor event loop
void reactorRun(Reactor* reactor, int serverSocket) {
    int clientSocket;
    while (1) {
        fd_set readSet = reactor->masterSet;
        int numReady = select(reactor->maxFd + 1, &readSet, NULL, NULL, NULL);
        if (numReady == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }
        printf("while: %d\n", numReady);
        // Handle the ready events
        if(numReady != 0){
        for (int i = 0; i < reactor->numEvents; i++) {
            Event* event = reactor->events[i];
            if (FD_ISSET(event->fd, &readSet)) {
                if(event->fd == serverSocket){
                    clientSocket = eventCallback2( serverSocket);
                    reactorRegister(reactor, clientSocket, eventCallback);

                }
                else{
                    printf("loop\n");
                event->callback(event->fd);
            }
            }
        }}
        numReady = 0;
    }
}


int server(){
    Reactor reactor;
    reactorInit(&reactor);

    // Create a server socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    printf("serverSocket is: %d\n",serverSocket);
    // Set up the server address
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(12345);

    // Bind the server socket to the address
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Register the server socket event
    reactorRegister(&reactor, serverSocket, eventCallback2);

    // Run the reactor
    reactorRun(&reactor,serverSocket);

    return 0;
}


int main() {
    server();
    /*/
    Reactor reactor;
    reactorInit(&reactor);

    // Register events
    reactorRegister(&reactor, 0, eventCallback);
    reactorRegister(&reactor, 1, eventCallback);
    reactorRegister(&reactor, 2, eventCallback);

    // Run the reactor
    reactorRun(&reactor);

    return 0;
    /*/
}
