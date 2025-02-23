#!/usr/bin/env python
#
# GAMBIT: Global and Modular BSM Inference Tool
#*********************************************
# \file
#
#  Particle harvesting script.
#  Generates particle_database.hpp
#
#  This script parses the particle_database.yaml 
#  file and adds the correct particle macros to 
#  the particle database .cpp file.
#
#*********************************************
#
#  Authors (add name and date if you modify):
#
#  \author Sanjay Bloor
#          (sanjay.bloor12@imperial.ac.uk)
#    \date 2017 Aug
#
#  \author Ben Farmer
#          (b.farmer@imperial.ac.uk)
#  \date 2018 Oct
#
#*********************************************
import os
import yaml
import itertools

toolsfile="./Utils/scripts/harvesting_tools.py"
exec(compile(open(toolsfile, "rb").read(), toolsfile, 'exec')) # Python 2/3 compatible version of 'execfile'

def create_entry(macro, particle_list):

  '''Create the correct entry to add to particle_database.cpp. The 
  argument "macro" should be one of the macros in particle_macros.hpp.'''
  
  # Output to add to the particle database. 
  output = "\n      "
  
  for i in range(0, len(particle_list)):
  
    entry = particle_list[i]
  
    PDG = entry['PDG_context']

    # Count how many particles appear in the PDG_context lists.
    numlists = sum(1 for x in PDG if isinstance(x, list))

    if numlists == 0:
      PDGbar = [-PDG[0], PDG[1]]
    else:
      PDGbar = [[-i[0], i[1]] for i in PDG]

    # Add description (C++ comment) if it exists
    if 'description' in entry:
      output += "\n      // " + str(entry['description']) + "\n      "

    # Get the spin x2 (return -1 if it is absent; i.e. for generic particles)
    spinx2 = -1
    if 'spinx2' in entry:
      spinx2 = entry['spinx2']

    # If the spin isn't an integer, throw an error
    if not isinstance(spinx2, int):
      print('Spin (x2) for particle with name {}').format(entry['name'])
      print('is not an integer. Please check particle_database.yaml.')
      sys.exit()

    # Get the color (return -1 if it is absent; i.e. for generic particles)
    color = -1
    if 'color' in entry:
      color = entry['color']

    # If we do not recognise the color assignment, throw an error
    if color not in [-1, 1, 3, 6, 8]:
      print(('Color for particle with name {}').format(entry['name']))
      print(('given as {}. GAMBIT only knows [1, 3, 6, 8].').format(color))
      print('Please check particle_database.yaml.')
      sys.exit()

    chargex3 = -1
    # Get the electric charge x3 (return -1 if it is absent, i.e. for generic particles)
    if 'chargex3' in entry:
      chargex3 = entry['chargex3']

    # If the electric charge (x3) isn't an integer, throw an error
    if not isinstance(spinx2, int):
      print('Charge (x3) for particle {}').format(entry['name'])
      print('is not an integer. Please check particle_database.yaml.')
      sys.exit()
      
    # Add the macro plus the particle name, plus the PDG-context pair. 
    output += macro + '("{0}", {1}, {2}, {3}, {4})\n      '.format(str(entry['name']), str(PDG).replace(']',')').replace('[','('), str(spinx2), str(chargex3), str(color))

    # If the YAML file says there is a conjugate particle, add the name of it and the negative PDG-context pair
    if 'conjugate' in entry: 
      output += macro + '("{0}", {1}, {2}, {3}, {4})\n      '.format(str(entry['conjugate']), str(PDGbar).replace(']',')').replace('[','('), str(spinx2), str(chargex3), str(color))
        
  return output

def main(argv):
    
  with open("./config/particle_database.yaml", "r") as f:

    try:
      data = yaml.safe_load(f)
    except yaml.YAMLerror as exc:
      print (exc)
      
    # These correspond to the five macros in particle_macros.hpp.
    SM_particles = data['StandardModel']['Particles']   # add_SM_particle
    SM_sets = data['StandardModel']['Sets']             # add_SM_partcle_set
    BSM_particles = data['OtherModels']['Particles']    # add_particle
    BSM_sets = data['OtherModels']['Sets']              # add_particle_set
    Generic = data['OtherModels']['Generic']            # add_generic_particle
    
  towrite = """//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \\file                                       
///                                               
///  Compile-time generation of particle          
///  database.                                    
///                                               
///  This file was automatically generated by     
///  particle_harvester.py. Do not modify.        
///                                               
///  Do not add to this if you want to add new    
///  particles -- just add the particles to the   
///  include/gambit/Models/particle_database.yaml 
///  file, and be confident that the particle     
///  harvester will add it to the database.       
///                                               
///  *********************************************
///                                               
///  Authors (add name and date if you modify):   
///                                               
///  \\author The GAMBIT Collaboration            
///  \\date """+datetime.datetime.now().strftime("%I:%M%p on %B %d, %Y")+"""
///                                               
///  *********************************************
                                                  
// Automatically-generated particle database.     
                                                  
                                                  
#include \"gambit/Models/partmap.hpp\"            
#include \"gambit/Models/particle_macros.hpp\"    
                                                  
namespace Gambit                                  
{                                                 
                                                  
  namespace Models                                
  {                                               
                                                  
    void define_particles(partmap* particles)     
    {                                             
"""
  towrite+= create_entry("add_SM_particle", SM_particles)  
    
  towrite+= create_entry("add_SM_particle_set", SM_sets)
  
  towrite+= create_entry("add_particle", BSM_particles)
  
  towrite+= create_entry("add_particle_set", BSM_sets)

  towrite+= create_entry("add_generic_particle", Generic)
  
  towrite+="                                      \n\
    }                                             \n\
                                                  \n\
  }                                               \n\
                                                  \n\
}                                                 \n"


  with open("./Models/src/particle_database.cpp","w") as f:
    f.write(towrite)

# Handle command line arguments (verbosity)
if __name__ == "__main__":
   main(sys.argv[1:])
