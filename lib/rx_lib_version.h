#pragma once


#define RX_LIB_MAJOR_VERSION 1
#define RX_LIB_MINOR_VERSION 0
#define RX_LIB_BUILD_NUMBER 4


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


