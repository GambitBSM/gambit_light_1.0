module gambit_light_mod
  use iso_c_binding
  implicit none

  interface
     real(c_double) function gambit_light_invalid_point() bind(c)
       use iso_c_binding
     end function gambit_light_invalid_point
     
     subroutine gambit_light_error(str_error) bind(c)
       use iso_c_binding
       character(kind=c_char), dimension(*) :: str_error
     end subroutine gambit_light_error

     subroutine gambit_light_warning(str_warning) bind(c)
       use iso_c_binding
       character(kind=c_char), dimension(*) :: str_warning
     end subroutine gambit_light_warning

     real(c_double) function gambit_light_user_like(niparams, iparams, noparams, oparams) bind(c)
       use iso_c_binding
       integer(c_int), value, intent(in) :: niparams, noparams
       type(c_ptr), intent(in), value :: iparams
       type(c_ptr), intent(in), value :: oparams
     end function gambit_light_user_like

     subroutine gambit_light_register_fcn(fcn_name, fcn_user_like) bind(c)
       use iso_c_binding
       type(c_ptr), value :: fcn_name
       type(c_funptr), value :: fcn_user_like
     end subroutine gambit_light_register_fcn
  end interface
end module gambit_light_mod
