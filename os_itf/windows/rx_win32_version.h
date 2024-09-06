#pragma once


#define RX_HAL_NAME "Windows"
#define RX_HAL_MAJOR_VERSION 2
#define RX_HAL_MINOR_VERSION 2
#define RX_HAL_BUILD_NUMBER 1

/*********************************************************************************

0.10.3. added SO_RCVBUF and  SO_SNDBUF to zero to avoid copying
0.10.5. ALPC ports
0.10.6. Corrected rx_os_collect_time function
0.10.7. swap bytes functions
0.10.8. windows compiler version written in the right way
0.10.9. changed the name
0.10.10. pid retried internal
0.10.11. version reading
0.10.12. added some file system changes
0.10.13. corrected some pipe errors
0.10.14. removed include "targetver.h"
0.10.15. added UDP4 socket function
0.10.16. added processor count
0.10.17. added rx_current_thread
0.10.18. added thread name
0.10.19. corrected windows implementation for RX_ASSERT
0.10.19. corrected include for python support
0.10.21. removed SetThreadDescription from os itf for win32 to work with older kernels
0.11.0. added os errors support
0.11.1. resolved all of "WSAGetLastError" issues for last os error support
0.11.2. rx_msleep => rx_ms_sleep
0.11.3. corrected rx_thread_create, skips priority
0.12.0. added rx_socket_read_from and rx_socket_write_to
0.12.1. added addresses to connect callback
1.0.0 first release version
1.0.1 resolved stuff with get_us_ticks and overflow
1.0.2 implemented hd-timer option
1.0.3 ERROR_PORT_UNREACHABLE does not generate error on UDP
1.0.4 wait for single object is calculated as (t-50)/1000 + 1
1.0.5 support for rx-common shared library
1.1.0 added COM port support
1.1.1 added RtlGetVersion call
1.2.0 added Ethernet support
1.2.1 node name handling done
1.3.0 added rx_thread_set_afinity
1.4.0 first functional ethernet version
1.4.1 added rx_file_get_time_from_path, included creation time for files
2.0.0 added TLS to platform
2.0.1 Included Build Minor in OS info
2.1.0 Included IP table in Ethernet Cards list, added Multi-cast
2.2.0 VS 2022 compiler
2.2.1 implemented rx_file_rename
*********************************************************************************/
