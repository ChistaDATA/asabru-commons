# asabru-commons
### This project is a shared static library used in the ChistaDATA Database Proxy ( chista-asabru ) 

This project contains common utility classes used in the chistadata database proxy 

### Prerequisite

Make
`https://www.gnu.org/software/make`

CMake
`https://cmake.org/install/`

### Build

build the asabru app from  the root of the repository

```
mkdir build
cd build
cmake ..
make
```

### Run Tests

```
cd build
ctest
```

### Coverage

```
cd build
cmake ..;make;
make gcov;
make coverage;
```

The coverage report will be generated at build/gcoverage/coverage.html

