#ifndef REACTOR_H
#define REACTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

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
    int isRunning;
    pthread_t thread;  // Thread identifier

} Reactor;


// Signal handler function
//void handleSignal(int signal);

// Initialize the reactor
void* createReactor();

// Register an event with the reactor
void addFD(Reactor* reactor, int fd, int (*callback)(int));

// Example callback function
int eventCallback(int fd);

// Callback function for handling client connections
int eventCallback2(int serverSocket);

// Callback function for handling client messages
int handleClientMessage(int clientSocket);

// Stop the reactor event loop
void stopReactor(void* yuval);

// Run the reactor event loop
void reactorRun(Reactor* reactor, int serverSocket);

// Start the reactor thread
void startReactor(void* yuval);

// Wait for the reactor thread to finish
void* WaitFor(void* yuval);

// Server function
int server();
#endif /* REACTOR_H */
