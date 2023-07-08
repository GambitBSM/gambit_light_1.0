#include "gambit_light_interface.h"

// User-side log-likelihood function, which can be called by GAMBIT-light.
double user_loglike(const int n_inputs, const double *input, const int n_outputs, double *output)
{
    // Error handling: Report an invalid point using gambit_light_invalid_point.
    // gambit_light_invalid_point("This input point is no good.");

    // Error handling: Report a warning using gambit_light_warning.
    gambit_light_warning("Some warning.");

    // Error handling: Report an error using gambit_light_error.
    // gambit_light_error("Some error.");

    // Compute loglike
    double loglike = input[0] + input[1] + input[2];

    // Save some extra outputs    
    output[0] = 1;
    output[1] = 2;
    output[2] = 3;

    return loglike;
}


// User-side prior transform function, which can be called by GAMBIT-light.
void user_prior(const int n_inputs, const double *input, double *output)
{
    for (int i = 0; i < n_inputs; i++)
    {
        output[i] = input[i] * 10;
    }
}

