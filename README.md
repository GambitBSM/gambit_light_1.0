GAMBIT-light
============

GAMBIT-light is a powerful yet easy-to-use tool for computationally difficult statistical fits and global optimisation tasks, with user-supplied target/likelihood functions connected as Python, C, C++ or Fortran libraries. 

Some features of GAMBIT-light:

  - A unified interface to a collection of powerful, MPI-parallelised sampling/optimisation algorithms
  - Simple interface for connecting user-supplied target/likelihood functions (and sampling priors, if needed) as runtime plugins 
  - Outputs in different file formats (binary or text)
  - Systems for exception handling and logging
  - Safe shutdown and resuming of aborted runs
  - Run configuration via a simple YAML file

GAMBIT-light is a spin-off project from GAMBIT (the Global And Modular BSM Inference Tool), https://gambitbsm.org/, a software tool designed for performing large-scale statistical fits of new models in particle physics and astrophysics.


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

COMPULSORY:

 - gcc >= 5.1 / llvm clang >= 10 / AppleClang >= 13 / icc >= 15.0.2
 - gfortran >= 5.1 / ifort >=15.0.2
 - Cmake 2.8.12 or greater
 - Python 3
 - Python modules: yaml, future, os, re, datetime, sys, getopt, shutil and itertools.
 - git
 - Boost 1.48 or greater
 - GNU Scientific Library (GSL) 2.1 or greater
 - LAPACK
 - pkg-config

OPTIONAL:

 - HDF5 (for use of the hdf5 printer)
 - MPI (required for parallel sampling)
 - axel (speeds up downloads of backends and scanners)
 - Python modules:
    - numpy 1.12 or greater (required for using any python backend)
    - h5py (required to use hdf5 utilities located in gambit/Printers/scripts)


Building GAMBIT-light
--

GAMBIT-light itself is a C++ code that you only have to build once. Afterwards you connect your own target/likelihood function code to GAMBIT-light as a runtime plugin, in the form of a Python code or a C/C++/Fortran shared library. 

GAMBIT-light supports Linux and Mac OSX. (On Windows, you can run it through WSL or Cygwin.) A full walkthrough of how to install all dependencies and build GAMBIT-light with AppleClang on OSX can be found in the file README_OSX.md.

Note that cmake might fail to find some dependencies on some systems without guidance. If you encounter problems configuring or building GAMBIT-light, have a look in BUILD_OPTIONS.md for a list of commonly used build options.

Here is a basic example of how to build GAMBIT-light and full collection of "scanners" (sampling/optimisation libraries):

```
  mkdir build
  cd build
  cmake -DCMAKE_BUILD_TYPE=Release -DWITH_MPI=On -DCMAKE_CXX_COMPILER=g++-11 -DCMAKE_C_COMPILER=gcc-11 -DCMAKE_Fortran_COMPILER=gfortran-11 ..
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


Connecting your own target/likelihood function
--

For step-by-step instructions on how to connect your own Python/C/C++/Fortran code to GAMBIT-light, see the README files and example codes in the 
`gambit_light_interface/example_*` directories.


Running GAMBIT-light
--

A GAMBIT-light run is configured with a single YAML file. An commented example is provided in `yaml_files/gambit_light_example.yaml`. To run GAMBIT with this example configuration do

```
  ./gambit -f yaml_files/gambit_light_example.yaml
```

To see a full list of command-line options, do 
```
  ./gambit --help
```

When using an MPI-parallelised scanner, start GAMBIT with `mpiexec` or `mpirun`, e.g.

```
  mpiexec -np 4 ./gambit -f yaml_files/your_configuration_file.yaml
```



Licensing
--

The BSD license below applies to all source files in the GAMBIT-light distribution.

License
--
Copyright (c) 2017-2023, The GAMBIT Collaboration
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
