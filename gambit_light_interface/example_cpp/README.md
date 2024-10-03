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

   You can give this function any name you prefer -- here we used `user_loglike` as an example.


3. Just below your target function, add the following macro call:
   ```cpp
   GAMBIT_LIGHT_REGISTER_LOGLIKE(user_loglike)
   ```
   **Note:** The purpose of this macro is to construct a C-style registration function that GAMBIT can connect to without worrying about the 
   name mangling for C++ library symbol names. GAMBIT will use this function to obtain a pointer to the actual target function `user_loglike`.


4. Build your C++ code as a shared library. Make sure to include the `gambit_light_interface/include` directory containing `gambit_light_interface.h`. Example:
   ```console
   g++ example.cpp -I /your/path/to/gambit_light_interface/include -shared -fPIC -o example.so
   ``` 
   **Note:** On Mac, you may need to add the flag `-undefined dynamic_lookup` to the `g++` command to make sure the library builds happily even if some symbols (correctly) are undefined.


5. Add an entry for your target function in the `UserLogLikes` section of your GAMBIT configuration file. Example:
   ```yaml
   UserLogLikes:

     cpp_user_loglike:
       lang: c++
       user_lib: gambit_light_interface/example_cpp/example.so
       func_name: user_loglike
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
   * The `func_name` setting must match the name of the target function in your library (here `user_loglike`).
   * In the above example, the target function will receive three input parameters via the `input_names` and `input_vals` vectors, and GAMBIT will attempt to extract three output quantities from the `output` map filled by the target function.
   * The input parameter names (`param_name_1`, etc.) correspond to parameters defined in the GAMBIT configuration file. These names will be used for the names in the `input_names` vector.
   * The output names (`c_user_loglike_output_1`, etc.) are the string keys that GAMBIT will look for to extract outputs from the `output` map.

   See `yaml_files/gambit_light_example.yaml` for a complete GAMBIT configuration file.
   
