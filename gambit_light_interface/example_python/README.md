## Steps to connect your Python code to GAMBIT-light

_See the example code in `example.py`._


1. Import the `gambit_light_interface` module from the shared library in `/your/path/to/gambit_light_interface/lib/`. 
   If you add this path to your `PYTHONPATH` environment variable, you can simply do:
   ```python
   import gambit_light_interface as gambit_light
   ```
   If you prefer to not edit your `PYTHONPATH`, you can append the path directly to `sys.path` at the top of your Python code:
   ```python
   import sys
   sys.path.append("/your/path/to/gambit_light_interface/lib/")
   import gambit_light_interface as gambit_light
   ```
   Alternatively, to avoid hardcoding an absolute path, you can construct the path relative to the path of your Python code:
   ```python
   import sys
   import os
   current_dir = os.path.dirname(os.path.abspath(__file__))
   sys.path.append(os.path.join(current_dir, "../lib"))
   import gambit_light_interface as gambit_light
   ```

2. Add to your code a target/log-likelihood function with the following signature:
   ```python
   user_loglike(input_names, input_vals, output)
   ```
   _Arguments_:
   * `input_names`: A list of strings with the names of the input parameters.
   * `input_vals`: A list of floats with the values of the input parameters.
   * `output`: A reference to a string-to-float dictionary that the target function can fill with any additional output quantities. 
   
      **Note:** Technically, `input_names` and `input_vals` will be instances of the classes `gambit_light_interface.str_vec` and `gambit_light_interface.dbl_vec`,respectively. These classes correspond to the pybind11 types `pybind11::bind_vector<std::vector<std::string>>` and `pybind11::bind_vector<std::vector<double>>`, which mimics the regular Python `list` type. Similarly, `output` will be instances of the class `gambit_light_interface.str_dbl_map`, which corresponds to `pybind11::bind_map<std::map<std::string,double>>` and mimics a regular Python `dict`.

   _Return value_: The target/log-likelihood value.


3. Add a small initialisation function that registers the target function (in this example `user_loglike`) with GAMBIT-light:
   ```python
   def init_user_loglike(fcn_name, rf_name):
       rf = getattr(gambit_light, rf_name)
       rf(fcn_name, "user_loglike")
   ```

4. Add an entry for your target function in the `UserLogLikes` section of your GAMBIT configuration file. Example:
   ```yaml
   UserLogLikes:

     py_user_loglike:
       lang: python
       user_lib: gambit_light_interface/example_python/example.py
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
   * Here `py_user_loglike` is simply a label you choose for your target function. It will be used in GAMBIT output files, log messages, etc. 
   * The `init_fun` setting must match the name of the initialisation function in your code (here `init_user_loglike`).
   * In the above example, the target function will receive three input parameters via the `input` dictionary, and GAMBIT will attempt to extract three output quantities from the `output` dictionary filled by the target function.
   * The input parameter names (`param_name_1`, etc.) correspond to parameters defined in the GAMBIT configuration file. These names will be used for the string keys in the `input` dictionary.
   * The output names (`c_user_loglike_output_1`, etc.) are the string keys that GAMBIT will look for to extract outputs from the `output` dictionary.

   See `yaml_files/gambit_light_example.yaml` for a complete GAMBIT configuration file.
   
