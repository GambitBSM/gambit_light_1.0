gfortran -c light_interface_mod.f90
g++ light_interface.cpp -shared -fPIC -o light_interface.so

# stand-alone test program, without the need to run gambit
g++ light_interface.cpp -ldl -DDEBUG_MAIN

# compile fotran user-side library
gfortran -shared -fPIC libuser.f90 -o libuser.so

# compile C user-side library
# gcc libuser.c -shared -fPIC -o libuser.so
