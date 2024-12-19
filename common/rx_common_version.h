#pragma once


#define RX_COMMON_MAJOR_VERSION 3
#define RX_COMMON_MINOR_VERSION 2
#define RX_COMMON_BUILD_NUMBER 2

// Ver 1.0.1 added RX_STRUCT_TYPE support
// Ver 2.0.0 added meta data to rx-common
// Ver 2.0.1 added RX_ALWAYS_ORIGIN
// Ver 2.1.0 added struct value handling
// Ver 2.1.1 corrected rx_init_struct_array_value and rx_init_struct_array_value_with_ptrs
// Ver 2.1.3 created rx_init_bytes_array_value
// Ver 2.1.4 corrected memcmp on byte string values (rx_c_ptr)
// Ver 2.1.5 corrected rx_convert_value for arrays - error conversion bug
// Ver 2.1.6 added base64 to common
// Ver 3.0.0 cpp heap implementation
// Ver 3.0.1 corrected rx_get_struct_value
// Ver 3.1.0 added rx_get_sub_struct_value and rx_set_sub_struct_value
// Ver 3.1.3 corrected nasty bug with rx_init_bytes_value_struct function
// Ver 3.2.0 added rx_get_new_transaction_id and rx_get_new_handle
// Ver 3.2.1 added rx_init_array_value_with_move and rx_init_array_value_with_ptrs_move
// Ver 3.2.2 corrected memory leak for strings of length 8
