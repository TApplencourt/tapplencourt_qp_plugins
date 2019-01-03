#include "hdf5.h"
#include <stdlib.h>
#include <string.h>

/*
      _
 |_| |_    | | _|_ o |  _
 | |  _)   |_|  |_ | | _>

*/

void strcati(char *foobar, char *foo, char *bar) {
  // Concat foo and bar and copying it  foobar"
  // Assume foobar is larg enought"
  memset(foobar, 0, strlen(foobar));
  strcpy(foobar, foo);
  strcat(foobar, bar);
}

void H5Dread666(hid_t file_id, char *h, char *path, hid_t H5T_USER, void *data) {

  herr_t status;
  hid_t dataset_id;

  char str[80];
  strcati(str, h, path);

  dataset_id = H5Dopen(file_id, str, H5P_DEFAULT);
  status = H5Dread(dataset_id, H5T_USER, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
  status = H5Dclose(dataset_id);
}

void H5Dread666_hyperslab(hid_t file_id, char *h, char *path, hsize_t *start, hsize_t *count, hid_t memspace, hid_t H5T_USER, void *data) {
  char str[80];

  hid_t dataset_id;   /* identifiers */
  hid_t dataspace_id; /* identifiers */
  herr_t status;

  memspace = H5Screate_simple(1, count, NULL);

  strcati(str, h, path);
  dataset_id = H5Dopen(file_id, str, H5P_DEFAULT);

  dataspace_id = H5Dget_space(dataset_id);
  status = H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, start, NULL, count, NULL);

  status = H5Dread(dataset_id, H5T_USER, memspace, dataspace_id, H5P_DEFAULT, data);

  status = H5Dclose(dataset_id);
  status = H5Sclose(dataspace_id);
}

