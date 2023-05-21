#include "reactor.h"

Reactor* reactor;
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
int main(){
    // Register the signal handler
    reactor = createReactor();
    printf("check1\n");

    signal(SIGINT, handleSignal);
    printf("check\n");

    startReactor(reactor);
    WaitFor(reactor);

    return 0;
}