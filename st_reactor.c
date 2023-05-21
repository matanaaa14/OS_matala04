#include "reactor.h"


//Reactor* reactor;

// Signal handler function
/*/
void handleSignal(int signal) {
    if (signal == SIGINT) {
        stopReactor(reactor);
        printf("Ctrl+C signal received. Exiting...\n");
        // Perform cleanup or other necessary actions
        // Free the allocated memory in the Reactor structure
        for (int i = 0; i < reactor->numEvents; i++) {
            free(reactor->events[i]);
        }
        free(reactor->events);

        // Exit the program
        exit(0);
    }
}
/*/
// Initialize the reactor
void* createReactor() {
    Reactor* reactor;
    reactor=(Reactor*)malloc(sizeof(reactor));
    printf("check2\n");
    FD_ZERO(&reactor->masterSet);
    reactor->maxFd = -1;
    reactor->events = NULL;
    reactor->numEvents = 0;
    reactor->isRunning = 0;

    return reactor;
}

// Register an event with the reactor
void addFD(Reactor* reactor, int fd, int (*callback)(int)) {
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
void stopReactor(void* this){
    Reactor* react = (Reactor*) this;
    if(react->isRunning == 0)
        return;
    pthread_cancel(react->thread);
    react->isRunning = 0;
}
// Run the reactor event loop
void reactorRun(Reactor* reactor, int serverSocket) {
    int clientSocket;
    reactor->isRunning = 1;
    while (1) {
        fd_set readSet = reactor->masterSet;
        int numReady = select(reactor->maxFd + 1, &readSet, NULL, NULL, NULL);
        if (numReady == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }
        // Handle the ready events
        if(numReady != 0){
        for (int i = 0; i < reactor->numEvents; i++) {
            Event* event = reactor->events[i];
            if (FD_ISSET(event->fd, &readSet)) {
                if(event->fd == serverSocket){
                    clientSocket = eventCallback2( serverSocket);
                    addFD(reactor, clientSocket, handleClientMessage);

                }
                else{
                event->callback(event->fd);
            }
            }
        }}
        numReady = 0;
    }
}


int server(Reactor* reactor){
   // Reactor reactor;
    //signal(SIGINT, handleSignal);
    // Create a server socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    printf("serverSocket is: %d\n",serverSocket);
    // Set the SO_REUSEADDR option
    int reuse = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Set up the server address
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(9034);

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
    addFD(reactor, serverSocket, eventCallback2);

    // Run the reactor
    reactorRun(reactor,serverSocket);

    return 0;
}

// Function that will be executed by the thread
void* threadFunction(void* this) {
    Reactor* reactor = this;
    server(reactor);  // Call the server function
    return NULL;
}
void* WaitFor(void* this){
    Reactor* react = (Reactor* ) this;
    // Wait for the thread to finish (optional)
    if (pthread_join(react->thread, NULL) != 0) {
        perror("pthread_join");
        exit(1);
    }
    return NULL;
}
void startReactor(void* this){

    Reactor* react = (Reactor*) this;

    // Create the thread
    if (pthread_create(&react->thread, NULL, threadFunction, (void*)react) != 0) {
        perror("pthread_create");
        exit(1);
    }
    
}/*/
int main() {
    // Register the signal handler
    createReactor();
    printf("check1\n");

    //signal(SIGINT, handleSignal);
    printf("check\n");

   // startReactor(reactor);
   // WaitFor(reactor);

    return 0;
}
/*/