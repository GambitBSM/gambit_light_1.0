module light_interface_mod
  use iso_c_binding
  implicit none

  interface
     real(c_double) function light_interface_user_like(niparams, iparams, noparams, oparams) bind(c)
       use iso_c_binding
       integer(c_int), value, intent(in) :: niparams, noparams
       type(c_ptr), intent(in), value :: iparams
       type(c_ptr), intent(in), value :: oparams
     end function light_interface_user_like

     subroutine light_interface_register_fcn(fcn_name, fcn_user_like) bind(c)
       use iso_c_binding
       type(c_ptr), value :: fcn_name
       type(c_funptr), value :: fcn_user_like
     end subroutine light_interface_register_fcn
  end interface
end module light_interface_mod
