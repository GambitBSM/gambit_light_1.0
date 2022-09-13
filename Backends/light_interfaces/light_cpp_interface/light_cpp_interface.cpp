#include<iostream>
#include<string>
#include<map>

extern "C"
void run(const std::map<std::string,double>& input, std::map<std::string,double>& output)
{

  std::cout << "light_cpp_interface: run: "
            << "Will now construct the output map by "
            << "adding 1.0 to all values in the input map."
            << std::endl;

  for (const auto& kv : input)
  {
    output[kv.first] = kv.second + 1.0;
  }

  // Add the expected "loglike" entry
  output["loglike"] = input.at("p1") + input.at("p2");

}