program print_integrals

    use iso_c_binding
    use map_module

    implicit none

    character (len = 100) :: h5path
    character (len = 1) :: basis_id_str

    integer :: err

    integer :: n_bielec_int, n_hcore_int, n_to_read, o_tot_num_h5
    integer :: i_int, chuck_size_cur

    real(integral_kind), allocatable :: buffer_values(:)
    integer(key_kind), allocatable :: buffer_i(:)

    integer(key_kind), allocatable :: buffer_i_hcore(:,:)
    real(integral_kind), allocatable :: buffer_values_hcore_h5(:)

    double precision, allocatable :: buffer_values_hcore(:,:)
    integer, parameter :: chunk_size = 1 !0 !100*1000

    integer :: basis_id = 1
    integer*8 :: i,j, a,b, n

    integer :: o_tot_num
    PROVIDE ezfio_filename
    !  _
    ! |_) _. ._ _. ._ _
    ! |  (_| | (_| | | |
    !
    CALL GET_ENVIRONMENT_VARIABLE("h5_int_path", h5path, status = err)
    if ( err .ne. 0) then
        h5path  = "qp_in.qp.h5"
    endif

    ! basis_id == 0 -> MO ; basis_id != 0 -> AO. Default basis_id == 0
    CALL GET_ENVIRONMENT_VARIABLE("h5_basis_id", basis_id_str, status = err)
    if ( err .ne. 0) then
        basis_id = 0
    else
        read(basis_id_str, '(i)') basis_id
    endif

    ! Set param in hd5f

    if (basis_id .eq. 0) then
        o_tot_num = mo_tot_num
    else
        o_tot_num = ao_num
    endif

    !                                             _
    ! |\/|  _  ._   _   _  |  _   _      /   |_| /   _  ._ _    \
    ! |  | (_) | | (_) (/_ | (/_ (_     |    | | \_ (_) | (/_    |
    !                                    \                      /
    
    integer*8 :: get_mo_map_size, get_ao_map_size, o_map_size

   integer(cache_key_kind), pointer :: key(:)
   real(integral_kind), pointer   :: val(:)

   if (basis_id .eq. 0) then
        PROVIDE mo_bielec_integrals_in_map
        o_map_size = get_mo_map_size();
   else
        PROVIDE ao_bielec_integrals_in_map
        o_map_size = get_ao_map_size();
   endif
   
   allocate (buffer_values(o_map_size), buffer_i(o_map_size))

   integer*8 k
   k = 0

   if (basis_id .eq. 0) then
    do i=0_8,mo_integrals_map%map_size
        key => mo_integrals_map%map(i)%key
        val => mo_integrals_map%map(i)%value
        n = mo_integrals_map%map(i)%n_elements
        do j = 1, n
            if (abs(val(j)) >= 1.E-15) then
                k = k + 1
                buffer_i(k) = key(j)
                buffer_values(k) = val(j)
            endif
        end do
    enddo
   else
    do i=0_8,ao_integrals_map%map_size
        key => ao_integrals_map%map(i)%key
        val => ao_integrals_map%map(i)%value
        n = ao_integrals_map%map(i)%n_elements
        do j = 1, n
            if (abs(val(j)) >= 1.E-15) then
                k = k + 1
                buffer_i(k) = key(j)
                buffer_values(k) = val(j)
            endif
        end do
    enddo
   endif
   
  call write_init_h5(trim(h5path)//c_null_char, o_tot_num, k, basis_id, nuclear_repulsion)

  
  call write_bielec_h5(trim(h5path)//c_null_char, buffer_i, buffer_values,basis_id)

  if (basis_id .eq. 0) then
      call write_hcore_h5(trim(h5path)//c_null_char, o_tot_num, mo_mono_elec_integral, basis_id)
  else
      call write_hcore_h5(trim(h5path)//c_null_char, o_tot_num, ao_mono_elec_integral, basis_id)
  endif

end
