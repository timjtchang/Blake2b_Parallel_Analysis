# EE 451 Final Project - Implicit vs. Explicit Parallelization - Blake2b Hashing Function
    Group Members:
        - Jake Hosking
        - Tim Chang

## Files

    Within this .zip file is included 3 versions of our blake2b implementation:

    blake2b_serial.cpp
        A serial implementation of the hashing algorithm which simulates no implicit 
        parallelization or optimizations

    blake2b_optimized.cpp
        The optimized and best performing version of blake2b, with implicit parallelizations 
        and optimizations included

    blake2b_parallel.cpp
        The parallelized version of blake2b_optimized.cpp, using pthreads and OpenMP to explicitly 
        parallize the Blake2b hashing algorithm

    In addition there is:

    test.cpp
        Testing code writen to test the operating time of all 3 blake2b algorithms
        Can be easily modified within the main code block and the global variables at the top 
        of the file to change which tests are completed,the test parameters, and which file 
        you want to be hashed

    Makefile
        The primary makefile for the project
        Running "make test_serial" will test blake2b_serial.cpp
        Running "make test" will test blake2b_optimized.cpp
        Running "make test_par" will test blake2b_parallel.cpp

    blake2.pdf
        Testing file for Medium input data

    test-large-file.pdf 
        Testing file for Large input data

    READ_ME.txt
        What you are reading currently :)

    blake2-impl.h
    blake2.h
        Header files required to make the .cpp file operate

    testvectors/blake2-kat.h
        Test input vectors used in some tests of the code

## Using our blake2b code as a library

    Our code can easily be added to any current C++ project
    Simply add both of the header files to the top of your project with #include
    Then add the .cpp of the version you would like to use to your makefile dependency list 
    (use the Makefile and test.cpp as an example)