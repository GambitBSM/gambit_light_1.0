//  GAMBIT: Global and Modular BSM Inference Tool
//  *********************************************
///  \file
///
///  Loader singleton class for scanner plugins
///
///  *********************************************
///
///  Authors (add name and date if you modify):
//
///  \author Gregory Martinez
///          (gregory.david.martinez@gmail.com)
///  \date 2013 August
///  \date 2014 Feb
///
///  \author Pat Scott
///          (p.scott@imperial.ac.uk)
///  \date 2014 Dec
///  \date 2017 Jun
///
///  \author Ben Farmer
///          (benjamin.farmer@fysik.su.se)
///  \date 2016 Aug
///
///  *********************************************

#include <cstdlib>
#include <iomanip>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <stdio.h>

#include "gambit/ScannerBit/scanner_utils.hpp"
#include "gambit/ScannerBit/plugin_comparators.hpp"
#include "gambit/ScannerBit/plugin_loader.hpp"
#include "gambit/cmake/cmake_variables.hpp"
#include "gambit/Utils/table_formatter.hpp"
#include "gambit/Utils/screen_print_utils.hpp"
#include "gambit/Utils/mpiwrapper.hpp"
#include "gambit/Utils/util_functions.hpp"
#include "gambit/ScannerBit/priors_rollcall.hpp"

#include "gambit/Utils/begin_ignore_warnings_pybind11.hpp"
#include <pybind11/embed.h>
#include "gambit/Utils/end_ignore_warnings.hpp"

namespace py = pybind11;

namespace Gambit
{

    namespace Scanner
    {

        namespace Plugins
        {
            
            /*********************************************/
            /************** PYTHON STUFF *****************/
            /*********************************************/
            
        #ifdef HAVE_PYBIND11
            void pluginInfo::load_python_plugins()
            {
                plugins.Load_PyPlugins();
            }
            
            std::string PyPlugin_Details::print() const
            {
                std::stringstream out;
                out << "\n\x1b[01m\x1b[04mGENERAL PLUGIN INFO\x1b[0m" << std::endl;
                out << "\nname:     " << plugin_name;
                out << "\ntype:     python " << type;
                out << "\nversion:  " << version;
                out << "\npackage:  " << package;
                out << "\nstatus:   ";
                if (status == "ok")
                    out << "\x1b[32;01m" << status << "\x1b[0m";
                else
                    out << "\x1b[31;01m" << status << "\x1b[0m";

                out << "\nlocation: " << loc << std::endl;
                if (error != "")
                    out << "\x1b[31;01m" << "error output: " << "\x1b[0m" << error << std::endl;

                out << std::endl;
                
                out << "\n\x1b[01m\x1b[04mDESCRIPTION\x1b[0m\n" << std::endl;
                out << class_doc << std::endl << std::endl;
                
                if (init_doc != "")
                {
                    out << "\n\x1b[01m\x1b[04mINIT DESCRIPTION\x1b[0m\n" << std::endl;
                    out << init_doc << std::endl << std::endl;
                }
                
                if (run_doc != "")
                {
                    out << "\n\x1b[01m\x1b[04mRUN DESCRIPTION\x1b[0m\n" << std::endl;
                    out << run_doc << std::endl << std::endl;
                }
                
                return out.str();
            }
            
            void Plugin_Loader::Load_PyPlugins()
            {
                if (python_plugin_map.size() == 0)
                {
                    std::vector<std::string> types = {"objective", "scanner"};
                    for (auto &&type : types)
                        Load_PyPlugins(type);
                }
            }
            
