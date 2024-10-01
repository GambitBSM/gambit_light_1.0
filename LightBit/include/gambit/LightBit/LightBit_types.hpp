//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Type definition header for LightBit module.
///
///  *********************************************
///
///  Authors (add name if you modify):
///
///  \author Anders Kvellestad
///          (anders.kvellestad@fys.uio.no)
///  \date 2024 Jan
///
///  *********************************************


#ifndef __LightBit_types_hpp__
#define __LightBit_types_hpp__

#include <vector>
#include <map>
#include <string>

namespace Gambit
{

  namespace LightBit
  {

    // A simple class to communicate a parameter point inside LightBit. 
    // The class uses std::vector instead of std::map to preserve 
    // the order in which parameter values are added. 
    class parameter_point
    {
    private:

      std::vector<std::string> par_names;
      std::vector<double> par_vals;

    public:

      parameter_point() { }

      parameter_point(const std::vector<std::string>& par_names_in, const std::vector<double>& par_vals_in)
      { 
        set(par_names_in, par_vals_in);
      }

      void clear()
      {
        par_names.clear();
        par_vals.clear();
      }

      void append(std::string name_in, double val_in)
      {
        par_names.push_back(name_in);
        par_vals.push_back(val_in);
      }

      void set(const std::vector<std::string>& par_names_in, const std::vector<double>& par_vals_in)
      {
        if (par_names_in.size() != par_vals_in.size()) throw std::runtime_error("The number of parameter names and values for a parameter_point instance must match.");
        par_names = par_names_in;
        par_vals = par_vals_in;
      }

      const std::vector<std::string>& get_names() const
      {
        return par_names;
      }

      const std::vector<double>& get_vals() const
      {
        return par_vals;
      }

      std::map<std::string,double> get_map() const
      {
        std::map<std::string,double> result;
        for (size_t i = 0; i < size(); ++i)
        {
          result[par_names[i]] = par_vals[i];
        }
        return result;
      }

      size_t size() const
      {
        return par_names.size();
      }
    };

  }
}



#endif /* defined __LightBit_types_hpp__ */
