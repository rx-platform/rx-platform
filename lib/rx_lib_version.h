#pragma once


#define RX_LIB_MAJOR_VERSION 0
#define RX_LIB_MINOR_VERSION 17
#define RX_LIB_BUILD_NUMBER 15


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