            void Plugin_Loader::Load_PyPlugins(const std::string &type)
            {
                py::scoped_interpreter *guard;
                try
                {
                    guard = new py::scoped_interpreter();
                }
                catch(std::exception &)
                {
                    guard = nullptr;
                }
                {
                    std::string path = std::string(GAMBIT_DIR "/ScannerBit/src/") + type + "s/python/plugins";
                    auto sys_list = py::list(py::module::import("sys").attr("path"));
                    sys_list.append(py::cast(path));
                    
                    if (FILE* p_f = popen((std::string("ls ") + path).c_str(), "r"))
                    {
                        char path_buffer[1024];
                        int p_n;
                        std::string fname;
                        std::stringstream ss;
                        
                        while ((p_n = fread(path_buffer, 1, sizeof path_buffer, p_f)) > 0)
                        {
                            ss << std::string(path_buffer, p_n);
                        }
                        while (ss >> fname)
                        {
                            std::string loc = path + "/" + fname;
                            if (fname.substr(fname.size() - 3) == ".py")
                                fname = fname.substr(0, fname.size() - 3);
                            else if (fname.substr(fname.size() - 4) == ".pyc")
                                fname = fname.substr(0, fname.size() - 4);
                            try
                            {
                                auto file = py::module::import(fname.c_str());
                                if (py::hasattr(file, "__plugins__") && py::isinstance<py::dict>(file.attr("__plugins__")))
                                {
                                    py::dict plugins = file.attr("__plugins__");
                                    for (auto &&plug : plugins)
                                    {
                                        
                                        std::string plug_name = plug.first.cast<std::string>();
                                        py::handle plug_class = plug.second;
                                        if (python_plugin_map[type].find(plug_name) == python_plugin_map[type].end())
                                        {
                                            PyPlugin_Details detail;
                                            detail.plugin_name = plug_name;
                                            detail.package = fname;
                                            detail.type = type;
                                            detail.loc = path;
                                            
                                            try
                                            {
                                                if (py::hasattr(plug_class, "run") && py::isinstance<py::function>(plug_class.attr("run")))
                                                {
                                                    auto run = plug_class.attr("run");
                                                    if (py::hasattr(run, "__doc__") && py::isinstance<py::str>(run.attr("__doc__")))
                                                    {
                                                        detail.run_doc = run.attr("__doc__").cast<std::string>();
                                                    }
                                                }
                                                else
                                                {
                                                    detail.status = "\"run\" not defined";
                                                }
                                                
                                                if (py::hasattr(plug_class, "__init__") && py::isinstance<py::function>(plug_class.attr("__init__")))
                                                {
                                                    auto init = plug_class.attr("__init__");
                                                    if (py::hasattr(init, "__doc__")&& py::isinstance<py::str>(init.attr("__doc__")))
                                                    {
                                                        detail.init_doc = init.attr("__doc__").cast<std::string>();
                                                    }
                                                }
                                                else
                                                {
                                                    detail.status = "\"__init__\" not defined";
                                                }
                                                
                                                if (py::hasattr(file, "__error__"))
                                                {
                                                    if (py::isinstance<py::str>(file.attr("__error__")))
                                                        detail.status = file.attr("__error__").cast<std::string>();
                                                    else
                                                        detail.status = "error thrown";
                                                }
                                                
                                                if (py::hasattr(plug_class, "__version__") && py::isinstance<py::str>(plug_class.attr("__version__")))
                                                {
                                                    detail.version = plug_class.attr("__version__").cast<std::string>();
                                                }
                                                
                                                if (py::hasattr(plug_class, "__doc__")&& py::isinstance<py::str>(plug_class.attr("__doc__")))
                                                {
                                                    detail.class_doc = plug_class.attr("__doc__").cast<std::string>();
                                                }
                                            }
                                            catch(std::exception &ex)
                                            {
                                                detail.status = "plugin not loaded";
                                                detail.error = ex.what();
                                            }
                                            
                                            python_plugin_map[type][plug_name] = detail;
                                            //detail.debug();
                                        }
                                        else
                                        {
                                            scan_warn << "PYthon plugin \"" << plug_name << "\" of type \"" << type << "\" has multiple definitions.  Ignoring all but one." << scan_end;
                                        }
                                    }
                                }
                            }
                            catch (std::exception &ex)
                            {
                                if (fname != "utils")
                                    scan_warn << "\"" << fname << "\" was not loaded: " << ex.what() << scan_end;
                            }
                            
                        }
                    }
                    
                    sys_list.attr("pop")();
                    }
                if (guard != nullptr)
                {
                    delete guard;
                }
            }

            PyPlugin_Details &Plugin_Loader::find_python_plugin(const std::string &type, const std::string &plugin)
            {
                std::map<std::string, std::map<std::string, PyPlugin_Details>>::iterator find;
                std::map<std::string, PyPlugin_Details>::iterator find2;
                
                if (((find = python_plugin_map.find(type)) != python_plugin_map.end()) &&
                    ((find2 = find->second.find(plugin)) != find->second.end()))
                {
                    return find2->second;
                }
                else
                {
                    scan_err << "Plugin \"" << plugin << "\" of type \"" << type << "\" is not found." << scan_end;
                    return find2->second;
                }
            }
            
