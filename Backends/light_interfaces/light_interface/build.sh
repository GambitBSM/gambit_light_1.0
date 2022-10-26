if true; then
    # stand-alone test program, without the need to run gambit
    g++ light_interface.cpp -ldl -DDEBUG_MAIN -I../../../contrib/yaml-cpp-0.6.2/include/ ./libyaml-cpp.a `python3-config --includes` `python3-config --ldflags` -lpython3.8 -fPIC -DDEBUG_MAIN -DHAVE_PYBIND11 -shared -o light_interface.so

    g++ test.cpp -o test -ldl ./light_interface.so
fi

if false; then
    # gambit-specific version
    g++ light_interface.cpp -ldl -I../../../contrib/yaml-cpp-0.6.2/include/ ./libyaml-cpp.a `python3-config --includes` `python3-config --ldflags` -lpython3.8 -fPIC -DHAVE_PYBIND11 -shared -o light_interface.so
    gfortran -c light_interface_mod.f90

    # compile fotran user-side library
    gfortran -shared -fPIC libuser.f90 -o libuser_fortran.so

    # compile C user-side library
    gcc libuser.c -shared -fPIC -o libuser_c.so

    # compile C++ user-side library
    g++ libuser.cpp -shared -fPIC -o libuser_cpp.so
fi
