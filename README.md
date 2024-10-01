GAMBIT-light
============

**GAMBIT-light** is a powerful yet easy-to-use tool for computationally difficult statistical fits and optimisation tasks. The user can provide their target/likelihood function as a Python, C, C++ or Fortran library.

Some features of GAMBIT-light:

  - A unified interface to a collection of powerful, MPI-parallelised sampling/optimisation algorithms
  - Simple interface for connecting user-supplied target/likelihood functions (and sampling priors, if needed) as runtime plugins 
  - Outputs in different file formats (binary or text)
  - Systems for exception handling and logging
  - Safe shutdown and resuming of aborted runs
  - Run configuration via a simple YAML file

GAMBIT-light is a spin-off project from **GAMBIT** (the Global And Modular BSM Inference Tool), https://gambitbsm.org/, a software tool for large-scale statistical fits in particle physics and astrophysics.


Citation(s)
--

Please cite the following GAMBIT papers if you use GAMBIT-light:

 - GAMBIT Collaboration: P. Athron, et al., **GAMBIT**: The Global and Modular Beyond-the-Standard-Model Inference Tool, Eur. Phys. J. C 77 (2017) 784, arXiv:1705.07908
 - GAMBIT Scanner Workgroup: G. D. Martinez, et al., Comparison of statistical sampling methods with **ScannerBit**, the GAMBIT scanning module, Eur. Phys. J. C 77 (2017) 761, arXiv:1705.07959


GAMBIT-light contains interfaces to some external codes, along with scripts for downloading and configuring them. Please cite as appropriate if you use those codes:

 - **Multinest:** F. Feroz, M. P. Hobson, M. Bridges, MULTINEST: an efficient and robust Bayesian inference tool for cosmology and particle physics, MNRAS 398 (2009) 1601–1614, arXiv:0809.3437
 - **PolyChord:** W. J. Handley, M. P. Hobson, M. P. A. N. Lasenby, POLYCHORD: next-generation nested sampling, MNRAS 453 (2015) 4384, arXiv:1506.00171


Supported Compilers and Library Dependencies
--

GAMBIT-light is built using the CMake system. The following libraries and packages must be installed prior to configuration:

**COMPULSORY**:

 - gcc >= 9 / llvm clang >= 10 / AppleClang >= 13 / icc >= 15.0.2
 - gfortran >= 9 / ifort >=15.0.2
 - Cmake 2.8.12 or greater
 - Python 3
 - Python modules: yaml, future, os, re, datetime, sys, getopt, shutil and itertools.
 - git
 - Boost 1.48 or greater
 - Eigen 3.1.0 or greater
 - GNU Scientific Library (GSL) 2.1 or greater
 - LAPACK
 - pkg-config

**OPTIONAL**:

 - HDF5 (for use of the hdf5 printer)
 - MPI (required for parallel sampling)
 - axel (speeds up downloads of scanners)
 - Python modules:
    - h5py (required to use hdf5 utilities located in gambit/Printers/scripts)


Building GAMBIT-light
--

GAMBIT-light itself is a C++ code. You build it once, and afterwards you connect your own target/likelihood function code to GAMBIT-light as a runtime plugin, in the form of a Python code or a C/C++/Fortran shared library. 

GAMBIT-light supports Linux and Mac OSX. (On Windows you can run it through WSL or Cygwin.) A full walkthrough of how to install all dependencies and build GAMBIT-light with AppleClang on OSX can be found in the file `README_OSX.md`.

Note that CMake might fail to find some dependencies on some systems without guidance. If you encounter problems configuring or building GAMBIT-light, have a look in `BUILD_OPTIONS.md` for a list of commonly used build options.

Here is a basic example of how to build GAMBIT-light and the full collection of "scanners" (sampling/optimisation libraries):

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DEIGEN3_INCLUDE_DIR=/path/to/eigen -DWITH_MPI=On -DCMAKE_CXX_COMPILER=g++-11 -DCMAKE_C_COMPILER=gcc-11 -DCMAKE_Fortran_COMPILER=gfortran-11 ..
make -jN scanners   # where N is the number of cores to use for the build, e.g. 4
cmake ..            # this step is needed for GAMBIT to detect the built scanners
make -jN gambit
```

(If you prefer to turn off compiler optimisations, use `-DCMAKE_BUILD_TYPE=None` instead of `-DCMAKE_BUILD_TYPE=Release`.)

Instead of building all the scanners (`make -jN scanners`) you can build each scanner you need individually, e.g.

```
make -jN diver
make -jN multinest
make -jN polychord
```

If for some reason you need to restart the build process, you can clean out the previous build as follows:

```
cd build
make nuke-all
cd ..
rm -r build
```


Running GAMBIT-light
--

A GAMBIT-light run is configured with a single YAML file. A fully commented example is provided in `yaml_files/gambit_light_example.yaml`. You can run GAMBIT-light with this example configuration by doing

```
./gambit -f yaml_files/gambit_light_example.yaml
```

To see a complete list of command-line options, do 
```
./gambit --help
```

When using an MPI-parallelised scanner, start GAMBIT-light with `mpiexec` or `mpirun`, e.g.

```
mpiexec -np 4 ./gambit -f yaml_files/your_configuration_file.yaml
```

for a run with 4 MPI processes.



Connecting your own target/likelihood function
--

For step-by-step instructions on how to connect your own Python/C/C++/Fortran code to GAMBIT-light, see the README files and example codes in the 
`gambit_light_interface/example_*` directories.



Common issues
--

- **Multiple Python installations**: If you have multiple versions of Python installed on your system, or you are working with a Python virtual environment, it is recommended to manually set the paths to the specific Python library and header files that GAMBIT-light should use. This can be done via the flags `PYTHON_INCLUDE_DIR` and `PYTHON_LIBRARIES` in the cmake command, e.g. like this:
  ```
  cmake -DPYTHON_INCLUDE_DIR=/usr/include/python3.10 -DPYTHON_LIBRARY=/usr/lib/x86_64-linux-gnu/libpython3.10.so (the rest of the cmake command) ..
  ```
  If needed, the `PYTHON_EXECUTABLE` flag can be used to ensure that GAMBIT-light also uses the correct Python executable, e.g. `-DPYTHON_EXECUTABLE=/usr/bin/python3`.

- **Multiple HDF5 libraries**: GAMBIT-light can fail to build or run due to conflicting versions of the HDF5 library. The most common source of this problem is that the Python package `h5py` has been built against an HDF5 version that differs from the version already installed on the system. One solution can be to reinstall `h5py` as follows:
  ```
  pip3 uninstall h5py
  pip3 install --no-binary=h5py h5py
  ```
  If needed, you can specify the specific HDF5 version that h5py should be built against:
  ```
  pip3 uninstall h5py
  HDF5_VERSION=1.10.7 pip3 install --no-binary=h5py h5py  
  ```


Licensing
--

The BSD license below applies to all source files in the GAMBIT-light distribution.

License
--
Copyright (c) 2017-2024, The GAMBIT Collaboration
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