            PyPlugin_Details &pluginInfo::load_python_plugin(const std::string &type, const std::string &plugin)
            {
                return plugins.find_python_plugin(type, plugin);
            }
            
            inline std::string print_plugins(std::map<std::string, std::map<std::string, std::vector<Plugin_Details>>>::const_iterator plugins, 
                                             const std::map< std::string, std::map<std::string, PyPlugin_Details>> &pyplugins)
            {
                table_formatter table(plugins->first + " PLUGINS", "VERSION", "STATUS");
                table.capitalize_title();
                table.padding(1);
                
                std::map<std::string, std::vector<std::pair<std::string, std::string>>> temp;

                for (auto it = plugins->second.begin(); it != plugins->second.end(); ++it)
                {
                    for (auto jt = it->second.begin(); jt != it->second.end(); ++jt)
                    {
                        temp[it->first].emplace_back(jt->version, jt->status);
                    }
                }
                
                if (pyplugins.find(plugins->first) != pyplugins.end())
                    for (auto &&elem : pyplugins.at(plugins->first))
                    {
                        temp[elem.first].emplace_back(elem.second.version, elem.second.status);
                    }
                
                for (auto&& elem : temp)
                {
                    for (auto it = elem.second.begin(); it != elem.second.end(); ++it)
                    {
                        if (it == elem.second.begin())
                            table << elem.first;
                        else
                            table << "";
                        table << it->first;
                        if (it->second == "ok")
                            table.green() << it->second;
                        else
                            table.red() << it->second;
                    }
                }

                return table.str();
            }
        #endif

            /*************************************************/
            /************** END PYTHON STUFF *****************/
            /*************************************************/

            inline std::string print_plugins(std::map< std::string, std::map<std::string, std::vector<Plugin_Details> > >::const_iterator plugins)
            {
                table_formatter table(plugins->first + " PLUGINS", "VERSION", "STATUS");
                table.capitalize_title();
                table.padding(1);
                
                std::map<std::string, std::vector<std::pair<std::string, std::string>>> temp;

                for (auto it = plugins->second.begin(); it != plugins->second.end(); ++it)
                {
                    for (auto jt = it->second.begin(); jt != it->second.end(); ++jt)
                    {
                        const str firstentry = (jt == it->second.begin() ? it->first : "");
                        table << firstentry;
                        table << jt->version;
                        if (jt->status == "ok")
                            table.green() << jt->status;
                        else
                            table.red() << jt->status;
                    }
                }

                return table.str();
            }

            Plugin_Loader::Plugin_Loader() : path(GAMBIT_DIR "/ScannerBit/lib/")//, python_plugin_map(pyplugin_info())
            {
                std::string p_str;
                std::ifstream lib_list(path + "plugin_libraries.list");
                if (lib_list.is_open())
                {
                    while (lib_list >> p_str)
                    {
                        //if (p_str.find(".so") != std::string::npos && p_str.find(".so.") == std::string::npos)
                        p_str = path + p_str;
                        if(access(p_str.c_str(), F_OK) != -1) //can use R_OK|W_OK|X_OK also
                            loadLibrary (p_str);
                        else
                            scan_warn << "Could not find plugin library \"" << p_str << "\"." << scan_end;
                    }

                    auto excluded_plugins = loadExcluded(Utils::buildtime_scratch+"scanbit_excluded_libs.yaml");
                    const str linked_libs(Utils::buildtime_scratch+"scanbit_linked_libs.yaml");
                    const str reqd_entries(Utils::buildtime_scratch+"scanbit_reqd_entries.yaml");
                    const str flags(Utils::buildtime_scratch+"scanbit_flags.yaml");
                    process(linked_libs, reqd_entries, flags, excluded_plugins);
                }
                else
                {
                    scan_err << "Cannot open ./ScannerBit/lib/plugin_libraries.list" << scan_end;
                }

            }

            /// Check a plugin map and return a flag indicating if a candidate plugin is already in the map or not.
            bool is_new_plugin(std::map<str, std::map<str, std::vector<Plugin_Details>>>& pmap, Plugin_Details& cand)
            {
                bool new_plugin_type = pmap.find(cand.type) == pmap.end();
                bool new_plugin_name = new_plugin_type || pmap.at(cand.type).find(cand.plugin) == pmap.at(cand.type).end();
                bool new_plugin_version = true;
                if (not new_plugin_name)
                {
                    for (auto x : pmap.at(cand.type).at(cand.plugin))
                        if (x.version == cand.version) new_plugin_version = false;
                }
                return new_plugin_version;
            }

