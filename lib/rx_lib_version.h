#pragma once


#define RX_LIB_MAJOR_VERSION 3
#define RX_LIB_MINOR_VERSION 4
#define RX_LIB_BUILD_NUMBER 2


// version 0.16.0 removed virtual references and added hashes to library
// version 0.16.6 removed c string from serialization
// version 0.17.0 introduced string and node id hashes
// version 0.17.1 introduced virtual fill_code_info function for reference_object
// version 0.17.3 added extract_value template function
// version 0.17.6 safe security handle
// version 0.17.10 handling all values serialization
// version 0.17.11 clearing tcp/ip client socket
// version 0.17.12 introduced rx_transaction_type
// version 0.17.13 implemented dump_simple for log stream subscriber
// version 0.17.14 added log_query_type::include_trace
// version 0.17.15 added rx_result for io types
// version 0.17.17 log_query_type revised, enum classes fro log enums
// version 0.18.0 added runtime_constructor stuff
// version 0.18.2 rx_result_erros_t is now global typedef
// version 0.18.3 rx_item_reference is part of lib and has specific serialization
// version 0.18.4 read simple value is in serialization
// version 0.18.5 added RX_DEAD_QUALITY
// version 0.18.6 added rx_thread_data_object stack cache before push
// version 0.18.7 added errors_line to rx_result types
// version 0.18.8 added get_CPU() function, useful for monitoring
// version 0.18.9 simplified values serialization, corrected some errors in stream_chuks_decoder<>
// version 0.18.10 sync_event changed with callback in log for testing
// version 0.18.11 timer job max_sleep_period added
// version 0.19.0 introduced protocol buffers for IO types
// version 0.19.1 slim_lock can't be moved or copied
// version 0.20.0 introduced full_lambda_job and type_less_box
// version 0.20.1 introduced function_to_go<refT, Args...>
// version 0.20.2 finished udp_socket<>
// version 0.20.3 added tcp_client_socket::bind(addr, port, ...)
// version 1.0.0 first release version
// version 1.0.1 add value and add child in runtime data accepts full path
// version 1.0.2 added unauthorized_context() function
// version 1.0.3 added binary_istream<> and binary_ostream<>
// version 1.0.4 changed rx_log_query_type, added acceding sort
// version 1.0.6 added write/read value type in streams
// version 1.0.10 added atomic values for some job flags
// version 1.1.0 prepared rx_value_union for ABI
// version 2.0.0 first version with rx-common shared library
// version 2.1.0 added argument job template
// version 2.2.0 added serial port support
// version 2.2.1 added assign_defult to value storage
// version 2.4.0 binary serialization and rx_result to common library
// version 2.5.0 introduced std::byte
// version 2.5.2 added read/write slim lock
// version 2.5.3 added utf8_to_ascii
// version 2.5.4 added deserialization of bytes value
// version 2.6.0 added bytes value support, string and bytes address
// version 2.6.1 added numeric, string and bytes array values support
// version 2.6.1 added ASN library
// version 2.7.0 added member_check.h
// version 2.7.1 fill_code_info is const
// version 2.7.2 rx_value has increment_signal_level
// version 2.7.3 support for ARM dynamic linking
// version 2.7.4 added ASN writer
// version 2.7.5 added get_origin to rx_value
// version 2.8.0 ABI version added
// version 2.9.0 added RX_STRUCT_TYPE support
// version 3.0.0 added meta data and run at
// version 3.1.0 added struct values support
// version 3.1.1 corrected guid deserialization
// version 3.2.0 added struct reading support
// version 3.2.1 corrected deserialization of time, uuid, node id and bytes array
// version 3.2.2 corrected deserialization array regarding type
// version 3.2.3 added string_view to runtime data interface
// version 3.2.4 added CRLF line endings on pretty-writer (no support so changed "prettywriter.h")
// version 3.2.5 corrected support for values and IP4
// version 3.2.6 added running at to meta_data
// version 3.2.7 moved base64 to common
// version 3.2.8 added set_signal_level to rx_value
// version 3.2.9 added create_string function
// version 3.3.0 added create_string function
// version 3.3.1 alloc from buffer receives size
// version 3.3.2 alloc from buffer receives size
// version 3.3.3 mode_type_in_error
// version 3.4.0 added security guards
// version 3.4.1 added ISO-8601 as separate files
// version 3.4.2 extended a rx_const_io_buffer with size

