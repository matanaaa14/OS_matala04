#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

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
void* createReactor(Reactor* reactor) {
    FD_ZERO(&reactor->masterSet);
    reactor->maxFd = -1;
    reactor->events = NULL;
    reactor->numEvents = 0;
    return reactor;
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
// Callback function for handling client messages
int handleClientMessage(int clientSocket) {
    char buffer[1024];
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead == -1) {
        perror("recv");
        return -1;
    } else if (bytesRead == 0) {
        // Client has closed the connection
        printf("Client disconnected\n");
        close(clientSocket);
        return -1;
    } else {
        buffer[bytesRead] = '\0';
        printf("Received message from client: %s\n", buffer);
        // You can perform further processing with the received message here
    }
    return 0;
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
                    reactorRegister(reactor, clientSocket, handleClientMessage);

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
    createReactor(&reactor);

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
    serverAddress.sin_port = htons(12347);

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

// Function that will be executed by the thread
void* threadFunction() {
    server();  // Call the server function
    return NULL;
}

int main() {
    pthread_t thread;  // Thread identifier

    // Create the thread
    if (pthread_create(&thread, NULL, threadFunction, NULL) != 0) {
        perror("pthread_create");
        return EXIT_FAILURE;
    }
    /*/
    // Wait for the thread to finish (optional)
    if (pthread_join(thread, NULL) != 0) {
        perror("pthread_join");
        return EXIT_FAILURE;
    }/*/
    /*/
    while(1){
        printf("in main\n");
        sleep(5);
    }/*/
    return 0;
}
