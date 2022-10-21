module user_mod
  use iso_c_binding
  use light_interface_mod
  implicit none

contains
  
  ! user-side likelyhood function, registered in gambit by init_like
  real(c_double) function user_like(nparams, iparams, oparams) bind(c)
    integer(c_int), value, intent(in) :: nparams
    type(c_ptr), intent(in), value :: iparams
    type(c_ptr), intent(in), value :: oparams

    ! fortran arrays
    real(c_double), dimension(:), pointer :: fiparams
    real(c_double), dimension(:), pointer :: foparams
    integer :: pi

    print *, "libuser.f90: user_like: computing loglike."
    if (nparams == 0) then
      user_like = 0
      return
    end if

    call c_f_pointer(iparams, fiparams, shape=[nparams])
    call c_f_pointer(oparams, foparams, shape=[nparams])
    
    pi = 1
    do while(pi<=nparams)
      foparams(pi) = fiparams(pi) + 1
      pi = pi + 1
    end do

    user_like = fiparams(1) + fiparams(12)
  end function user_like

  ! user-side initialization function, called by gambit at init
  subroutine init_like(rf) bind(c)
    type(c_funptr), intent(in), value :: rf
    procedure(light_interface_register_fcn), pointer :: frf
    character(len=11), target :: fcn_name

    print *, "libuser.f90: init_like: initializing user library."

    ! registered function name (c-compatible, null-terminated)
    fcn_name = "fuser_like"
    fcn_name(len(fcn_name):) = c_null_char 
   
    ! convert c function to fortran function, and call
    call c_f_procpointer(rf, frf)
    call frf(c_loc(fcn_name), c_funloc(user_like))
  end subroutine init_like
end module user_mod
