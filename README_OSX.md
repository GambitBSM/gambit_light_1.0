GAMBIT-light: building on Mac OSX with AppleClang
===========================================

This file supplements the main README.md with a full walkthrough of the steps required to get GAMBIT-light up and running on a clean Mac Mini with an M1 (arm64) processor.  The steps should however be applicable to other Macs as well.

We assume in this walkthrough that you want to use the AppleClang compiler that ships with OSX.  If you want to use gcc instead, you can certainly do that, it's just more work.  You need to install it, install whatever dependencies are relevant for you, and then follow the instructions in README.md. Be careful to specify that you want to use gcc everywhere; you may run into linking problems if you build some dependencies accidentally with clang.  In general it is easier to just build using the built-in AppleClang.

Install dependencies
--
- Install clang
```
Xcode-select --install
```

Upgrade to latest version of pip (numpy will fail to install if you don't)
```
python3 -m pip install --upgrade pip
```

NB: You must use the system python3 (and pip3 in the steps below)! In case of doubt (if you have installed other python versions) check with 'which python3' and 'which pip3' what python3/pip3 points to, and use 
```
/full/path/to/the/system/python3
/full/path/to/the/system/pip3
```
instead.

- Install homebrew
```
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> /Users/$USER/.zprofile
eval "$(/opt/homebrew/bin/brew shellenv)"
```

- Install dependencies
```
brew install cmake
brew install gfortran
brew install libx11
brew install boost
brew install eigen
brew install libomp
brew install gsl
brew install ossp-uuid
brew install hdf5@1.10
pip3 install numpy
pip3 install pyyaml
```
- Optional: For parallel sampling
```
brew install openmpi
brew install mpi4py
```

- Fix paths:
```
echo 'export PATH="/opt/homebrew/opt/hdf5@1.10/bin:$PATH"' >> ~/.zprofile
. ~/.zprofile
```

Build GAMBIT-light
--
**Follow instructions in README.md.**

Note that CMake might fail to find some dependencies on some systems without guidance, especially OSX systems. If you encounter problems configuring or building GAMBIT-light, have a look in BUILD_OPTIONS.md for a list of commonly used build options. 

Example cmake command:
```
cmake -D PYTHON_EXECUTABLE=/usr/bin/python3 -D PYTHON_LIBRARY=/Library/Developer/CommandLineTools/Library/Frameworks/Python3.framework/Versions/Current/Python3 -D PYTHON_INCLUDE_DIR=/Library/Developer/CommandLineTools/Library/Frameworks/Python3.framework/Headers ..
```

Key optional extras:
  `-D WITH_MPI=ON` if you want scanners to be parallelised

Common OSX Issues
--

- **HDF5 Issues**. OSX systems typically fail when useing the HDF5 printer. This presents as different error messages on different systems, and we do not have a solution. If encountering such issues, one can change the printer to `cout` or `ascii` in the YAML file.







