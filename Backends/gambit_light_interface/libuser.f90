module user_mod
  use iso_c_binding
  use gambit_light_interface_mod
  implicit none

contains
  
  ! User-side log-likelihood function, registered in GAMBIT by init_user_loglike.
  real(c_double) function user_loglike(niparams, iparams, noparams, oparams) bind(c)
    integer(c_int), value, intent(in) :: niparams, noparams
    type(c_ptr), intent(in), value :: iparams
    type(c_ptr), intent(in), value :: oparams

    ! Fortran arrays
    real(c_double), dimension(:), pointer :: fiparams
    real(c_double), dimension(:), pointer :: foparams
    integer :: pi

    print *, "libuser.f90: user_loglike: computing loglike."
    if (niparams == 0) then
      user_loglike = 0
      return
    end if

    call c_f_pointer(iparams, fiparams, shape=[niparams])
    call c_f_pointer(oparams, foparams, shape=[noparams])
    
    pi = 1
    do while(pi<=noparams)
      foparams(pi) = pi
      pi = pi + 1
    end do

    ! user_loglike = fiparams(1) + fiparams(2)

    ! Error handling: Report an invalid point usiong gambit_light_invalid_point.
    ! call gambit_light_invalid_point('This input point is no good.'//c_null_char)

    ! Error handling: Report a warning using gambit_light_warning.
    ! call gambit_light_warning('Some warning.'//c_null_char)
    
    ! Error handling: Report an error using gambit_light_error.
    ! call gambit_light_error('Some error.'//c_null_char)

    user_loglike = fiparams(1) + fiparams(2)

  end function user_loglike

  ! User-side initialisation function, called by GAMBIT at init.
  subroutine init_user_loglike(fcn_name, rf) bind(c)
    type(c_funptr), intent(in), value :: rf
    type(c_ptr), intent(in), value:: fcn_name
    procedure(gambit_light_register_fcn), pointer :: frf

    print *, "libuser.f90: init_user_loglike: initialising user library."

    ! Convert c function to fortran function, and call it.
    call c_f_procpointer(rf, frf)
    call frf(fcn_name, c_funloc(user_loglike))
  end subroutine init_user_loglike
end module user_mod
