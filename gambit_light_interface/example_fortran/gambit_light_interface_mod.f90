
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

  end interface
end module gambit_light_interface_mod

