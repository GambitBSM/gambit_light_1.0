#include "gambit_light_interface.h"

// User-side log-likelihood function, which can be called by GAMBIT-light.
double user_loglike(const std::vector<std::string>& input_names, const std::vector<double>& input_vals, std::map<std::string,double>& output)
{

    // Make a map of the inputs?
    std::map<std::string,double> input;
    for (size_t i = 0; i < input_names.size(); i++)
    {
        input[input_names[i]] = input_vals[i];
    }

    // Error handling: Report an invalid point using gambit_light_invalid_point.
    // gambit_light_invalid_point("This input point is no good.");

    // Error handling: Report a string warning using gambit_light_warning.
    gambit_light_warning("Some warning.");

    // Error handling: Report an error using gambit_light_error.
    // gambit_light_error("Some error.");

    // Error handling, alternative to using gambit_light_error: Throw a runtime_error.
    // throw std::runtime_error("Some runtime_error.");

    // Compute loglike
    double loglike = input.at("param_name_2") + input.at("param_name_3");

    // Save some extra outputs
    output["cpp_user_loglike_output_1"] = 1;
    output["cpp_user_loglike_output_2"] = 2;
    output["cpp_user_loglike_output_3"] = 3;

    return loglike;
}

GAMBIT_LIGHT_REGISTER_LOGLIKE(user_loglike)



// User-side prior transform function, which can be called by GAMBIT-light.
void user_prior(const std::vector<std::string>& input_names, const std::vector<double>& input_vals, std::vector<double>& output)
{
    for (size_t i = 0; i < input_vals.size(); i++)
    {
        output[i] = input_vals[i] * 10.;
    }
}

GAMBIT_LIGHT_REGISTER_PRIOR(user_prior)
