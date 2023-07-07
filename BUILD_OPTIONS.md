Useful cmake options for building GAMBIT-light
--

Below are examples of commonly used options for the GAMBIT-light cmake build system. You pass these to cmake using the `-D` flag, e.g. `cmake -DOPTION1=value1 -DOPTION2=value2 ..`

For a more complete list of cmake variables, take a look in the file `CMakeCache.txt` which is generated in your build directory when you first run `cmake ..`

```

# Set the build type: CMAKE_BUILD_TYPE (Release|Debug|None)
-DCMAKE_BUILD_TYPE=None

# Switch MPI on/off: WITH_MPI (On|Off)
-DWITH_MPI=On

# Set the C compiler: CMAKE_C_COMPILER
-DCMAKE_C_COMPILER=/usr/bin/gcc-8

# Additional C compiler flags: CMAKE_C_FLAGS
-DCMAKE_C_FLAGS=-your-flags-here

# Set the C++ compiler: CMAKE_CXX_COMPILER
-DCMAKE_CXX_COMPILER=/usr/bin/g++-8

# Additional C++ compiler flags: CMAKE_CXX_FLAGS
-DCMAKE_CXX_FLAGS=-your-flags-here

# Set the Fortran compiler: CMAKE_Fortran_COMPILER
-DCMAKE_Fortran_COMPILER=/usr/bin/gfortran-8

# Additional Fortran compiler flags: CMAKE_Fortran_FLAGS
-DCMAKE_Fortran_FLAGS=-your-flags-here

# Switch verbose build output on/off: CMAKE_VERBOSE_MAKEFILE (On|Off)
# (Useful for debugging build problems.)
-DCMAKE_VERBOSE_MAKEFILE=On

# Set the Python executable: PYTHON_EXECUTABLE
# If working from a virtual environment, cmake will automatically set this as
# the executable in the environment's directory. It may, however, have trouble
# finding the system Python library and include directory of the same version, so
# setting PYTHON_LIBRARY and PYTHON_INCLUDE_DIR manually is encouraged.
-DPYTHON_EXECUTABLE=/usr/bin/python3

# Set the Python include directory: PYTHON_INCLUDE_DIR
-DPYTHON_INCLUDE_DIR=/usr/include/python3.7m

# Set the Python library: PYTHON_LIBRARY
-DPYTHON_LIBRARY=/usr/lib/x86_64-linux-gnu/libpython3.7m.so

```

