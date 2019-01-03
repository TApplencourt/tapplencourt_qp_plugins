program main

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
    double precision, allocatable :: buffer_values_hcore_h5(:)

    real(integral_kind), allocatable :: buffer_values_hcore(:,:)

    character (len = 20) :: chunk_size_str
    integer :: chunk_size     
    

    integer :: basis_id = 1 
    integer :: i,j, a,b
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

     CALL GET_ENVIRONMENT_VARIABLE("h5_chunk_size", chunk_size_str, status = err)
     if ( err .ne. 0) then
          chunk_size = 100*1000
    else
          read(chunk_size_str, '(i)') chunk_size
    endif


    call get_param_h5_int(trim(h5path)//c_null_char, n_bielec_int, n_hcore_int, basis_id)
    if ( basis_id.eq. 0 ) then
        o_tot_num_h5 = mo_tot_num
    else
        o_tot_num_h5 = ao_num
    endif
    !                                             _                
    ! |\/|  _  ._   _   _  |  _   _      /   |_| /   _  ._ _    \  
    ! |  | (_) | | (_) (/_ | (/_ (_     |    | | \_ (_) | (/_    | 
    !                                    \                      /  
    allocate(buffer_i_hcore(2,n_hcore_int), buffer_values_hcore_h5(n_hcore_int), buffer_values_hcore(o_tot_num_h5,o_tot_num_h5))
    call read_hcore_h5(trim(h5path)//c_null_char,buffer_i_hcore, buffer_values_hcore_h5, basis_id)

    buffer_values_hcore(:,:) = 0.d0 ! Assume that we may read sparse
                                    ! reprensation, but we store dense one
    !$PRAGMA OMP SIMD PRIVATE(a,b) 
    do j = 1, n_hcore_int*n_hcore_int
            a = buffer_i_hcore(1,j)
            b = buffer_i_hcore(2,j)
            buffer_values_hcore(a+1,b+1) =buffer_values_hcore_h5(j) 
    end do

    if (basis_id == 0) then
        call ezfio_set_mo_one_e_integrals_integral_c(buffer_values_hcore)
        call ezfio_set_mo_one_e_integrals_disk_access_mo_one_integrals_c("Read")
    else
        call ezfio_set_ao_one_e_integrals_integral_c(buffer_values_hcore)
        call ezfio_set_ao_one_e_integrals_disk_access_ao_one_integrals_c("Read")
    endif
    deallocate(buffer_i_hcore, buffer_values_hcore_h5, buffer_values_hcore)

    !  _     _          
    ! |_) o |_ |  _   _ 
    ! |_) | |_ | (/_ (_ 
    !                   
    allocate(buffer_values(chunk_size), buffer_i(chunk_size))
    
    print*, 'Bielect integral: number of int to read: ', n_bielec_int

    n_to_read = n_bielec_int
    i_int = 0

    do while ( (n_to_read .ne. 0) .and. (i_int .ne. n_bielec_int))
        chuck_size_cur = min(chunk_size, n_to_read)         
        call read_bielec_h5(trim(h5path)//c_null_char, buffer_i,buffer_values,i_int, chuck_size_cur, basis_id)
        if (basis_id == 0) then
            call insert_into_mo_integrals_map(chuck_size_cur, buffer_i,buffer_values)
        else
            !call insert_into_ao_integrals_map(chuck_size_cur, buffer_i,buffer_values)
            call map_update(ao_integrals_map, buffer_i, buffer_values, chuck_size_cur)
        endif
        i_int = i_int + chuck_size_cur
        n_to_read = n_to_read - chuck_size_cur
        print *, 'Bielect integral: Commited ', i_int, 'integral ', n_to_read, 'to go'
   end do

    print*, 'Sorting the mo_integrals_map'
    if (basis_id == 0) then
        call map_sort(mo_integrals_map)
        !call map_unique(mo_integrals_map) ! Assume key are already unique
    else
        call map_sort(ao_integrals_map)
    endif 

    if (basis_id == 0) then 
        call map_sort(mo_integrals_map)
        call map_save_to_disk(trim(ezfio_filename)//'/work/mo_ints',mo_integrals_map)
        call ezfio_set_integrals_bielec_disk_access_mo_integrals('Read')
    else 
        call map_sort(ao_integrals_map)
        call map_save_to_disk(trim(ezfio_filename)//'/work/ao_ints',ao_integrals_map)
        call ezfio_set_integrals_bielec_disk_access_ao_integrals('Read')
    endif
    !Now n_to_read == 0 and i_int = n_bielec_int)
    deallocate(buffer_values,buffer_i)

end program main


