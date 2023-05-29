## Steps to connect your Fortran code to GAMBIT-light

_See the example code in `example.f90`._

1. Prepare a module that will contain your target/log-likelihood function. Here's an example for a module called `user_mod`:

   ```.f90
   module user_mod
     use iso_c_binding
     use gambit_light_interface_mod
     implicit none
   
   contains

     ! Your target function will go here. 

     ! A small initialisation subroutine will go here.

   end module user_mod
   ```

2. Add to your module a target/log-likelihood function with the following signature:
   
   _Arguments_:
   * `n_inputs`: The number of input parameters.
   * `input`: A pointer to an array with the input parameters.
   * `n_outputs`: The number of output quantities (besides the function return value).
   * `output`: A pointer to the output array that the function will fill.

   _Return value_: The target/log-likelihood value.

   Here is a minimal example for a target function called `user_loglike`:

   ```.f90
     real(c_double) function user_loglike(n_inputs, input, n_outputs, output) bind(c)
       integer(c_int), value, intent(in) :: n_inputs, n_outputs
       type(c_ptr), intent(in), value :: input, output

       ! Create pointers to Fortran arrays
       real(c_double), dimension(:), pointer :: finput
       real(c_double), dimension(:), pointer :: foutput

       ! Connect the C arrays (input, output) to the Fortran arrays (finput, foutput)
       call c_f_pointer(input, finput, shape=[n_inputs])
       call c_f_pointer(output, foutput, shape=[n_outputs])
    
       ! Now perform your computations, fill the elements of the foutput array, 
       ! and assign the return value for the target function user_loglike.
       ! Here's a dummy example assuming n_inputs = 2 and n_outputs = 2:
       
       foutput(1) = 10 * finput(1)
       foutput(2) = 10 * finput(2)
       
       user_loglike = finput(1) * finput(1) + finput(2) * finput(2)
     end function user_loglike
   ```


3. Add a small initialisation subroutine that registers the target function (in this example `user_loglike`) with GAMBIT-light:
   ```.f90
     subroutine init_user_loglike(fcn_name, rf) bind(c)
       type(c_funptr), intent(in), value :: rf
       type(c_ptr), intent(in), value:: fcn_name
       procedure(gambit_light_register_fcn), pointer :: frf
       
       call c_f_procpointer(rf, frf)
       call frf(fcn_name, c_funloc(user_loglike))
     end subroutine init_user_loglike
   ```


4. Now build a shared library from your Fortran code and `gambit_light_interface_mod.f90`. Example:
   ```
   gfortran -c gambit_light_interface_mod.f90
   gfortran example.f90 -shared -fPIC -o example.so
   ```


5. Add an entry for your target function in the `UserLogLikes` section of your GAMBIT configuration file. Example:
   ```yaml
   UserLogLikes:

     fortran_user_loglike:
       lang: fortran
       user_lib: gambit_light_interface/example_fortran/example.so
       init_fun: init_user_loglike
       input:
         - param_name_1
         - param_name_2
       output:
        - fortran_user_loglike_output_1
        - fortran_user_loglike_output_2
   ```
   * Here `fortran_user_loglike` is simply a label you choose for your target function. It will be used in GAMBIT output files, log messages, etc.
   * The `init_fun` setting must match the name of the initialisation function in your library (here `init_user_loglike`).
   * In the above example, the target function expects two input parameters and computes two output quantities in addition to the return value.
   * The input parameter names (`param_name_1`, etc.) correspond to parameters defined in the GAMBIT configuration file.
   * The output names (`fortran_user_loglike_output_1`, etc.) are the names that will be assigned to the output quantities in the GAMBIT output.

   See `yaml_files/gambit_light_example.yaml` for a complete GAMBIT configuration file.
   
