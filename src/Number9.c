#include <string.h>
#include <stdio.h>

const char *timeStrings[] = {
        "19:12", 
        "19:42", 
};

// Define the structure for a key-value pair
typedef struct {
    int key;   // Key (integer index)
    int value; // Value (integer value)
} KeyValuePair;

// Define an array with one key-value pair
KeyValuePair stopOffsets[] = {
  {71779, 4}
};
