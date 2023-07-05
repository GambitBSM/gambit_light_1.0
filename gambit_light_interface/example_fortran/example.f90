include 'gambit_light_interface_mod.f90'

module user_mod
  use iso_c_binding
  use gambit_light_interface_mod
  implicit none

contains
  
  ! User-side log-likelihood function
  real(c_double) function user_loglike(n_inputs, input, n_outputs, output) bind(c)
    integer(c_int), value, intent(in) :: n_inputs, n_outputs
    type(c_ptr), intent(in), value :: input, output

    ! Create pointers to Fortran arrays
    real(c_double), dimension(:), pointer :: finput
    real(c_double), dimension(:), pointer :: foutput

    ! Connect the C arrays (input, output) to the Fortran arrays (finput, foutput)
    call c_f_pointer(input, finput, shape=[n_inputs])
    call c_f_pointer(output, foutput, shape=[n_outputs])
  
    print *, "example.f90: user_loglike: Computing loglike."

    ! Error handling: Report an invalid point usiong gambit_light_invalid_point.
    ! call gambit_light_invalid_point('This input point is no good.'//c_null_char)

    ! Error handling: Report a warning using gambit_light_warning.
    call gambit_light_warning('Some warning.'//c_null_char)
    
    ! Error handling: Report an error using gambit_light_error.
    ! call gambit_light_error('Some error.'//c_null_char)

    foutput(1) = 10 * finput(1)
    foutput(2) = 10 * finput(2)
    
    user_loglike = finput(1) * finput(1) + finput(2) * finput(2)

  end function user_loglike



  ! User-side prior transform function
  subroutine user_prior(n_inputs, input, output) bind(c)
    integer(c_int), value, intent(in) :: n_inputs
    type(c_ptr), intent(in), value :: input, output
    integer :: i

    ! Create pointers to Fortran arrays
    real(c_double), dimension(:), pointer :: finput
    real(c_double), dimension(:), pointer :: foutput

    ! Connect the C arrays (input, output) to the Fortran arrays (finput, foutput)
    call c_f_pointer(input, finput, shape=[n_inputs])
    call c_f_pointer(output, foutput, shape=[n_inputs])
  
    print *, "example.f90: user_prior: Transforming sample from unit hypercube."

    do i = 1, n_inputs
      foutput(i) = finput(i) * 10
    end do
    
  end subroutine user_prior

end module user_mod