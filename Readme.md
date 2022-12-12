# CommonLib

This is a common library of code that I share between my own personal projects hosted on my account. It includes the following:

- Barebones replacement of STL 
- - vector, unordered_map, strings, sorting
- Completely custom memory allocator mechanism
- Linear pool allocator currently using windows for reserver/committed memory
- Testing framework
- Error reporting, assertions and logging
- A reasonably well featured memory tracker and leak detector
- Json parser and file scanning utilities

It's designed *very* differently to the normal C++ you're used to. It's much more C-like, is very very fast when used correctly and is very tuned for my needs. I will provide a bit more documentation in time.