void H5Dcreate666(hid_t file_id, char *h, char *path, hsize_t *dims, int dims_size, hid_t H5T_USER) {
  herr_t status;
  hid_t dataspace_id, dataset_id;

  dataspace_id = H5Screate_simple(dims_size, dims, NULL);
  char str[80];
  strcati(str, h, path);
  dataset_id = H5Dcreate2(file_id, str, H5T_USER, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  status = H5Dclose(dataset_id);
  status = H5Sclose(dataspace_id);
}

void H5Dcreate666w(hid_t file_id, char *h, char *path, hsize_t *dims, int dims_size, hid_t H5T_USER, void *data) {
  herr_t status;
  hid_t dataspace_id, dataset_id;

  dataspace_id = H5Screate_simple(dims_size, dims, NULL);
  char str[80];
  strcati(str, h, path);
  dataset_id = H5Dcreate2(file_id, str, H5T_USER, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  status = H5Dwrite(dataset_id, H5T_USER, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
  status = H5Dclose(dataset_id);
  status = H5Sclose(dataspace_id);
}

void H5Dwrite666(hid_t file_id, char *h, char *path, hid_t H5T_USER, void *data) {

  herr_t status;
  hid_t dataset_id;

  char str[80];
  strcati(str, h, path);

  dataset_id = H5Dopen(file_id, str, H5P_DEFAULT);
  status = H5Dwrite(dataset_id, H5T_USER, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
  status = H5Dclose(dataset_id);
}

/*
  _                  _
 |_)  _   _.  _|   _|_    ._   _ _|_ o  _  ._
 | \ (/_ (_| (_|    | |_| | | (_  |_ | (_) | |

*/
void get_param_h5_int_(char *h5path, int *n_bielec, int *n_hcore, int *flag) {

  char *h = (flag[0] == 0) ? "MO_basis" : "AO_basis";

  hid_t file_id;
  herr_t status;

  file_id = H5Fopen(h5path, H5F_ACC_RDONLY, H5P_DEFAULT);

  H5Dread666(file_id, h, "/integrals/bielec/n", H5T_NATIVE_INT, n_bielec);
  H5Dread666(file_id, h, "/integrals/monoelec/hcore/n", H5T_NATIVE_INT, n_hcore);

  status = H5Fclose(file_id);
}

void read_bielec_h5_(char *h5path, int32_t *bielec_idx, double *bielec_val, int *offset, int *chunk_size, int *flag) {

  char *h = (flag[0] == 0) ? "MO_basis" : "AO_basis";

  hid_t file_id; /* identifiers */
  herr_t status;

  hid_t memspace;

  file_id = H5Fopen(h5path, H5F_ACC_RDONLY, H5P_DEFAULT);

  hsize_t start[1] = {offset[0]};
  hsize_t count[1] = {chunk_size[0]};

  memspace = H5Screate_simple(1, count, NULL);

  H5Dread666_hyperslab(file_id, h, "/integrals/bielec/value", start, count, memspace, H5T_NATIVE_DOUBLE, bielec_val);
  H5Dread666_hyperslab(file_id, h, "/integrals/bielec/idx", start, count, memspace, H5T_NATIVE_LLONG, bielec_idx);

  status = H5Fclose(file_id);
}

void read_hcore_h5_(char *h5path, int32_t *bielec_idx, double *bielec_val, int *flag) {

  char *h = (flag[0] == 0) ? "MO_basis" : "AO_basis";

  hid_t file_id; /* identifiers */
  herr_t status;

  file_id = H5Fopen(h5path, H5F_ACC_RDONLY, H5P_DEFAULT);

  H5Dread666(file_id, h, "/integrals/monoelec/hcore/value", H5T_NATIVE_DOUBLE, bielec_val);
  H5Dread666(file_id, h, "/integrals/monoelec/hcore/idx", H5T_NATIVE_LLONG, bielec_idx);

  status = H5Fclose(file_id);
}

/*
                         _
 \    / ._ o _|_  _    _|_    ._   _ _|_ o  _  ._
  \/\/  |  |  |_ (/_    | |_| | | (_  |_ | (_) | |

*/

void write_init_h5_(char *h5path, int *o_tot_num, int *bi_elec_num, int *flag, double *nr) {

  herr_t status;
  hid_t dataset_id;
  hsize_t dims[2];

  hid_t file_id = H5Fcreate(h5path, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

  // =~=~=~=
  // H_core
  // =~=~=~=

  hid_t grp_crt_plist = H5Pcreate(H5P_LINK_CREATE);
  /* Set flag for intermediate group creation */
  status = H5Pset_create_intermediate_group(grp_crt_plist, 1);

  char *h = (flag[0] == 0) ? "MO_basis" : "AO_basis";
  char str[80];

  strcati(str, h, "/integrals/monoelec/hcore");
  dataset_id = H5Gcreate2(file_id, str, grp_crt_plist, H5P_DEFAULT, H5P_DEFAULT);
  H5Gclose(dataset_id);

  dims[0] = 1;
  H5Dcreate666w(file_id, h, "/integrals/monoelec/hcore/n", dims, 1, H5T_NATIVE_INT, o_tot_num);

  dims[0] = o_tot_num[0] * o_tot_num[0];
  dims[1] = 2;
  H5Dcreate666(file_id, h, "/integrals/monoelec/hcore/idx", dims, 2, H5T_NATIVE_INT);
  H5Dcreate666(file_id, h, "/integrals/monoelec/hcore/value", dims, 1, H5T_NATIVE_DOUBLE);

  strcati(str, h, "/integrals/bielec");
  dataset_id = H5Gcreate2(file_id, str, grp_crt_plist, H5P_DEFAULT, H5P_DEFAULT);
  H5Gclose(dataset_id);

  dims[0] = 1;
  H5Dcreate666w(file_id, h, "/integrals/bielec/n", dims, 1, H5T_NATIVE_INT, bi_elec_num);

  dims[0] = bi_elec_num[0];
  H5Dcreate666(file_id, h, "/integrals/bielec/idx", dims, 1, H5T_NATIVE_LLONG);
  H5Dcreate666(file_id, h, "/integrals/bielec/value", dims, 1, H5T_NATIVE_DOUBLE);



  dataset_id = H5Gcreate2(file_id, "nuclei", grp_crt_plist, H5P_DEFAULT, H5P_DEFAULT);
  H5Gclose(dataset_id);

  dims[0] = 1;
  H5Dcreate666w(file_id, "nuclei", "/nuclear_repulsion", dims, 1, H5T_NATIVE_DOUBLE, nr);



  status = H5Fclose(file_id);
}

void write_hcore_h5_(char *h5path, int *o_tot_num, double *moelec_val, int *flag) {

  hid_t file_id; /* identifiers */
  herr_t status;
  int i, j, k = 0;

  long *ptr = (long *)malloc(o_tot_num[0] * o_tot_num[0] * 2 * sizeof(long));
  for (i = 0; i < o_tot_num[0]; i++) {
    for (j = 0; j < o_tot_num[0]; j++) {
      ptr[k] = i;
      ptr[k + 1] = j;
      k += 2;
    }
  }

  char *h = (flag[0] == 0) ? "MO_basis" : "AO_basis";

  file_id = H5Fopen(h5path, H5F_ACC_RDWR, H5P_DEFAULT);

  H5Dwrite666(file_id, h, "/integrals/monoelec/hcore/idx", H5T_NATIVE_LLONG, ptr);
  H5Dwrite666(file_id, h, "/integrals/monoelec/hcore/value", H5T_NATIVE_DOUBLE, moelec_val);

  /* Close file */
  status = H5Fclose(file_id);
}

void write_bielec_h5_(char *h5path, int *bielec_idx, int *bielec_val, int *flag) {

  hid_t file_id; /* identifiers */
  herr_t status;

  file_id = H5Fopen(h5path, H5F_ACC_RDWR, H5P_DEFAULT);

  char *h = (flag[0] == 0) ? "MO_basis" : "AO_basis";

  H5Dwrite666(file_id, h, "/integrals/bielec/idx", H5T_NATIVE_LLONG, bielec_idx);
  H5Dwrite666(file_id, h, "/integrals/bielec/value", H5T_NATIVE_DOUBLE, bielec_val);

  /* Close file */
  status = H5Fclose(file_id);
}
