//  GAMBIT: Global and Modular BSM Inference Tool
//  *********************************************
///  \file
///
///  declaration for scanner module
///
///  *********************************************
///
///  Authors (add name and date if you modify):
//
///  \author Gregory Martinez
///          (gregory.david.martinez@gmail.com)
///  \date Feb 2014
///
///  *********************************************

#ifndef PLUGIN_PRIOR_HPP
#define PLUGIN_PRIOR_HPP

#include "gambit/ScannerBit/priors.hpp"
#include "gambit/ScannerBit/plugin_interface.hpp"

namespace Gambit
{
    namespace Priors
    {
        class Plugin : public BasePrior
        {
        private:
            typedef Scanner::Plugins::Plugin_Interface<double (const std::unordered_map<std::string, double> &), void (hyper_cube_ref<double>, std::unordered_map<std::string,double> &)> plugin_type;
            mutable plugin_type *plugin;
                
        public:
            Plugin(const std::vector<std::string>& params, const Options& options) : BasePrior(params)
            {
                std::string plugin_name;
                if (options.hasKey("plugin"))
                {
                    plugin_name = options.getValue<std::string>("plugin");
                }
                else
                {
                    scan_err << "Plugin prior:  need to specify plugin with \"plugin\" tag." << scan_end;
                    plugin_name = "";
                }
                plugin = new plugin_type("objective", plugin_name, param_names, sizeRef());
            }
            
            void transform(hyper_cube_ref<double> unitpars, std::unordered_map<std::string,double> &outputMap) const override
            {
                return (*plugin)(unitpars, outputMap);
            }

            void inverse_transform(const std::unordered_map<std::string, double> &, hyper_cube_ref<double>) const override
            {
                scan_err << "inverse transform not supported in plugin prior" << scan_end;
            }
            
            double log_prior_density(const std::unordered_map<std::string, double> &physical) const override
            {
                return (*plugin)(physical);
            }
            
            ~Plugin()
            {
                delete plugin;
            }
        };
        
        LOAD_PRIOR(plugin, Plugin)
    }
}

#endif
