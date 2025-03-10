# CommonLib

This is a common library of code that I share between my own personal projects hosted on my account. It includes the following:

- Barebones replacement of STL 
- - ResizableArray, Stacks, Open-addressed Hashmap, Strings, Sorting
- Custom memory allocator system built around using arena's for lifetime grouping
- - Built directly on VirtualAlloc so you can get stable pointers
- Testing framework
- Error reporting, assertions and logging
- A reasonably well featured memory tracker and leak detector
- Callstack collector and resolution for Win32
- Json parser and file scanning utilities
- Filesystem API and File IO for Win32
- Basic 3D math stuff, such as vectors, matrices, quats, AABBs and so on
- Base64 encoder and decoder

Wishlist:
- Cross platform windowing
- Input/Gamepad handling
- Threading and sync primitives
- Cross platform audio

It's designed *very* differently to the normal C++ you're used to. It's much more C-like, is very very fast when used correctly and is very tuned for my needs. I will provide a bit more documentation in time.

