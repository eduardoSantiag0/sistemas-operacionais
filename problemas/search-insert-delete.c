#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>



// Searchers - Read the list without modifying it
// Inserters - Add new elements to the list
// Deleters - Remove elements from the list

// ✓ Searchers can work concurrently with each other
// ✓ Searchers can work concurrently with Inserters
// ✗ Inserters cannot work with other Inserters (may corrupt structure)
// ✗ Deleters need exclusive access (no Searchers, Inserters, or other Deleters)

int main ()
{
    return 0;
}