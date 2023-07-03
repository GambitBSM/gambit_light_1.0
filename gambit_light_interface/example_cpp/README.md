## Steps to connect your C++ code to GAMBIT-light

_See the example code in `example.cpp`._

1. Include the header for the GAMBIT-light interface:
   ```cpp
   #include "gambit_light_interface.h"
   ```

2. Add to your code a target/log-likelihood function with the following signature:
   ```cpp
   double user_loglike(const std::vector<std::string>& input_names, const std::vector<double>& input_vals, std::map<std::string,double>& output)
   ```
   _Arguments_:
   * `input_names`: A vector of strings with the names of the input parameters.
   * `input_vals`: A vector of doubles with the values of the input parameters.
   * `output`: A reference to a string-to-double map that the target function can fill with any additional output quantities.

   _Return value_: The target/log-likelihood value.


3. Add a small initialisation function that registers the target function (in this example `user_loglike`) with GAMBIT-light:
   ```cpp
   extern "C"
   void init_user_loglike(const char *fcn_name, gambit_light_register_loglike_fcn rf)
   {
     rf(fcn_name, (void*)user_loglike);
   }
   ```
   **Note:** The line `extern "C"` is needed to make sure that the library symbol name for this initialisation function will simply be the function name "init_user_loglike", instead of some decorated symbol name like "_Z17init_user_loglikePKcPFiS0_PvE".

4. Build your C++ code as a shared library. Make sure to include the `gambit_light_interface/include` directory containing `gambit_light_interface.h`. Example:
   ```
   g++ example.cpp -I /your/path/to/gambit_light_interface/include -shared -fPIC -o example.so
   ``` 

5. Add an entry for your target function in the `UserLogLikes` section of your GAMBIT configuration file. Example:
   ```yaml
   UserLogLikes:

     cpp_user_loglike:
       lang: c++
       user_lib: gambit_light_interface/example_cpp/example.so
       init_fun: init_user_loglike
       input:
         - param_name_1
         - param_name_2
         - param_name_3
       output:
        - cpp_user_loglike_output_1
        - cpp_user_loglike_output_2
        - cpp_user_loglike_output_3
   ```
   * Here `cpp_user_loglike` is simply a label you choose for your target function. It will be used in GAMBIT output files, log messages, etc. 
   * The `init_fun` setting must match the name of the initialisation function in your library (here `init_user_loglike`).
   * In the above example, the target function will receive three input parameters via the `input` map, and GAMBIT will attempt to extract three output quantities from the `output` map filled by the target function.
   * The input parameter names (`param_name_1`, etc.) correspond to parameters defined in the GAMBIT configuration file. These names will be used for the string keys in the `input` parameter map.
   * The output names (`c_user_loglike_output_1`, etc.) are the string keys that GAMBIT will look for to extract outputs from the `output` map.

   See `yaml_files/gambit_light_example.yaml` for a complete GAMBIT configuration file.
   
