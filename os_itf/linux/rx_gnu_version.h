#pragma once

#define RX_HAL_NAME "Linux"
#define RX_HAL_MAJOR_VERSION 2
#define RX_HAL_MINOR_VERSION 1
#define RX_HAL_BUILD_NUMBER 1

/*********************************************************************************

0.10.3. swap bytes functions
0.10.4. changed the name of interface
0.10.5. corrected perror warning
0.10.6. version reading
0.10.7. corrected rx_file_exsits
0.10.8. corrected pipe errors
0.10.9. added UDP4 socket
0.10.10. added processor count
0.10.11. implemented rx_crypt_gen_random
0.10.12. added rx_current_thread
0.10.13. added thread name
0.10.13. corrected rx_create_and_bind_ip4_udp_socket
0.11.0. added os errors supportrx_ms_sleep
0.11.1. rx_msleep => rx_ms_sleep
0.11.2. get_nprocs_conf used for core counts
0.12.0. added rx_socket_read_from and rx_socket_write_to
1.0.0 first release version
1.0.1 udp socket supports broadcast
1.0.2 corrected file open error with O_TRUNC flag
1.0.3 support for rx-common shared library
1.1.0 added rx_thread_set_afinity
1.2.0 added raw socket support
1.2.1 corrected read after release bug
1.2.2 added rx_file_get_time_from_path, included creation time for files
1.2.3 corrected fd 0 error
1.3.0 added FIFO_SCHED
2.0.0 added TLS to platform
2.0.2 resolved year 2038 _TIME_BITS 64
2.1.0 Included IP table in Ethernet Cards list, added Multi-cast
2.1.1 Implemented rx_file_rename
*********************************************************************************/