            void Plugin_Loader::process(const std::string& libFile, const std::string& plugFile, const std::string& flagFile, std::vector<Plugin_Details>& excluded_plugins)
            {
                YAML::Node libNode = YAML::LoadFile(libFile);
                YAML::Node plugNode = YAML::LoadFile(plugFile);
                YAML::Node flagNode = YAML::LoadFile(flagFile);

                for (auto it = excluded_plugins.begin(), end = excluded_plugins.end(); it != end; it++)
                {
                    if (is_new_plugin(total_plugin_map, *it))
                    {
                        it->get_status(libNode, plugNode, flagNode);
                        excluded_plugin_map[it->type][it->plugin].push_back(*it);
                        total_plugin_map[it->type][it->plugin].push_back(*it);
                    }
                }

                for (auto it = plugins.begin(), end = plugins.end(); it != end; it++)
                {
                    if (is_new_plugin(total_plugin_map, *it))
                    {
                        it->get_status(libNode, plugNode, flagNode);
                        plugin_map[it->type][it->plugin].push_back(*it);
                        total_plugin_map[it->type][it->plugin].push_back(*it);
                    }
                }
            }

            std::vector<Plugin_Details> Plugin_Loader::loadExcluded (const std::string& file)
            {
                std::vector<Plugin_Details> excluded_plugins;
                YAML::Node node = YAML::LoadFile(file);

                if (node.IsMap())
                {
                    for (auto it = node.begin(), end = node.end(); it != end; it++)
                    {
                        std::string lib = it->first.as<std::string>();
                        std::vector<Plugin_Details> excluded_plugins_temp;
                        std::vector<std::string> reason;
                        if (it->second.IsMap())
                        {
                            if (it->second["plugins"])
                            {
                                for (auto it2 = it->second["plugins"].begin(), end2 = it->second["plugins"].end(); it2 != end2; ++it2)
                                {
                                    Plugin_Details temp(it2->as<std::string>());

                                    temp.path = path + lib;
                                    temp.status = "excluded";
                                    excluded_plugins_temp.push_back(temp);
                                }
                            }

                            if (it->second["reason"])
                            {
                                for (auto rea : it->second["reason"])
                                {
                                    if(rea.IsScalar())
                                    {
                                        reason.push_back(rea.as<std::string>());
                                    }
                                    if(rea.IsMap())
                                    {
                                        for (auto it2 = rea.begin(), end2 = rea.end(); it2 != end2; ++it2)
                                        {
                                            reason.push_back(it2->first.as<std::string>() + ":  " + it2->second.as<std::string>());
                                        }
                                    }
                                }
                            }

                            for (auto it2 = excluded_plugins_temp.begin(), end2 = excluded_plugins_temp.end(); it2 != end2; ++it2)
                            {
                                it2->status = "excluded";
                                it2->reason.insert(it2->reason.end(), reason.begin(), reason.end());
                            }

                            excluded_plugins.insert(excluded_plugins.end(), excluded_plugins_temp.begin(), excluded_plugins_temp.end());
                            total_plugins.insert(total_plugins.end(), excluded_plugins_temp.begin(), excluded_plugins_temp.end());

                        }
                    }
                }
                return excluded_plugins;
            }

            void Plugin_Loader::loadLibrary (const std::string &p_str, const std::string &plug)
            {
                std::string str;
                if (FILE* f = popen((std::string("nm ") + p_str + std::string(" | grep \"__gambit_plugin_pluginInit_\"")).c_str(), "r"))
                {
                    char buffer[1024];
                    int n;
                    std::stringstream ss;

                    while ((n = fread(buffer, 1, sizeof buffer, f)) > 0)
                    {
                        ss << std::string(buffer, n);
                    }

                    while(std::getline(ss, str))
                    {
                        std::string::size_type pos = str.find("__gambit_plugin_pluginInit_");

                        if (pos != std::string::npos &&
                                (str.rfind(" T ", pos) != std::string::npos || str.rfind(" t ", pos) != std::string::npos))
                        {
                            Plugin_Details temp(str.substr(pos + 27, str.rfind("__") - pos - 27));

                            if (plug == "" || temp.plugin == plug)
                            {
                                temp.path = p_str;
                                plugins.push_back(temp);
                                total_plugins.push_back(temp);
                            }
                        }
                    }

                    pclose(f);
                }
            }

