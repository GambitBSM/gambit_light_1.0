module gambit_light_interface_mod
  use iso_c_binding
  implicit none

  interface
     subroutine gambit_light_invalid_point(str_invalid_point) bind(c)
       use iso_c_binding
       character(kind=c_char), dimension(*) :: str_invalid_point
     end subroutine gambit_light_invalid_point
     
     subroutine gambit_light_error(str_error) bind(c)
       use iso_c_binding
       character(kind=c_char), dimension(*) :: str_error
     end subroutine gambit_light_error

     subroutine gambit_light_warning(str_warning) bind(c)
       use iso_c_binding
       character(kind=c_char), dimension(*) :: str_warning
     end subroutine gambit_light_warning

     subroutine gambit_light_register_fcn(fcn_name, fcn_user_loglike) bind(c)
       use iso_c_binding
       type(c_ptr), value :: fcn_name
       type(c_funptr), value :: fcn_user_loglike
     end subroutine gambit_light_register_fcn

     subroutine gambit_light_register_prior_fcn(fcn_user_prior) bind(c)
       use iso_c_binding
       type(c_funptr), value :: fcn_user_prior
     end subroutine gambit_light_register_prior_fcn

  end interface
end module gambit_light_interface_mod
