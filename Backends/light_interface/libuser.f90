module user_mod
  use iso_c_binding
  use gambit_light_mod
  implicit none

contains
  
  ! User-side log-likelihood function, registered in gambit by init_like
  real(c_double) function user_loglike(niparams, iparams, noparams, oparams) bind(c)
    integer(c_int), value, intent(in) :: niparams, noparams
    type(c_ptr), intent(in), value :: iparams
    type(c_ptr), intent(in), value :: oparams

    ! fortran arrays
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

    ! error handling: return a value denoting an invalid point
    ! user_loglike = gambit_light_invalid_point()

    ! error handling: report a string warning using gambit_light_warning
    call gambit_light_warning('Some warning.'//c_null_char)
    user_loglike = fiparams(1) + fiparams(2)
    
    ! error handling: report a string error using gambit_light_error
    ! call gambit_light_error('Invalid input arguments.'//c_null_char)
    ! user_loglike = gambit_light_invalid_point()

  end function user_loglike

  ! user-side initialization function, called by gambit at init
  subroutine init_like(fcn_name, rf) bind(c)
    type(c_funptr), intent(in), value :: rf
    type(c_ptr), intent(in), value:: fcn_name
    procedure(gambit_light_register_fcn), pointer :: frf

    print *, "libuser.f90: init_like: initializing user library."

    ! convert c function to fortran function, and call
    call c_f_procpointer(rf, frf)
    call frf(fcn_name, c_funloc(user_loglike))
  end subroutine init_like
end module user_mod
