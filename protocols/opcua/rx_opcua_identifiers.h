

/****************************************************************************
*
*  protocols\opcua\rx_opcua_identifiers.h
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
*  Copyright (c) 2018-2019 Dusan Ciric
*
*  
*  This file is part of {rx-platform} 
*
*  
*  {rx-platform} is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  {rx-platform} is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License  
*  along with {rx-platform}. It is also available in any {rx-platform} console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_opcua_identifiers_h
#define rx_opcua_identifiers_h 1


#include "rx_opcua_ids.h"


namespace protocols {
namespace opcua {
namespace ids {

//Error Results

const uint32_t opcid_OK = 0x00000000;

const uint32_t opcid_Bad_UnexpectedError = 0x80010000;
const uint32_t opcid_Bad_InternalError = 0x80020000;
const uint32_t opcid_Bad_OutOfMemory = 0x80030000;
const uint32_t opcid_Bad_ResourceUnavailable = 0x80040000;
const uint32_t opcid_Bad_CommunicationError = 0x80050000;
const uint32_t opcid_Bad_EncodingError = 0x80060000;
const uint32_t opcid_Bad_DecodingError = 0x80070000;
const uint32_t opcid_Bad_EncodingLimitsExceeded = 0x80080000;
const uint32_t opcid_Bad_UnknownResponse = 0x80090000;
const uint32_t opcid_Bad_Timeout = 0x800A0000;
const uint32_t opcid_Bad_ServiceUnsupported = 0x800B0000;
const uint32_t opcid_Bad_Shutdown = 0x800C0000;
const uint32_t opcid_Bad_ServerNotConnected = 0x800D0000;
const uint32_t opcid_Bad_ServerHalted = 0x800E0000;
const uint32_t opcid_Bad_NothingToDo = 0x800F0000;
const uint32_t opcid_Bad_TooManyOperations = 0x80100000;
const uint32_t opcid_Bad_DataTypeIdUnknown = 0x80110000;
const uint32_t opcid_Bad_CertificateInvalid = 0x80120000;
const uint32_t opcid_Bad_SecurityChecksFailed = 0x80130000;
const uint32_t opcid_Bad_CertificateTimeInvalid = 0x80140000;
const uint32_t opcid_Bad_CertificateIssuerTimeInvalid = 0x80150000;
const uint32_t opcid_Bad_CertificateHostNameInvalid = 0x80160000;
const uint32_t opcid_Bad_CertificateUriInvalid = 0x80170000;
const uint32_t opcid_Bad_CertificateUseNotAllowed = 0x80180000;
const uint32_t opcid_Bad_CertificateIssuerUseNotAllowed = 0x80190000;
const uint32_t opcid_Bad_CertificateUntrusted = 0x801A0000;
const uint32_t opcid_Bad_CertificateRevocationUnknown = 0x801B0000;
const uint32_t opcid_Bad_CertificateIssuerRevocationUnknown = 0x801C0000;
const uint32_t opcid_Bad_CertificateRevoked = 0x801D0000;
const uint32_t opcid_Bad_CertificateIssuerRevoked = 0x801E0000;
const uint32_t opcid_Bad_UserAccessDenied = 0x801F0000;
const uint32_t opcid_Bad_IdentityTokenInvalid = 0x80200000;
const uint32_t opcid_Bad_IdentityTokenRejected = 0x80210000;
const uint32_t opcid_Bad_SecureChannelIdInvalid = 0x80220000;
const uint32_t opcid_Bad_InvalidTimestamp = 0x80230000;
const uint32_t opcid_Bad_NonceInvalid = 0x80240000;
const uint32_t opcid_Bad_SessionIdInvalid = 0x80250000;
const uint32_t opcid_Bad_SessionClosed = 0x80260000;
const uint32_t opcid_Bad_SessionNotActivated = 0x80270000;
const uint32_t opcid_Bad_SubscriptionIdInvalid = 0x80280000;
const uint32_t opcid_Bad_RequestHeaderInvalid = 0x802A0000;
const uint32_t opcid_Bad_TimestampsToReturnInvalid = 0x802B0000;
const uint32_t opcid_Bad_RequestCancelledByClient = 0x802C0000;
const uint32_t opcid_Good_SubscriptionTransferred = 0x002D0000;
const uint32_t opcid_Good_CompletesAsynchronously = 0x002E0000;
const uint32_t opcid_Good_Overload = 0x002F0000;
const uint32_t opcid_Good_Clamped = 0x00300000;
const uint32_t opcid_Bad_NoCommunication = 0x80310000;
const uint32_t opcid_Bad_WaitingForInitialData = 0x80320000;
const uint32_t opcid_Bad_NodeIdInvalid = 0x80330000;
const uint32_t opcid_Bad_NodeIdUnknown = 0x80340000;
const uint32_t opcid_Bad_AttributeIdInvalid = 0x80350000;
const uint32_t opcid_Bad_IndexRangeInvalid = 0x80360000;
const uint32_t opcid_Bad_IndexRangeNoData = 0x80370000;
const uint32_t opcid_Bad_DataEncodingInvalid = 0x80380000;
const uint32_t opcid_Bad_DataEncodingUnsupported = 0x80390000;
const uint32_t opcid_Bad_NotReadable = 0x803A0000;
const uint32_t opcid_Bad_NotWritable = 0x803B0000;
const uint32_t opcid_Bad_OutOfRange = 0x803C0000;
const uint32_t opcid_Bad_NotSupported = 0x803D0000;
const uint32_t opcid_Bad_NotFound = 0x803E0000;
const uint32_t opcid_Bad_ObjectDeleted = 0x803F0000;
const uint32_t opcid_Bad_NotImplemented = 0x80400000;
const uint32_t opcid_Bad_MonitoringModeInvalid = 0x80410000;
const uint32_t opcid_Bad_MonitoredItemIdInvalid = 0x80420000;
const uint32_t opcid_Bad_MonitoredItemFilterInvalid = 0x80430000;
const uint32_t opcid_Bad_MonitoredItemFilterUnsupported = 0x80440000;
const uint32_t opcid_Bad_FilterNotAllowed = 0x80450000;
const uint32_t opcid_Bad_StructureMissing = 0x80460000;
const uint32_t opcid_Bad_EventFilterInvalid = 0x80470000;
const uint32_t opcid_Bad_ContentFilterInvalid = 0x80480000;
const uint32_t opcid_Bad_FilterOperandInvalid = 0x80490000;
const uint32_t opcid_Bad_ContinuationPointInvalid = 0x804A0000;
const uint32_t opcid_Bad_NoContinuationPoints = 0x804B0000;
const uint32_t opcid_Bad_ReferenceTypeIdInvalid = 0x804C0000;
const uint32_t opcid_Bad_BrowseDirectionInvalid = 0x804D0000;
const uint32_t opcid_Bad_NodeNotInView = 0x804E0000;
const uint32_t opcid_Bad_ServerUriInvalid = 0x804F0000;
const uint32_t opcid_Bad_ServerNameMissing = 0x80500000;
const uint32_t opcid_Bad_DiscoveryUrlMissing = 0x80510000;
const uint32_t opcid_Bad_SempahoreFileMissing = 0x80520000;
const uint32_t opcid_Bad_RequestTypeInvalid = 0x80530000;
const uint32_t opcid_Bad_SecurityModeRejected = 0x80540000;
const uint32_t opcid_Bad_SecurityPolicyRejected = 0x80550000;
const uint32_t opcid_Bad_TooManySessions = 0x80560000;
const uint32_t opcid_Bad_UserSignatureInvalid = 0x80570000;
const uint32_t opcid_Bad_ApplicationSignatureInvalid = 0x80580000;
const uint32_t opcid_Bad_NoValidCertificates = 0x80590000;
const uint32_t opcid_Bad_RequestCancelledByRequest = 0x805A0000;
const uint32_t opcid_Bad_ParentNodeIdInvalid = 0x805B0000;
const uint32_t opcid_Bad_ReferenceNotAllowed = 0x805C0000;
const uint32_t opcid_Bad_NodeIdRejected = 0x805D0000;
const uint32_t opcid_Bad_NodeIdExists = 0x805E0000;
const uint32_t opcid_Bad_NodeClassInvalid = 0x805F0000;
const uint32_t opcid_Bad_BrowseNameInvalid = 0x80600000;
const uint32_t opcid_Bad_BrowseNameDuplicated = 0x80610000;
const uint32_t opcid_Bad_NodeAttributesInvalid = 0x80620000;
const uint32_t opcid_Bad_TypeDefinitionInvalid = 0x80630000;
const uint32_t opcid_Bad_SourceNodeIdInvalid = 0x80640000;
const uint32_t opcid_Bad_TargetNodeIdInvalid = 0x80650000;
const uint32_t opcid_Bad_DuplicateReferenceNotAllowed = 0x80660000;
const uint32_t opcid_Bad_InvalidSelfReference = 0x80670000;
const uint32_t opcid_Bad_ReferenceLocalOnly = 0x80680000;
const uint32_t opcid_Bad_NoDeleteRights = 0x80690000;
const uint32_t opcid_Bad_ServerIndexInvalid = 0x806A0000;
const uint32_t opcid_Bad_ViewIdUnknown = 0x806B0000;
const uint32_t opcid_Uncertain_ReferenceOutOfServer = 0x406C0000;
const uint32_t opcid_Bad_TooManyMatches = 0x806D0000;
const uint32_t opcid_Bad_QueryTooComplex = 0x806E0000;
const uint32_t opcid_Bad_NoMatch = 0x806F0000;
const uint32_t opcid_Bad_MaxAgeInvalid = 0x80700000;
const uint32_t opcid_Bad_HistoryOperationInvalid = 0x80710000;
const uint32_t opcid_Bad_HistoryOperationUnsupported = 0x80720000;
const uint32_t opcid_Bad_WriteNotSupported = 0x80730000;
const uint32_t opcid_Bad_TypeMismatch = 0x80740000;
const uint32_t opcid_Bad_MethodInvalid = 0x80750000;
const uint32_t opcid_Bad_ArgumentsMissing = 0x80760000;
const uint32_t opcid_Bad_TooManySubscriptions = 0x80770000;
const uint32_t opcid_Bad_TooManyPublishRequests = 0x80780000;
const uint32_t opcid_Bad_NoSubscription = 0x80790000;
const uint32_t opcid_Bad_SequenceNumberUnknown = 0x807A0000;
const uint32_t opcid_Bad_MessageNotAvailable = 0x807B0000;
const uint32_t opcid_Bad_InsufficientClientProfile = 0x807C0000;
const uint32_t opcid_Bad_TcpServerTooBusy = 0x807D0000;
const uint32_t opcid_Bad_TcpMessageTypeInvalid = 0x807E0000;
const uint32_t opcid_Bad_TcpSecureChannelUnknown = 0x807F0000;
const uint32_t opcid_Bad_TcpMessageTooLarge = 0x80800000;
const uint32_t opcid_Bad_TcpNotEnoughResources = 0x80810000;
const uint32_t opcid_Bad_TcpInternalError = 0x80820000;
const uint32_t opcid_Bad_TcpEndpointUrlInvalid = 0x80830000;
const uint32_t opcid_Bad_RequestInterrupted = 0x80840000;
const uint32_t opcid_Bad_RequestTimeout = 0x80850000;
const uint32_t opcid_Bad_SecureChannelClosed = 0x80860000;
const uint32_t opcid_Bad_SecureChannelTokenUnknown = 0x80870000;
const uint32_t opcid_Bad_SequenceNumberInvalid = 0x80880000;
const uint32_t opcid_Bad_ConfigurationError = 0x80890000;
const uint32_t opcid_Bad_NotConnected = 0x808A0000;
const uint32_t opcid_Bad_DeviceFailure = 0x808B0000;
const uint32_t opcid_Bad_SensorFailure = 0x808C0000;
const uint32_t opcid_Bad_OutOfService = 0x808D0000;
const uint32_t opcid_Bad_DeadbandFilterInvalid = 0x808E0000;
const uint32_t opcid_Uncertain_NoCommunicationLastUsableValue = 0x408F0000;
const uint32_t opcid_Uncertain_LastUsableValue = 0x40900000;
const uint32_t opcid_Uncertain_SubstituteValue = 0x40910000;
const uint32_t opcid_Uncertain_InitialValue = 0x40920000;
const uint32_t opcid_Uncertain_SensorNotAccurate = 0x40930000;
const uint32_t opcid_Uncertain_EngineeringUnitsExceeded = 0x40940000;
const uint32_t opcid_Uncertain_SubNormal = 0x40950000;
const uint32_t opcid_Good_LocalOverride = 0x00960000;
const uint32_t opcid_Bad_RefreshInProgress = 0x80970000;
const uint32_t opcid_Bad_ConditionAlreadyDisabled = 0x80980000;
const uint32_t opcid_Bad_ConditionDisabled = 0x80990000;
const uint32_t opcid_Bad_EventIdUnknown = 0x809A0000;
const uint32_t opcid_Bad_NoData = 0x809B0000;
const uint32_t opcid_Bad_NoBound = 0x809C0000;
const uint32_t opcid_Bad_DataLost = 0x809D0000;
const uint32_t opcid_Bad_DataUnavailable = 0x809E0000;
const uint32_t opcid_Bad_EntryExists = 0x809F0000;
const uint32_t opcid_Bad_NoEntryExists = 0x80A00000;
const uint32_t opcid_Bad_TimestampNotSupported = 0x80A10000;
const uint32_t opcid_Good_EntryInserted = 0x00A20000;
const uint32_t opcid_Good_EntryReplaced = 0x00A30000;
const uint32_t opcid_Uncertain_DataSubNormal = 0x40A40000;
const uint32_t opcid_Good_NoData = 0x00A50000;
const uint32_t opcid_Good_MoreData = 0x00A60000;
const uint32_t opcid_Good_CommunicationEvent = 0x00A70000;
const uint32_t opcid_Good_ShutdownEvent = 0x00A80000;
const uint32_t opcid_Good_CallAgain = 0x00A90000;
const uint32_t opcid_Good_NonCriticalTimeout = 0x00AA0000;
const uint32_t opcid_Bad_InvalidArgument = 0x80AB0000;
const uint32_t opcid_Bad_ConnectionRejected = 0x80AC0000;
const uint32_t opcid_Bad_Disconnect = 0x80AD0000;
const uint32_t opcid_Bad_ConnectionClosed = 0x80AE0000;
const uint32_t opcid_Bad_InvalidState = 0x80AF0000;
const uint32_t opcid_Bad_EndOfStream = 0x80B00000;
const uint32_t opcid_Bad_NoDataAvailable = 0x80B10000;
const uint32_t opcid_Bad_WaitingForResponse = 0x80B20000;
const uint32_t opcid_Bad_OperationAbandoned = 0x80B30000;
const uint32_t opcid_Bad_ExpectedStreamToBlock = 0x80B40000;
const uint32_t opcid_Bad_WouldBlock = 0x80B50000;
const uint32_t opcid_Bad_SyntaxError = 0x80B60000;
const uint32_t opcid_Bad_MaxConnectionsReached = 0x80B70000;
const uint32_t opcid_Bad_RequestTooLarge = 0x80B80000;
const uint32_t opcid_Bad_ResponseTooLarge = 0x80B90000;
const uint32_t opcid_Good_ResultsMayBeIncomplete = 0x00BA0000;
const uint32_t opcid_Bad_EventNotAcknowledgeable = 0x80BB0000;
const uint32_t opcid_Uncertain_ReferenceNotDeleted = 0x40BC0000;
const uint32_t opcid_Bad_InvalidTimestampArgument = 0x80BD0000;
const uint32_t opcid_Bad_ProtocolVersionUnsupported = 0x80BE0000;
const uint32_t opcid_Bad_StateNotActive = 0x80BF0000;
const uint32_t opcid_Uncertain_NotAllNodesAvailable = 0x40C00000;
const uint32_t opcid_Bad_FilterOperatorInvalid = 0x80C10000;
const uint32_t opcid_Bad_FilterOperatorUnsupported = 0x80C20000;
const uint32_t opcid_Bad_FilterOperandCountMismatch = 0x80C30000;
const uint32_t opcid_Bad_FilterElementInvalid = 0x80C40000;
const uint32_t opcid_Bad_FilterLiteralInvalid = 0x80C50000;
const uint32_t opcid_Bad_IdentityChangeNotSupported = 0x80C60000;
const uint32_t opcid_Bad_NotTypeDefinition = 0x80C80000;
const uint32_t opcid_Bad_ViewTimestampInvalid = 0x80C90000;
const uint32_t opcid_Bad_ViewParameterMismatch = 0x80CA0000;
const uint32_t opcid_Bad_ViewVersionInvalid = 0x80CB0000;
const uint32_t opcid_Bad_ConditionAlreadyEnabled = 0x80CC0000;
const uint32_t opcid_Bad_DialogNotActive = 0x80CD0000;
const uint32_t opcid_Bad_DialogResponseInvalid = 0x80CE0000;
const uint32_t opcid_Bad_ConditionBranchAlreadyAcked = 0x80CF0000;
const uint32_t opcid_Bad_ConditionBranchAlreadyConfirmed = 0x80D00000;
const uint32_t opcid_Bad_ConditionAlreadyShelved = 0x80D10000;
const uint32_t opcid_Bad_ConditionNotShelved = 0x80D20000;
const uint32_t opcid_Bad_ShelvingTimeOutOfRange = 0x80D30000;




} //namespace ids 
} //namespace opcua 
} //namespace protocols 



#endif