            std::vector<std::string> Plugin_Loader::print_plugin_names(const std::string &plug_type) const
            {
                std::vector<std::string> vec;
            #ifdef HAVE_PYBIND11
                plugin_info.load_python_plugins();
            #endif
                
                if (plug_type != "")
                {
                    auto plugins = total_plugin_map.find(plug_type);
                    if (plugins == total_plugin_map.end())
                    {
                        return vec;
                    }
                    else
                    {
                        for (auto it = plugins->second.begin(), end = plugins->second.end(); it != end; ++it)
                        {
                            vec.push_back(it->first);
                        }
                        
                    #ifdef HAVE_PYBIND11
                        if (python_plugin_map.find(plug_type) != python_plugin_map.end())
                            for (auto &&elem : python_plugin_map.at(plug_type))
                                vec.push_back(elem.first);
                    #endif
                    }
                }
                else
                {
                    for (auto it = total_plugin_map.begin(), end = total_plugin_map.end(); it != end; it++)
                    {
                        for (auto it2 = it->second.begin(), end2 = it->second.end(); it2 != end2; ++it2)
                        {
                            vec.push_back(it2->first);
                        }
                        
                    #ifdef HAVE_PYBIND11
                        if (python_plugin_map.find(it->first) != python_plugin_map.end())
                            for (auto &&elem : python_plugin_map.at(it->first))
                                vec.push_back(elem.first);
                    #endif
                    }
                }

                return vec;
            }

            std::vector<std::string> Plugin_Loader::list_prior_groups() const
            {
                YAML::Node node = YAML::LoadFile(GAMBIT_DIR "/config/priors.dat");
                std::vector<std::string> vec;

                for(auto &&n : node)
                {
                    if (n.second.IsSequence())
                        vec.push_back(n.first.as<std::string>());
                }

                vec.push_back("priors");

                return vec;
            }

            std::string Plugin_Loader::print_priors(const std::string &prior_group) const
            {
                YAML::Node node = YAML::LoadFile(GAMBIT_DIR "/config/priors.dat");
                std::stringstream out;

                if (prior_group == "priors")
                {
                    table_formatter table("Prior Name", "Prior Group");
                    //table.top_line(true);
                    table.bottom_line(true);
                    std::unordered_set<std::string> prior_set;
                    for(auto &&n : node)
                    {
                        if (n.second.IsSequence())
                        {
                            for(auto &&v : n.second.as<std::vector<std::string>>())
                            {
                                prior_set.insert(v);
                                if (Gambit::Priors::prior_creators.find(v) != Gambit::Priors::prior_creators.end())
                                table.no_newline() << v << n.first.as<std::string>();
                            }
                        }
                    }

                    for (auto &&v : Gambit::Priors::prior_creators)
                    {
                        if (prior_set.find(v.first) == prior_set.end())
                        {
                            table.no_newline() << v.first << "none";
                        }
                    }

                    table.no_newline() << "" << "";
                    out << "\x1b[01m\x1b[04mPRIOR LIST\x1b[0m\n" << std::endl;
                    out << format_for_screen("For information on a specific prior, see its prior group's diagnostic via \"./gambit group_name\".");
                    out << table.str() << std::endl;
                    out << "\x1b[01m\x1b[04mDESCRIPTION\x1b[0m\n" << std::endl;
                    if (node["priors"])
                        out << format_for_screen(node["priors"].as<std::string>()) << std::endl;
                }
                else
                {
                    std::vector<std::string> prior_names;
                    std::string description;
                    for(auto &&n : node)
                    {
                        if (n.first.as<std::string>() == prior_group)
                        {
                            if (n.second.IsSequence())
                            {
                                prior_names = n.second.as<decltype(prior_names)>();
                            }
                            else if (n.second.IsScalar())
                            {
                                description = n.second.as<std::string>();
                            }
                        }
                    }

                    if (prior_names.size() >0 || description != "")
                    {
                        out << "\x1b[01m\x1b[04mPRIORS INFO\x1b[0m\n" << std::endl;
                        out << "\x1b[01mprior group name:  \x1b[0m" << prior_group << std::endl;
                        out << "\x1b[01mpriors:  \x1b[0m" << prior_names << "\n" << std::endl;
                        out << "\x1b[01m\x1b[04mDESCRIPTION\x1b[0m\n\n" << description << std::endl;
                    }
                }

                return out.str();
            }

