#include <stdio.h>
#include <pthread.h>

// Function that will be executed by the thread
void* threadFunction(void* arg) {
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

    // Wait for the thread to finish (optional)
    if (pthread_join(thread, NULL) != 0) {
        perror("pthread_join");
        return EXIT_FAILURE;
    }

    return 0;
}
