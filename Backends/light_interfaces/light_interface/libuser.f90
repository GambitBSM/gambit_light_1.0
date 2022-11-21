module user_mod
  use iso_c_binding
  use light_interface_mod
  implicit none

contains
  
  ! user-side likelyhood function, registered in gambit by init_like
  real(c_double) function user_like(niparams, iparams, noparams, oparams) bind(c)
    integer(c_int), value, intent(in) :: niparams, noparams
    type(c_ptr), intent(in), value :: iparams
    type(c_ptr), intent(in), value :: oparams

    ! fortran arrays
    real(c_double), dimension(:), pointer :: fiparams
    real(c_double), dimension(:), pointer :: foparams
    integer :: pi

    print *, "libuser.f90: user_like: computing loglike."
    if (niparams == 0) then
      user_like = 0
      return
    end if

    call c_f_pointer(iparams, fiparams, shape=[niparams])
    call c_f_pointer(oparams, foparams, shape=[noparams])
    
    pi = 1
    do while(pi<=noparams)
      foparams(pi) = pi
      pi = pi + 1
    end do

    user_like = fiparams(1) + fiparams(2)

    ! error handling: return a value denoting an invalid point
    ! user_like = light_interface_invalid_point()

    ! error handling: report a string warning using light_interface_warning
    ! call light_interface_warning('Some warning.'//c_null_char)
    ! user_like = fiparams(1) + fiparams(2)
    
    ! error handling: report a string error using light_interface_error
    ! call light_interface_error('Invalid input arguments.'//c_null_char)
    ! user_like = light_interface_invalid_point()

  end function user_like

  ! user-side initialization function, called by gambit at init
  subroutine init_like(fcn_name, rf) bind(c)
    type(c_funptr), intent(in), value :: rf
    type(c_ptr), intent(in), value:: fcn_name
    procedure(light_interface_register_fcn), pointer :: frf

    print *, "libuser.f90: init_like: initializing user library."

    ! convert c function to fortran function, and call
    call c_f_procpointer(rf, frf)
    call frf(fcn_name, c_funloc(user_like))
  end subroutine init_like
end module user_mod