            std::string Plugin_Loader::print_all(const std::string &plug_type) const
            {
            #ifdef HAVE_PYBIND11
                plugin_info.load_python_plugins();
            #endif
                
                if (plug_type != "")
                {
                    auto plugins = total_plugin_map.find(plug_type);
                    if (plugins == total_plugin_map.end())
                    {
                        return "";
                    }
                    else
                    {
                    #ifdef HAVE_PYBIND11
                        return print_plugins(plugins, python_plugin_map);
                    #else
                        return print_plugins(plugins);
                    #endif
                    }
                }
                else
                {
                    std::string ret = "";
                    for (auto it = total_plugin_map.begin(), end = total_plugin_map.end(); it != end; it++)
                    {
                    #ifdef HAVE_PYBIND11
                        ret += print_plugins(it, python_plugin_map);
                    #else
                        ret += print_plugins(it);
                    #endif
                    }
                    
                    return ret;
                }

                return "";
            }

            int Plugin_Loader::print_all_to_screen (const std::string &name) const
            {
                std::string output = print_all(name);
                if (output == "")
                    return 1;

                print_to_screen(output, name);
                return 0;
            }

            std::string Plugin_Loader::print_plugin(const std::string &name) const
            {
                std::unordered_map<std::string, std::vector<const Scanner::Plugins::Plugin_Details*>> vec;
                std::stringstream output;
                
                for (auto it_map = getPluginsMap().begin(), end = getPluginsMap().end(); it_map!= end; it_map++)
                {
                    auto it = it_map->second.find(name);
                    if (it != it_map->second.end())
                    {
                        for (auto &&plug : it->second)
                        {
                            vec[it_map->first].push_back(&plug);
                        }
                    }
                }

                if (vec.size() == 0)
                {
                    return ""; //NOTE: Need to add python scanners here.
                }
                else
                {
                    for (auto it = vec.begin(), end = vec.end(); it != end; it++)
                    {
                        output << Plugin_Details::printMultiPlugins(it->second) << std::endl;
                    }
                }

                return output.str();
            }

            std::string Plugin_Loader::print_plugin(const std::string &type, const std::string &plugin) const
            {
                std::vector<const Scanner::Plugins::Plugin_Details *> vec;
                if((getPluginsMap().find(type) == getPluginsMap().end()) || (getPluginsMap().at(type).find(plugin) == getPluginsMap().at(type).end()))
                {
                #ifdef HAVE_PYBIND11
                    plugin_info.load_python_plugins();
                    std::map<std::string, std::map<std::string, PyPlugin_Details>>::const_iterator find;
                    std::map<std::string, PyPlugin_Details>::const_iterator find2;
                    
                    if (((find = python_plugin_map.find(type)) != python_plugin_map.end()) &&
                        ((find2 = find->second.find(plugin)) != find->second.end()))
                    {
                        return find2->second.print();
                    }
                    else
                    {
                        return "";
                    }
                #else
                    return "";
                #endif
                }

                for (auto it = getPluginsMap().at(type).at(plugin).begin(), end = getPluginsMap().at(type).at(plugin).end(); it != end; it++)
                {
                    vec.push_back(&(*it));
                }

                return Plugin_Details::printMultiPlugins(vec) + "\n";
            }

            int Plugin_Loader::print_plugin_to_screen (const std::string &name) const
            {
                std::string output = print_plugin(name);
                if (output == "")
                    return 1;

                print_to_screen(output, name);
                return 0;
            }

            int Plugin_Loader::print_plugin_to_screen (const std::string &type, const std::string &name) const
            {
                std::string output = print_plugin(type, name);
                if (output == "")
                    return 1;

                print_to_screen(output, name);
                return 0;
            }

            int Plugin_Loader::print_plugin_to_screen (const std::vector<std::string> &names) const
            {
                std::string output;
                for (auto it = names.begin(), end = names.end(); it != end; ++it)
                {
                    output += print_plugin(*it) + "\n";
                }

                if (output == "")
                    return 1;

                print_to_screen(output, names[0]);
                return 0;
            }

