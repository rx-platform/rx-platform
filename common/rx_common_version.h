#pragma once


#define RX_COMMON_MAJOR_VERSION 2
#define RX_COMMON_MINOR_VERSION 1
#define RX_COMMON_BUILD_NUMBER 6

// Ver 1.0.1 added RX_STRUCT_TYPE support
// Ver 2.0.0 added meta data to rx-common
// Ver 2.0.1 added RX_ALWAYS_ORIGIN
// Ver 2.1.0 added struct value handling
// Ver 2.1.1 corrected rx_init_struct_array_value and rx_init_struct_array_value_with_ptrs
// Ver 2.1.3 created rx_init_bytes_array_value
// Ver 2.1.4 corrected memcmp on byte string values (rx_c_ptr)
// Ver 2.1.5 corrected rx_convert_value for arrays - error conversion bug
// Ver 2.1.6 added base64 to common
