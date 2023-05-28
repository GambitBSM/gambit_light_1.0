## Steps to connect your C code to GAMBIT-light

_See the example code in `example.c`._

1. Include the header for the GAMBIT-light interface:
   ```c
   #include "gambit_light_interface.h
   ```

2. Add to your code a target/log-likelihood function with the following signature:
   ```c
   double user_loglike(const int niparams, const double *iparams, const int noparams, double *oparams)
   ```
   _Arguments_:
   * `niparams`: The number of input parameters.
   * `iparams`: A pointer to an array with the input parameters.
   * `noparams`: The number of output quantities (besides the function return value).
   * `oparams`: A pointer to the output array that the function will fill.

   _Return value_: The target/log-likelihood value.


3. Add a small initialisation function that registeres the target function (in this example `user_loglike`) with GAMBIT-light:
   ```c
   void init_user_loglike(const char *fcn_name, gambit_light_register_loglike_fcn rf)
   {
     rf(fcn_name, user_loglike);
   }   
   ```

4. Build your C code as a shared library. Make sure to include the `gambit_light_interface/include` directory containing `gambit_light_interface.h`. Example:
   ```
   gcc example.c -I /your/path/to/gambit_light_interface/include -shared -fPIC -o example.so
   ``` 

5. Add an entry for your target function in the `UserLogLikes` section of your GAMBIT configuration file. Example:
   ```yaml
   UserLogLikes:

     c_user_loglike:
       lang: c
       user_lib: gambit_light_interface/example_c/example.so
       init_fun: init_user_loglike
       input:
         - param_name_1
         - param_name_2
         - param_name_3
       output:
        - c_user_loglike_output_1
        - c_user_loglike_output_2
        - c_user_loglike_output_3
   ```
   * Here `c_user_loglike` is simply the GAMBIT name we choose for our target function, for use in output files, log messages, etc. 
   * The `init_fun` setting must match the name of the initialisation function in our library (here `init_user_loglike`).
   * In the above example, the target function expects three input parameters and computes three output quantities in addition to the return value. 
   * The input parameter names (`param_name_1`, etc.) correspond to parameters defined in the GAMBIT configuration file.
   * The output names (`c_user_loglike_output_1`, etc.) are the names that will be assigned to the three output quantities in the GAMBIT output.

   See `yaml_files/gambit_light_example.yaml` for a complete GAMBIT configuration file.
   
