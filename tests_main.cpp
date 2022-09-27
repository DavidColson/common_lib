
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
    // Array myArray;
    // myArray.append(5);
    return 0;
}  

// TODO: 
// 1. Make a dynamic array class
// 2. Make a custom allocator mechanism for the dynamic array class
// 3. Make a dynamic string class inspired by bx stringT and built on the dynamic array 
// 4. Make a string view class
// 5. Make a simple hash map class
// 6. Make a memory tracker using custom allocator
// 7. Make other allocators (such as jai's pool alloc)


// Re: Allocators (point 2)
//
// I have the feeling that a virtual function allocator interface is actually going to be fine
// So we can avoid the template allocator parameter. And just have set/get allocator functions on containers
// Things that will free much later than they alloc must store the allocator for later freeing
// Everything else that does mem alloc should take an allocator as a parameter to make it clear they're doing mem ops
// I think we should have a global mem allocator object that anyone can access
// Regarding global context stack, we can probably play with this as a bonus if needed, but lets see how far we get without it first please


// Re: Tests
// https://github.com/maghoff/jsonxx/tree/master/tests

int main() {
    printf("hello world");
    return 0;
}