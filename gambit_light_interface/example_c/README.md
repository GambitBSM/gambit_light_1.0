## Steps to connect your C code to GAMBIT-light

_See the example code in `example.c`._

1. Include the header for the GAMBIT-light interface:
   ```c
   #include "gambit_light_interface.h"
   ```


2. Add to your code a target/log-likelihood function with the following signature:
   ```c
   double user_loglike(const int n_inputs, const double *input, const int n_outputs, double *output)
   ```
   _Arguments_:
   * `n_inputs`: The number of input parameters.
   * `input`: A pointer to an array with the input parameters.
   * `n_outputs`: The number of output quantities (besides the function return value).
   * `output`: A pointer to the output array that the function will fill.

   _Return value_: The target/log-likelihood value.

   You can give this function any name you prefer -- here we used `user_loglike` as an example.


3. Build your C code as a shared library. Make sure to include the `gambit_light_interface/include` directory containing `gambit_light_interface.h`. Example:
   ```console
   gcc example.c -I /your/path/to/gambit_light_interface/include -shared -fPIC -o example.so
   ```
   **Note:** On Mac, you may need to add the flag `-undefined dynamic_lookup` to the `gcc` command to make sure the library builds happily even if some symbols (correctly) are undefined.


4. Add an entry for your target function in the `UserLogLikes` section of your GAMBIT configuration file. Example:
   ```yaml
   UserLogLikes:

     c_user_loglike:
       lang: c
       user_lib: gambit_light_interface/example_c/example.so
       func_name: user_loglike
       input:
         - param_name_1
         - param_name_2
         - param_name_3
       output:
        - c_user_loglike_output_1
        - c_user_loglike_output_2
        - c_user_loglike_output_3
   ```
   * Here `c_user_loglike` is simply a label you choose for your target function. It will be used in GAMBIT output files, log messages, etc. 
   * The `func_name` setting must match the name of the target function in your library (here `user_loglike`).
   * In the above example, the target function expects three input parameters and computes three output quantities in addition to the return value. 
   * The input parameter names (`param_name_1`, etc.) correspond to parameters defined in the GAMBIT configuration file.
   * The output names (`c_user_loglike_output_1`, etc.) are the names that will be assigned to the output quantities in the GAMBIT output.

   See `yaml_files/gambit_light_example.yaml` for a complete GAMBIT configuration file.
   
