#
#   ================================
#   |                              |
#   |   CBGB configuration module  |
#   |                              |
#   ================================
#


# Set path to source file:
src_file_path = 'example/minimal_test.f90'


# Choose 'fixed' or 'free' format:
format = 'free'  

# List the functions/subroutines to be loaded:
load_functions = ['Five', 'six', 'text', 'MA::text']

# List the common blocks to be loaded:
load_common_blocks = ['WIDTHA_HDEC', 'WIDTHHL_HDEC', 'WIDTHHH_HDEC', 
                      'WIDTHHC_HDEC', 'WISUSY_HDEC', 'WISFER_HDEC', 
                      'HD_golddec'] 


# Convert tabs to how many spaces?
tabs_to_n_spaces = 6

# Choose name mangling scheme: 'gfortran', 'ifort' or 'g77'
name_mangling = 'gfortran'

# Settings for constructing a GAMBIT capability corresponding to each function/subroutine:
f_capability_prefix = ''
f_capability_suffix = ''

# Include module name in capability?
module_name_in_capability = True

# Settings for constructing a GAMBIT capability corresponding to each common block:
cb_capability_prefix = 'cb_'
cb_capability_suffix = ''