            Plugin_Details &Plugin_Loader::find (const std::string &type, std::string plugin, const std::string &version, const std::string &lib) const
            {
                std::vector<Plugin_Details_Ref> plugins;

                if((plugin_map.find(type) == plugin_map.end()) || (plugin_map.at(type).find(plugin) == plugin_map.at(type).end()))
                {
                #ifdef HAVE_PYBIND11
                    plugin = "python";
                #else
                    scan_err << "There is no plugin named \"" << plugin <<"\" of type \"" << type << "\"" << scan_end;
                #endif
                }

                for (auto it = plugin_map.at(type).at(plugin).begin(), end = plugin_map.at(type).at(plugin).end(); it != end; it++)
                {
                    if (VersionCompare(version)(*it) && (lib == "" || lib == it->path) and it->status == "ok")
                        plugins.push_back(*it);
                }

                if (plugins.size() > 1)
                {
                    std::sort(plugins.begin(), plugins.end(), Plugin_Version_Supersedes);
                    auto it2 = plugins.begin();
                    std::vector<Plugin_Details_Ref> matches;
                    for (auto it = it2 + 1, end = plugins.end(); it != end; it++)
                    {
                        if (*it == *it2) matches.push_back(*it);
                    }
                    if (not matches.empty())
                    {
                      scan_err << "More than one plugin met the input criteria in the YAML file:\n\n" << static_cast<Plugin_Details &>(*it2).print() << "\n";
                      for (auto it = matches.begin(); it != matches.end(); it++)
                      {
                        scan_err << static_cast<Plugin_Details &>(*it).print() << "\n";
                      }
                      scan_err << "To indicate to ScannerBit which of these plugins should be used, please"
                               << "\nadd more specific information in the Scanners section of your YAML file." << scan_end;
                    }
                }
                else if (plugins.size() == 0)
                {
                    scan_err << "Plugin \"" << plugin << "\" of type \"" << type << "\" and "
                            << " version \"" << version << "\" is not found." << scan_end;
                    plugins.resize(1);
                }

                return plugins[0];
            }

            void pluginInfo::iniFile(const Options &options_in)
            {
                options = options_in;

                if (options.getNode().IsMap())
                {
                    if (options.hasKey("default_output_path"))
                        def_out_path = options.getValue<std::string>("default_output_path");
                    else
                        scan_err << "\"default output path\" must be specified in KeyValues section." << scan_end;

                    for (auto it = options.getNode().begin(), end = options.getNode().end(); it != end; it++)
                    {
                        std::string plug_type = it->first.as<std::string>();

                        if (it->second.IsMap() && plug_type[plug_type.length()-1] == 's' && plug_type != "priors" && plug_type != "parameters")
                        {
                            for (auto it_p = it->second.begin(), end = it->second.end(); it_p != end; it_p++)
                            {
                                std::string plug_tag = it_p->first.as<std::string>();

                                Proto_Plugin_Details temp;

                                if (it_p->second["plugin"])
                                {
                                    temp.plugin = it_p->second["plugin"].as<std::string>();
                                }
                                else
                                {
                                    scan_warn << "Plugin name is not defined under the \"" << it->first << "\" tag.  "
                                                << "using the tag \"" << it_p->first.as<std::string>()
                                                << "\" as the plugin name." << scan_end;
                                    temp.plugin = it_p->first.as<std::string>();
                                }

                                if (it_p->second["version"])
                                    temp.version = it_p->second["version"].as<std::string>();

                                if (it_p->second["plugin_path"])
                                {
                                    temp.path = it_p->second["plugin_path"].as<std::string>();
                                    plugins.loadLibrary(temp.path, temp.plugin);
                                }

                                selectedPlugins[plug_type.substr(0, plug_type.length()-1)][plug_tag] = temp;
                            }
                        }
                    }
                }
                else
                {
                    scan_err << "Plugins subsection is not of the \"Map\" YAML format." << scan_end;
                }
            }

            void pluginInfo::printer_prior(printer_interface &printerIn, Priors::BasePrior &prior_in)
            {
                printer = &printerIn;
                prior = &prior_in;
            }

            Plugins::Plugin_Interface_Details pluginInfo::operator()(const std::string &type, const std::string &tag)
            {
                if (selectedPlugins.find(type) != selectedPlugins.end() && selectedPlugins[type].find(tag) != selectedPlugins[type].end())
                {
                    Proto_Plugin_Details &detail = selectedPlugins[type][tag];
                    YAML::Node plugin_options = options.getNode(type + "s", tag);

                    plugin_options["default_output_path"] = options.getValue<std::string>("default_output_path");
                    plugin_options["print_timing_data"] = options.getValue<std::string>("print_timing_data");

                    if (!plugin_options["likelihood: model_invalid_for_lnlike_below"])
                        plugin_options["likelihood: model_invalid_for_lnlike_below"] = options.getValue<double>("model_invalid_for_lnlike_below");

                    if (!plugin_options["likelihood: lnlike_offset"] and options.hasKey("lnlike_offset"))
                        plugin_options["likelihood: lnlike_offset"] = options.getValue<double>("lnlike_offset");

                    return Plugin_Interface_Details(plugins.find(type, detail.plugin, detail.version, detail.path), printer, prior, plugin_options);
                }
                else
                {
                    scan_err << "Plugin \"" << tag << "\" of type \"" << type << "\" is not defined under the \"Scanner\""
                            << " subsection in the inifile" << scan_end;

                    return Plugin_Interface_Details(plugins.find(type, "", "", ""), printer, prior, options.getOptions(type + "s", tag).getNode());
                }
            }

