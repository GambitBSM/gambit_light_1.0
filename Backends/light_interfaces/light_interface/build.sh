gfortran -c light_interface_mod.f90

g++ light_interface.cpp -shared -fPIC -o light_interface.so

# stand-alone test program, without the need to run gambit
g++ light_interface.cpp -ldl -DDEBUG_MAIN -I../../../contrib/yaml-cpp-0.6.2/include/ ./libyaml-cpp.a

# compile fotran user-side library
gfortran -shared -fPIC libuser.f90 -o libuser_fortran.so

# compile C user-side library
gcc libuser.c -shared -fPIC -o libuser_c.so

# compile C++ user-side library
g++ libuser.cpp -shared -fPIC -o libuser_cpp.so
