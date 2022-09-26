
#include "stdio.h"


// Core Types
typedef unsigned int uint;


struct Array {
    char* pData{ nullptr };
    uint count{ 0 };

    // operator []?
    // Consider what this is even solving?

};


int arrayTest() {
    // Create Allocator
    Array myArray;
    myArray.append(5);
}  

// TODO: 
// 1. Make a dynamic array class
// 2. Make a custom allocator mechanism for the dynamic array class
// 2a. Toy with global context allocator objects 
// 2b. Toy with global allocator stack to push/pop allocators for all to access
// 3. Make a dynamic string class inspired by bx stringT and built on the dynamic array 
// 4. Make a string view class
// 5. Make a simple hash map class
// 6. Make a memory tracker using custom allocator
// 7. Make other allocators (such as jai's pool alloc)

int main() {
    printf("hello world");
    return 0;
}