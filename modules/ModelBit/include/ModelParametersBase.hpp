#ifndef gambit_ModelParametersBase_hpp
#define gambit_ModelParametersBase_hpp 1

#include <vector>
#include "gambitCommonTypes.hpp"
#include "logcore.hpp"

namespace gambit {
  /*! \brief virtual base class for model parameters.
  //
  // This class is only taking care of the version
  //
  // \author Johan Lundberg
  // \date July-September 2011
  */
  class ModelParametersBase {
  public:
    u_longlong getVersion() const {
      return _paramversion ;
    }
    ModelParametersBase():_paramversion(1) {SUFIT_MSG_DEBUG("hi.");}
  protected:
    virtual ~ModelParametersBase(){SUFIT_MSG_DEBUG("bye.");}
    void updateVersion(){ _paramversion++;}
  private:
    u_longlong _paramversion;
  };
}

#endif