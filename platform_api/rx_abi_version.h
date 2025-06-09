#pragma once


#define RX_ABI_MAJOR_VERSION 1
#define RX_ABI_MINOR_VERSION 5
#define RX_ABI_BUILD_NUMBER 0

// Ver 1.1.0 struct platform_runtime_api3_t rxBindPlugin3 [bind version 3]
// Ver 1.1.1 renamed prxCtxSetRemotePending to prxCtxSetAsyncPending [bind version 3
// Ver 1.1.2 changed rx_event_fired_t and rx_mapper_event_fired_t [bind version 3]
// Ver 1.2.0 added get data type for source and mappers [bind version 4]
// Ver 1.3.0 added get data type for source and mappers [bind version 5]
// Ver 1.3.1 corrected event binding that was missing
// Ver 1.4.0 added execute and write binded [bind version 6]
// Ver 1.4.1 no changes on interface just corrected version time for plugins [bind version 6]
// Ver 1.5.0 added weak pointer support  [bind version 6]
