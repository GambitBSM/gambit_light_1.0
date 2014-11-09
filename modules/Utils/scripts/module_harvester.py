#!/bin/python
#
# GAMBIT: Global and Modular BSM Inference Tool
#*********************************************
# \file
#
#  Module and functor type harvesting script
#  Generates all_functor_types.hpp and 
#  module_rollcall.hpp.
#  
#  This script identified then reads through 
#  all the module rollcall and frontend headers, 
#  and harvests the types of every functor that 
#  GAMBIT will try to compile. These are needed
#  to generate the default (virtual) 
#  'print' functions in the base printer class
#  (see Printers/include/baseprinter.hpp)  
#
#*********************************************
#
#  Authors (add name and date if you modify):
#   
#  \author Ben Farmer 
#          (ben.farmer@gmail.com)
#    \date 2013 Sep, 2014 Jan 
#
#  \author Pat Scott 
#          (patscott@physics.mcgill.ca)
#    \date 2013 Oct, Nov
#    \date 2014 Jan, Nov
#
#*********************************************
from harvesting_tools import *

def main(argv):

    # Lists of backends, models and modules to exclude; anything starting with one of these strings is excluded.
    exclude_modules=set([])

    # Handle command line options
    verbose = False
    try:
        opts, args = getopt.getopt(argv,"vx:",["verbose","exclude-modules="])
    except getopt.GetoptError:
        print 'Usage: module_harvestor.py [flags]'
        print ' flags:'
        print '        -v                   : More verbose output'  
        print '        -x model1,model2,... : Exclude model1, model2, etc.' 
        sys.exit(2)
    for opt, arg in opts:
      if opt in ('-v','--verbose'):
        verbose = True
        print 'module_harvester.py: verbose=True'
      elif opt in ('-x','--exclude-modules'):
        exclude_modules.update(neatsplit(",",arg))
    exclude_header = exclude_modules
    fullheaders=[]
    fulltypeheaders=[]

    # List of headers to search
    headers      = set(["backend_rollcall.hpp"])
    type_headers = set(["types_rollcall.hpp"])

    # List of headers NOT to search (things we know are not module rollcall headers or module type headers, 
    # but are included in module_rollcall.hpp or types_rollcall.hpp)
    exclude_header.update(["shared_types.hpp", "backend_macros.hpp", "backend_undefs.hpp", "identification.hpp",
                           "backend_type_macros.hpp", "yaml.h"])

    # List of types NOT to return (things we know are not printable, but can appear in START_FUNCTION calls)
    exclude_type=set(["void"])

    # Get list of header files to search
    headers.update(retrieve_rollcall_headers(verbose,".",exclude_header))

    # Recurse through chosen headers, locating all the included headers therein, and find them all in the gambit source tree so that we can parse
    # them for types etc.   
    find_and_harvest_headers(headers,fullheaders,exclude_header,verbose=verbose)
    find_and_harvest_headers(type_headers,fulltypeheaders,exclude_header,verbose=verbose)
     
    # Search through rollcall headers and look for macro calls that create module_functors or safe pointers to them    
    types=set(["ModelParameters"]) #Manually add this one to avoid scanning through Models directory
    for header in fullheaders:
        with open(header) as f:
            if verbose: print "  Scanning header {0} for types used to instantiate functor class templates".format(header)
            module = ""
            for line in readlines_nocomments(f):
                # If this line defines the module name, update it.
                module = update_module(line,module)
                # Check for calls to functor creation macros, and harvest the types used.
                addiffunctormacro(line,module,types,fulltypeheaders,verbose=verbose)
    
    # Remove excluded types from the set
    types.difference_update(exclude_type)
    
    print "\nTypes harvested from headers:"
    for t in types:
        print ' ',t
    print 
    
    # Generate a c++ header containing the preprocessor sequence needed by Utils/include/printers.hpp, containing all the types we have harvested.
    towrite = "\
//   GAMBIT: Global and Modular BSM Inference Tool\n\
//   *********************************************\n\
///  \\file                                       \n\
///                                               \n\
///  Automatically generated preprocessor sequence\n\
///  of functor types.                            \n\
///                                               \n\
///  This file was automatically generated by     \n\
///  module_harvester.py. Do not modify.          \n\
///  The content is harvested from the rollcall   \n\
///  headers registered in module_rollcall.hpp    \n\
///  and the types registered in                  \n\
///  types_rollcall.hpp.                          \n\
///                                               \n\
///  *********************************************\n\
///                                               \n\
///  Authors (add name and date if you modify):   \n\
///                                               \n\
///  \\author The GAMBIT Collaboration            \n\
///  \date "+datetime.datetime.now().strftime("%I:%M%p on %B %d, %Y")+"\n\
///                                               \n\
///  *********************************************\n\
                                                  \n\
#ifndef __all_functor_types_hpp__                 \n\
#define __all_functor_types_hpp__                 \n\
                                                  \n\
#include \"types_rollcall.hpp\"                   \n\
                                                  \n\
// Automatically generated preprocessor sequence of types \n\
#define PRINTABLE_TYPES "
    for t in types:
        towrite+='({0})'.format(t)
    towrite+="\n\n#endif // defined __all_functor_types_hpp__\n"
    
    with open("./Utils/include/all_functor_types.hpp","w") as f:
        f.write(towrite)
    

    module_type_headers = set([])

    # Now generate module_type_rollcall.hpp
    #module_type_headers.update(retrieve_module_type_headers(verbose,".",exclude_modules))

    #print "\nModule type headers identified:"
    #for h in module_type_headers:
    #    print ' ',h


# Handle command line arguments (verbosity)
if __name__ == "__main__":
   main(sys.argv[1:])