            void pluginInfo::dump()
            {
                for (auto it = resume_data.begin(), end = resume_data.end(); it != end; ++it)
                {
                    std::string path = Gambit::Utils::ensure_path_exists(def_out_path + "/temp_files/" + it->first);
                    std::ofstream out((path).c_str(), std::ofstream::binary);
                    for (auto v_it = it->second.begin(), v_end = it->second.end(); v_it != v_end; ++v_it)
                    {
                        (*v_it)->print(out);
                    }
                }

                printer->finalise(true); //"true" flag for "abnormal" stop; i.e. run is not completely finished
                // Debugging output
                // #ifdef WITH_MPI
                //   std::cout << "rank " << getRank() <<": ";
                // #endif
                // std::cout << "Gambit has written resume data to disk, preparing to stop!" << std::endl;
            }

            void pluginInfo::dump(const std::string &name)
            {
                auto it = resume_data.find(name);
                if (it != resume_data.end())
                {
                    std::string path = Gambit::Utils::ensure_path_exists(def_out_path + "/temp_files/" + name);
                    std::ofstream out((path).c_str(), std::ofstream::binary);
                    for (auto v_it = it->second.begin(), v_end = it->second.end(); v_it != v_end; ++v_it)
                    {
                        (*v_it)->print(out);
                    }
                }
            }

            /// Save persistence file to record that the alternative min_LogL value is in use for this scan
            void pluginInfo::save_alt_min_LogL_state() const
            {
                std::string state_fname(def_out_path+"/ALT_MIN_LOGL_IN_USE");
                std::ofstream outfile(state_fname);
                outfile.close();
            }

            /// Delete the persistence file if it exists (e.g. when starting a new run)
            void pluginInfo::clear_alt_min_LogL_state() const
            {
                if(check_alt_min_LogL_state())
                {
                    std::string state_fname(def_out_path+"/ALT_MIN_LOGL_IN_USE");
                    // Persistence file exists: delete it
                    if (remove(state_fname.c_str())) scan_err << "Failed to delete alternative min_LogL persistence file '" << state_fname << "'! Error was: " << strerror(errno) << scan_end;
                }
            }

            /// Check persistence file to see if we should be using the alternative min_LogL value
            bool pluginInfo::check_alt_min_LogL_state() const
            {
                return Utils::file_exists(def_out_path+"/ALT_MIN_LOGL_IN_USE");
            }

            pluginInfo::~pluginInfo()
            {
                for (auto it = resume_streams.begin(), end = resume_streams.end(); it != end; ++it)
                {
                    delete it->second;
                }
            }

            pluginInfo::pluginInfo()
              : keepRunning(true), funcCalculating(false), MPIrank(0)
              #ifdef WITH_MPI
              , scannerComm(NULL), MPIdata_is_init(false)
              #endif
              , earlyShutdownInProgress(false)
            {}

            #ifdef WITH_MPI
            ///Initialise any MPI functionality (currently just used to provide a communicator object to ScannerBit)
            void pluginInfo::initMPIdata(GMPI::Comm* newcomm)
            {
               scannerComm = newcomm;
               MPIdata_is_init = true;
               MPIrank = scanComm().Get_rank();
            }

            GMPI::Comm& pluginInfo::scanComm()
            {
               if(not MPIdata_is_init)
               {
                  std::cerr << "Tried to retrieve scanComm MPI communicator (in ScannerBit), but it has not been initialised! Please be sure to call 'initMPIdata' and provide a communicator before allowing any scans to commence." << std::endl;
                  exit(1); //TODO not sure if this should be a standard GAMBIT error or not. Going with 'not' for now.
               }
               return *scannerComm;
            }
            #endif

            pluginInfo plugin_info;
        }
    }
}

