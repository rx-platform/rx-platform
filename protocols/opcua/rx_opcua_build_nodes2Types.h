
/****************************************************************************
* This file is automatically generated by <opcgen-nodes> tool
*
* Generated at:6/29/2022 10:02 PM
*
****************************************************************************/

namespace protocols {
namespace opcua {
namespace opcua_addr_space {

struct opc_KeyValuePair  {
	qualified_name Key;
};
struct opc_AdditionalParametersType  {
	opc_KeyValuePair Parameters;
};
struct opc_EphemeralKeyType  {
	byte_string PublicKey;
	byte_string Signature;
};
struct opc_EndpointType  {
	string_type EndpointUrl;
	opc_MessageSecurityMode SecurityMode;
	string_type SecurityPolicyUri;
	string_type TransportProfileUri;
};
struct opc_RationalNumber  {
	int32_t Numerator;
	uint32_t Denominator;
};
struct opc_ThreeDVector  {
	double X;
	double Y;
	double Z;
};
struct opc_ThreeDCartesianCoordinates  {
	double X;
	double Y;
	double Z;
};
struct opc_ThreeDOrientation  {
	double A;
	double B;
	double C;
};
struct opc_ThreeDFrame  {
	opc_ThreeDCartesianCoordinates CartesianCoordinates;
	opc_ThreeDOrientation Orientation;
};
enum opc_OpenFileMode : int {

	Read = 1,
	Write = 2,
	EraseExisting = 4,
	Append = 8
};
enum opc_IdentityCriteriaType : int {

	UserName = 1,
	Thumbprint = 2,
	Role = 3,
	GroupId = 4,
	Anonymous = 5,
	AuthenticatedUser = 6,
	Application = 7,
	X509Subject = 8
};
struct opc_IdentityMappingRuleType  {
	opc_IdentityCriteriaType CriteriaType;
	string_type Criteria;
};
struct opc_CurrencyUnitType  {
	int16_t NumericCode;
	int8_t Exponent;
	string_type AlphabeticCode;
	localized_text Currency;
};
enum opc_TrustListMasks : int {

	None = 0,
	TrustedCertificates = 1,
	TrustedCrls = 2,
	IssuerCertificates = 4,
	IssuerCrls = 8,
	All = 15
};
struct opc_TrustListDataType  {
	uint32_t SpecifiedLists;
	byte_string TrustedCertificates;
	byte_string TrustedCrls;
	byte_string IssuerCertificates;
	byte_string IssuerCrls;
};
struct opc_DataTypeSchemaHeader  {
	string_type Namespaces;
	opc_StructureDescription StructureDataTypes;
	opc_EnumDescription EnumDataTypes;
	opc_SimpleTypeDescription SimpleDataTypes;
};
struct opc_DataTypeDescription  {
	rx_node_id DataTypeId;
	qualified_name Name;
};
struct opc_StructureDescription  {
	opc_StructureDefinition StructureDefinition;
};
struct opc_EnumDescription  {
	opc_EnumDefinition EnumDefinition;
	uint8_t BuiltInType;
};
struct opc_SimpleTypeDescription  {
	rx_node_id BaseDataType;
	uint8_t BuiltInType;
};
struct opc_UABinaryFileDataType  {
	string_type SchemaLocation;
	opc_KeyValuePair FileHeader;
};
struct opc_PortableQualifiedName  {
	string_type NamespaceUri;
	string_type Name;
};
struct opc_PortableNodeId  {
	string_type NamespaceUri;
	rx_node_id Identifier;
};
struct opc_UnsignedRationalNumber  {
	uint32_t Numerator;
	uint32_t Denominator;
};
enum opc_PubSubState : int {

	Disabled = 0,
	Paused = 1,
	Operational = 2,
	Error = 3,
	PreOperational = 4
};
struct opc_DataSetMetaDataType  {
	string_type Name;
	localized_text Description;
	opc_FieldMetaData Fields;
	rx_uuid DataSetClassId;
	opc_ConfigurationVersionDataType ConfigurationVersion;
};
struct opc_FieldMetaData  {
	string_type Name;
	localized_text Description;
	opc_DataSetFieldFlags FieldFlags;
	uint8_t BuiltInType;
	rx_node_id DataType;
	int32_t ValueRank;
	uint32_t ArrayDimensions;
	uint32_t MaxStringLength;
	rx_uuid DataSetFieldId;
	opc_KeyValuePair Properties;
};
enum opc_DataSetFieldFlags : uint16_t {

	PromotedField = 1
};
struct opc_ConfigurationVersionDataType  {
	 MajorVersion;
	 MinorVersion;
};
struct opc_PublishedDataSetDataType  {
	string_type Name;
	string_type DataSetFolder;
	opc_DataSetMetaDataType DataSetMetaData;
	opc_KeyValuePair ExtensionFields;
	opc_PublishedDataSetSourceDataType DataSetSource;
};
struct opc_PublishedVariableDataType  {
	rx_node_id PublishedVariable;
	uint32_t AttributeId;
	double SamplingIntervalHint;
	uint32_t DeadbandType;
	double DeadbandValue;
	 IndexRange;
	qualified_name MetaDataProperties;
};
struct opc_PublishedDataItemsDataType  {
	opc_PublishedVariableDataType PublishedData;
};
struct opc_PublishedEventsDataType  {
	rx_node_id EventNotifier;
	opc_SimpleAttributeOperand SelectedFields;
	opc_ContentFilter Filter;
};
struct opc_PublishedDataSetCustomSourceDataType  {
	bool CyclicDataSet;
};
enum opc_DataSetFieldContentMask : uint32_t {

	StatusCode = 1,
	SourceTimestamp = 2,
	ServerTimestamp = 4,
	SourcePicoSeconds = 8,
	ServerPicoSeconds = 16,
	RawData = 32
};
struct opc_DataSetWriterDataType  {
	string_type Name;
	bool Enabled;
	uint16_t DataSetWriterId;
	opc_DataSetFieldContentMask DataSetFieldContentMask;
	uint32_t KeyFrameCount;
	string_type DataSetName;
	opc_KeyValuePair DataSetWriterProperties;
	opc_DataSetWriterTransportDataType TransportSettings;
	opc_DataSetWriterMessageDataType MessageSettings;
};
struct opc_PubSubGroupDataType  {
	string_type Name;
	bool Enabled;
	opc_MessageSecurityMode SecurityMode;
	string_type SecurityGroupId;
	opc_EndpointDescription SecurityKeyServices;
	uint32_t MaxNetworkMessageSize;
	opc_KeyValuePair GroupProperties;
};
struct opc_WriterGroupDataType  {
	uint16_t WriterGroupId;
	double PublishingInterval;
	double KeepAliveTime;
	uint8_t Priority;
	string_type LocaleIds;
	string_type HeaderLayoutUri;
	opc_WriterGroupTransportDataType TransportSettings;
	opc_WriterGroupMessageDataType MessageSettings;
	opc_DataSetWriterDataType DataSetWriters;
};
struct opc_PubSubConnectionDataType  {
	string_type Name;
	bool Enabled;
	string_type TransportProfileUri;
	opc_NetworkAddressDataType Address;
	opc_KeyValuePair ConnectionProperties;
	opc_ConnectionTransportDataType TransportSettings;
	opc_WriterGroupDataType WriterGroups;
	opc_ReaderGroupDataType ReaderGroups;
};
struct opc_NetworkAddressDataType  {
	string_type NetworkInterface;
};
struct opc_NetworkAddressUrlDataType  {
	string_type Url;
};
struct opc_ReaderGroupDataType  {
	opc_ReaderGroupTransportDataType TransportSettings;
	opc_ReaderGroupMessageDataType MessageSettings;
	opc_DataSetReaderDataType DataSetReaders;
};
struct opc_DataSetReaderDataType  {
	string_type Name;
	bool Enabled;
	uint16_t WriterGroupId;
	uint16_t DataSetWriterId;
	opc_DataSetMetaDataType DataSetMetaData;
	opc_DataSetFieldContentMask DataSetFieldContentMask;
	double MessageReceiveTimeout;
	uint32_t KeyFrameCount;
	string_type HeaderLayoutUri;
	opc_MessageSecurityMode SecurityMode;
	string_type SecurityGroupId;
	opc_EndpointDescription SecurityKeyServices;
	opc_KeyValuePair DataSetReaderProperties;
	opc_DataSetReaderTransportDataType TransportSettings;
	opc_DataSetReaderMessageDataType MessageSettings;
	opc_SubscribedDataSetDataType SubscribedDataSet;
};
struct opc_TargetVariablesDataType  {
	opc_FieldTargetDataType TargetVariables;
};
struct opc_FieldTargetDataType  {
	rx_uuid DataSetFieldId;
	 ReceiverIndexRange;
	rx_node_id TargetNodeId;
	uint32_t AttributeId;
	 WriteIndexRange;
	opc_OverrideValueHandling OverrideValueHandling;
};
enum opc_OverrideValueHandling : int {

	Disabled = 0,
	LastUsableValue = 1,
	OverrideValue = 2
};
struct opc_SubscribedDataSetMirrorDataType  {
	string_type ParentNodeName;
	opc_RolePermissionType RolePermissions;
};
struct opc_PubSubConfigurationDataType  {
	opc_PublishedDataSetDataType PublishedDataSets;
	opc_PubSubConnectionDataType Connections;
	bool Enabled;
};
struct opc_StandaloneSubscribedDataSetRefDataType  {
	string_type DataSetName;
};
struct opc_StandaloneSubscribedDataSetDataType  {
	string_type Name;
	string_type DataSetFolder;
	opc_DataSetMetaDataType DataSetMetaData;
	opc_SubscribedDataSetDataType SubscribedDataSet;
};
struct opc_SecurityGroupDataType  {
	string_type Name;
	string_type SecurityGroupFolder;
	double KeyLifetime;
	string_type SecurityPolicyUri;
	uint32_t MaxFutureKeyCount;
	uint32_t MaxPastKeyCount;
	string_type SecurityGroupId;
	opc_RolePermissionType RolePermissions;
	opc_KeyValuePair GroupProperties;
};
struct opc_PubSubKeyPushTargetDataType  {
	string_type ApplicationUri;
	string_type PushTargetFolder;
	string_type EndpointUrl;
	string_type SecurityPolicyUri;
	opc_UserTokenPolicy UserTokenType;
	uint16_t RequestedKeyCount;
	double RetryInterval;
	opc_KeyValuePair PushTargetProperties;
	string_type SecurityGroups;
};
struct opc_PubSubConfiguration2DataType  {
	opc_StandaloneSubscribedDataSetDataType SubscribedDataSets;
	opc_DataSetMetaDataType DataSetClasses;
	opc_EndpointDescription DefaultSecurityKeyServices;
	opc_SecurityGroupDataType SecurityGroups;
	opc_PubSubKeyPushTargetDataType PubSubKeyPushTargets;
	 ConfigurationVersion;
	opc_KeyValuePair ConfigurationProperties;
};
enum opc_DataSetOrderingType : int {

	Undefined = 0,
	AscendingWriterId = 1,
	AscendingWriterIdSingle = 2
};
enum opc_UadpNetworkMessageContentMask : uint32_t {

	PublisherId = 1,
	GroupHeader = 2,
	WriterGroupId = 4,
	GroupVersion = 8,
	NetworkMessageNumber = 16,
	SequenceNumber = 32,
	PayloadHeader = 64,
	Timestamp = 128,
	PicoSeconds = 256,
	DataSetClassId = 512,
	PromotedFields = 1024
};
struct opc_UadpWriterGroupMessageDataType  {
	 GroupVersion;
	opc_DataSetOrderingType DataSetOrdering;
	opc_UadpNetworkMessageContentMask NetworkMessageContentMask;
	double SamplingOffset;
	double PublishingOffset;
};
enum opc_UadpDataSetMessageContentMask : uint32_t {

	Timestamp = 1,
	PicoSeconds = 2,
	Status = 4,
	MajorVersion = 8,
	MinorVersion = 16,
	SequenceNumber = 32
};
struct opc_UadpDataSetWriterMessageDataType  {
	opc_UadpDataSetMessageContentMask DataSetMessageContentMask;
	uint16_t ConfiguredSize;
	uint16_t NetworkMessageNumber;
	uint16_t DataSetOffset;
};
struct opc_UadpDataSetReaderMessageDataType  {
	 GroupVersion;
	uint16_t NetworkMessageNumber;
	uint16_t DataSetOffset;
	rx_uuid DataSetClassId;
	opc_UadpNetworkMessageContentMask NetworkMessageContentMask;
	opc_UadpDataSetMessageContentMask DataSetMessageContentMask;
	double PublishingInterval;
	double ReceiveOffset;
	double ProcessingOffset;
};
enum opc_JsonNetworkMessageContentMask : uint32_t {

	NetworkMessageHeader = 1,
	DataSetMessageHeader = 2,
	SingleDataSetMessage = 4,
	PublisherId = 8,
	DataSetClassId = 16,
	ReplyTo = 32
};
struct opc_JsonWriterGroupMessageDataType  {
	opc_JsonNetworkMessageContentMask NetworkMessageContentMask;
};
enum opc_JsonDataSetMessageContentMask : uint32_t {

	DataSetWriterId = 1,
	MetaDataVersion = 2,
	SequenceNumber = 4,
	Timestamp = 8,
	Status = 16,
	MessageType = 32,
	DataSetWriterName = 64,
	ReversibleFieldEncoding = 128
};
struct opc_JsonDataSetWriterMessageDataType  {
	opc_JsonDataSetMessageContentMask DataSetMessageContentMask;
};
struct opc_JsonDataSetReaderMessageDataType  {
	opc_JsonNetworkMessageContentMask NetworkMessageContentMask;
	opc_JsonDataSetMessageContentMask DataSetMessageContentMask;
};
struct opc_TransmitQosPriorityDataType  {
	string_type PriorityLabel;
};
struct opc_ReceiveQosPriorityDataType  {
	string_type PriorityLabel;
};
struct opc_DatagramConnectionTransportDataType  {
	opc_NetworkAddressDataType DiscoveryAddress;
};
struct opc_DatagramConnectionTransport2DataType  {
	uint32_t DiscoveryAnnounceRate;
	uint32_t DiscoveryMaxMessageSize;
	string_type QosCategory;
	opc_QosDataType DatagramQos;
};
struct opc_DatagramWriterGroupTransportDataType  {
	uint8_t MessageRepeatCount;
	double MessageRepeatDelay;
};
struct opc_DatagramWriterGroupTransport2DataType  {
	opc_NetworkAddressDataType Address;
	string_type QosCategory;
	opc_TransmitQosDataType DatagramQos;
	uint32_t DiscoveryAnnounceRate;
	string_type Topic;
};
struct opc_DatagramDataSetReaderTransportDataType  {
	opc_NetworkAddressDataType Address;
	string_type QosCategory;
	opc_ReceiveQosDataType DatagramQos;
	string_type Topic;
};
struct opc_BrokerConnectionTransportDataType  {
	string_type ResourceUri;
	string_type AuthenticationProfileUri;
};
enum opc_BrokerTransportQualityOfService : int {

	NotSpecified = 0,
	BestEffort = 1,
	AtLeastOnce = 2,
	AtMostOnce = 3,
	ExactlyOnce = 4
};
struct opc_BrokerWriterGroupTransportDataType  {
	string_type QueueName;
	string_type ResourceUri;
	string_type AuthenticationProfileUri;
	opc_BrokerTransportQualityOfService RequestedDeliveryGuarantee;
};
struct opc_BrokerDataSetWriterTransportDataType  {
	string_type QueueName;
	string_type ResourceUri;
	string_type AuthenticationProfileUri;
	opc_BrokerTransportQualityOfService RequestedDeliveryGuarantee;
	string_type MetaDataQueueName;
	double MetaDataUpdateTime;
};
struct opc_BrokerDataSetReaderTransportDataType  {
	string_type QueueName;
	string_type ResourceUri;
	string_type AuthenticationProfileUri;
	opc_BrokerTransportQualityOfService RequestedDeliveryGuarantee;
	string_type MetaDataQueueName;
};
enum opc_PubSubConfigurationRefMask : uint32_t {

	ElementAdd = 1,
	ElementMatch = 2,
	ElementModify = 4,
	ElementRemove = 8,
	ReferenceWriter = 16,
	ReferenceReader = 32,
	ReferenceWriterGroup = 64,
	ReferenceReaderGroup = 128,
	ReferenceConnection = 256,
	ReferencePubDataset = 512,
	ReferenceSubDataset = 1024,
	ReferenceSecurityGroup = 2048,
	ReferencePushTarget = 4096
};
struct opc_PubSubConfigurationRefDataType  {
	opc_PubSubConfigurationRefMask ConfigurationMask;
	uint16_t ElementIndex;
	uint16_t ConnectionIndex;
	uint16_t GroupIndex;
};
struct opc_PubSubConfigurationValueDataType  {
	opc_PubSubConfigurationRefDataType ConfigurationElement;
	string_type Name;
};
enum opc_DiagnosticsLevel : int {

	Basic = 0,
	Advanced = 1,
	Info = 2,
	Log = 3,
	Debug = 4
};
enum opc_PubSubDiagnosticsCounterClassification : int {

	Information = 0,
	Error = 1
};
struct opc_AliasNameDataType  {
	qualified_name AliasName;
	rx_node_id ReferencedNodes;
};
enum opc_PasswordOptionsMask : uint32_t {

	SupportInitialPasswordChange = 1,
	SupportDisableUser = 2,
	SupportDisableDeleteForUser = 4,
	SupportNoChangeForUser = 8,
	SupportDescriptionForUser = 16,
	RequiresUpperCaseCharacters = 32,
	RequiresLowerCaseCharacters = 64,
	RequiresDigitCharacters = 128,
	RequiresSpecialCharacters = 256
};
enum opc_UserConfigurationMask : uint32_t {

	NoDelete = 1,
	Disabled = 2,
	NoChangeByUser = 4,
	MustChangePassword = 8
};
struct opc_UserManagementDataType  {
	string_type UserName;
	opc_UserConfigurationMask UserConfiguration;
	string_type Description;
};
enum opc_Duplex : int {

	Full = 0,
	Half = 1,
	Unknown = 2
};
enum opc_InterfaceAdminStatus : int {

	Up = 0,
	Down = 1,
	Testing = 2
};
enum opc_InterfaceOperStatus : int {

	Up = 0,
	Down = 1,
	Testing = 2,
	Unknown = 3,
	Dormant = 4,
	NotPresent = 5,
	LowerLayerDown = 6
};
enum opc_NegotiationStatus : int {

	InProgress = 0,
	Complete = 1,
	Failed = 2,
	Unknown = 3,
	NoNegotiation = 4
};
enum opc_TsnFailureCode : int {

	NoFailure = 0,
	InsufficientBandwidth = 1,
	InsufficientResources = 2,
	InsufficientTrafficClassBandwidth = 3,
	StreamIdInUse = 4,
	StreamDestinationAddressInUse = 5,
	StreamPreemptedByHigherRank = 6,
	LatencyHasChanged = 7,
	EgressPortNotAvbCapable = 8,
	UseDifferentDestinationAddress = 9,
	OutOfMsrpResources = 10,
	OutOfMmrpResources = 11,
	CannotStoreDestinationAddress = 12,
	PriorityIsNotAnSrcClass = 13,
	MaxFrameSizeTooLarge = 14,
	MaxFanInPortsLimitReached = 15,
	FirstValueChangedForStreamId = 16,
	VlanBlockedOnEgress = 17,
	VlanTaggingDisabledOnEgress = 18,
	SrClassPriorityMismatch = 19,
	FeatureNotPropagated = 20,
	MaxLatencyExceeded = 21,
	BridgeDoesNotProvideNetworkId = 22,
	StreamTransformNotSupported = 23,
	StreamIdTypeNotSupported = 24,
	FeatureNotSupported = 25
};
enum opc_TsnStreamState : int {

	Disabled = 0,
	Configuring = 1,
	Ready = 2,
	Operational = 3,
	Error = 4
};
enum opc_TsnTalkerStatus : int {

	None = 0,
	Ready = 1,
	Failed = 2
};
enum opc_TsnListenerStatus : int {

	None = 0,
	Ready = 1,
	PartialFailed = 2,
	Failed = 3
};
struct opc_PriorityMappingEntryType  {
	string_type MappingUri;
	string_type PriorityLabel;
	uint8_t PriorityValue_PCP;
	uint32_t PriorityValue_DSCP;
};
enum opc_IdType : int {

	Numeric = 0,
	String = 1,
	Guid = 2,
	Opaque = 3
};
enum opc_NodeClass : int {

	Unspecified = 0,
	Object = 1,
	Variable = 2,
	Method = 4,
	ObjectType = 8,
	VariableType = 16,
	ReferenceType = 32,
	DataType = 64,
	View = 128
};
enum opc_PermissionType : uint32_t {

	Browse = 1,
	ReadRolePermissions = 2,
	WriteAttribute = 4,
	WriteRolePermissions = 8,
	WriteHistorizing = 16,
	Read = 32,
	Write = 64,
	ReadHistory = 128,
	InsertHistory = 256,
	ModifyHistory = 512,
	DeleteHistory = 1024,
	ReceiveEvents = 2048,
	Call = 4096,
	AddReference = 8192,
	RemoveReference = 16384,
	DeleteNode = 32768,
	AddNode = 65536
};
enum opc_AccessLevelType : uint8_t {

	CurrentRead = 1,
	CurrentWrite = 2,
	HistoryRead = 4,
	HistoryWrite = 8,
	SemanticChange = 16,
	StatusWrite = 32,
	TimestampWrite = 64
};
enum opc_AccessLevelExType : uint32_t {

	CurrentRead = 1,
	CurrentWrite = 2,
	HistoryRead = 4,
	HistoryWrite = 8,
	SemanticChange = 16,
	StatusWrite = 32,
	TimestampWrite = 64,
	NonatomicRead = 256,
	NonatomicWrite = 512,
	WriteFullArrayOnly = 1024,
	NoSubDataTypes = 2048,
	NonVolatile = 4096,
	Constant = 8192
};
enum opc_EventNotifierType : uint8_t {

	SubscribeToEvents = 1,
	HistoryRead = 4,
	HistoryWrite = 8
};
enum opc_AccessRestrictionType : uint16_t {

	SigningRequired = 1,
	EncryptionRequired = 2,
	SessionRequired = 4,
	ApplyRestrictionsToBrowse = 8
};
struct opc_RolePermissionType  {
	rx_node_id RoleId;
	opc_PermissionType Permissions;
};
enum opc_StructureType : int {

	Structure = 0,
	StructureWithOptionalFields = 1,
	Union = 2,
	StructureWithSubtypedValues = 3,
	UnionWithSubtypedValues = 4
};
struct opc_StructureField  {
	string_type Name;
	localized_text Description;
	rx_node_id DataType;
	int32_t ValueRank;
	uint32_t ArrayDimensions;
	uint32_t MaxStringLength;
	bool IsOptional;
};
struct opc_StructureDefinition  {
	rx_node_id DefaultEncodingId;
	rx_node_id BaseDataType;
	opc_StructureType StructureType;
	opc_StructureField Fields;
};
struct opc_EnumDefinition  {
	opc_EnumField Fields;
};
struct opc_Argument  {
	string_type Name;
	rx_node_id DataType;
	int32_t ValueRank;
	uint32_t ArrayDimensions;
	localized_text Description;
};
struct opc_EnumValueType  {
	int64_t Value;
	localized_text DisplayName;
	localized_text Description;
};
struct opc_EnumField  {
	string_type Name;
};
struct opc_OptionSet  {
	byte_string Value;
	byte_string ValidBits;
};
struct opc_TimeZoneDataType  {
	int16_t Offset;
	bool DaylightSavingInOffset;
};
enum opc_ApplicationType : int {

	Server = 0,
	Client = 1,
	ClientAndServer = 2,
	DiscoveryServer = 3
};
struct opc_ApplicationDescription  {
	string_type ApplicationUri;
	string_type ProductUri;
	localized_text ApplicationName;
	opc_ApplicationType ApplicationType;
	string_type GatewayServerUri;
	string_type DiscoveryProfileUri;
	string_type DiscoveryUrls;
};
struct opc_ServerOnNetwork  {
	uint32_t RecordId;
	string_type ServerName;
	string_type DiscoveryUrl;
	string_type ServerCapabilities;
};
enum opc_MessageSecurityMode : int {

	Invalid = 0,
	None = 1,
	Sign = 2,
	SignAndEncrypt = 3
};
enum opc_UserTokenType : int {

	Anonymous = 0,
	UserName = 1,
	Certificate = 2,
	IssuedToken = 3
};
struct opc_UserTokenPolicy  {
	string_type PolicyId;
	opc_UserTokenType TokenType;
	string_type IssuedTokenType;
	string_type IssuerEndpointUrl;
	string_type SecurityPolicyUri;
};
struct opc_EndpointDescription  {
	string_type EndpointUrl;
	opc_ApplicationDescription Server;
	 ServerCertificate;
	opc_MessageSecurityMode SecurityMode;
	string_type SecurityPolicyUri;
	opc_UserTokenPolicy UserIdentityTokens;
	string_type TransportProfileUri;
	uint8_t SecurityLevel;
};
struct opc_RegisteredServer  {
	string_type ServerUri;
	string_type ProductUri;
	localized_text ServerNames;
	opc_ApplicationType ServerType;
	string_type GatewayServerUri;
	string_type DiscoveryUrls;
	string_type SemaphoreFilePath;
	bool IsOnline;
};
struct opc_MdnsDiscoveryConfiguration  {
	string_type MdnsServerName;
	string_type ServerCapabilities;
};
enum opc_SecurityTokenRequestType : int {

	Issue = 0,
	Renew = 1
};
struct opc_SignedSoftwareCertificate  {
	byte_string CertificateData;
	byte_string Signature;
};
struct opc_UserIdentityToken  {
	string_type PolicyId;
};
struct opc_UserNameIdentityToken  {
	string_type UserName;
	byte_string Password;
	string_type EncryptionAlgorithm;
};
struct opc_X509IdentityToken  {
	byte_string CertificateData;
};
struct opc_IssuedIdentityToken  {
	byte_string TokenData;
	string_type EncryptionAlgorithm;
};
enum opc_NodeAttributesMask : int {

	None = 0,
	AccessLevel = 1,
	ArrayDimensions = 2,
	BrowseName = 4,
	ContainsNoLoops = 8,
	DataType = 16,
	Description = 32,
	DisplayName = 64,
	EventNotifier = 128,
	Executable = 256,
	Historizing = 512,
	InverseName = 1024,
	IsAbstract = 2048,
	MinimumSamplingInterval = 4096,
	NodeClass = 8192,
	NodeId = 16384,
	Symmetric = 32768,
	UserAccessLevel = 65536,
	UserExecutable = 131072,
	UserWriteMask = 262144,
	ValueRank = 524288,
	WriteMask = 1048576,
	Value = 2097152,
	DataTypeDefinition = 4194304,
	RolePermissions = 8388608,
	AccessRestrictions = 16777216,
	All = 33554431,
	BaseNode = 26501220,
	Object = 26501348,
	ObjectType = 26503268,
	Variable = 26571383,
	VariableType = 28600438,
	Method = 26632548,
	ReferenceType = 26537060,
	View = 26501356
};
struct opc_AddNodesItem  {
	rx_node_id ParentNodeId;
	rx_node_id ReferenceTypeId;
	rx_node_id RequestedNewNodeId;
	qualified_name BrowseName;
	opc_NodeClass NodeClass;
	localized_text NodeAttributes;
	rx_node_id TypeDefinition;
};
struct opc_AddReferencesItem  {
	rx_node_id SourceNodeId;
	rx_node_id ReferenceTypeId;
	bool IsForward;
	string_type TargetServerUri;
	rx_node_id TargetNodeId;
	opc_NodeClass TargetNodeClass;
};
struct opc_DeleteNodesItem  {
	rx_node_id NodeId;
	bool DeleteTargetReferences;
};
struct opc_DeleteReferencesItem  {
	rx_node_id SourceNodeId;
	rx_node_id ReferenceTypeId;
	bool IsForward;
	rx_node_id TargetNodeId;
	bool DeleteBidirectional;
};
enum opc_AttributeWriteMask : uint32_t {

	AccessLevel = 1,
	ArrayDimensions = 2,
	BrowseName = 4,
	ContainsNoLoops = 8,
	DataType = 16,
	Description = 32,
	DisplayName = 64,
	EventNotifier = 128,
	Executable = 256,
	Historizing = 512,
	InverseName = 1024,
	IsAbstract = 2048,
	MinimumSamplingInterval = 4096,
	NodeClass = 8192,
	NodeId = 16384,
	Symmetric = 32768,
	UserAccessLevel = 65536,
	UserExecutable = 131072,
	UserWriteMask = 262144,
	ValueRank = 524288,
	WriteMask = 1048576,
	ValueForVariableType = 2097152,
	DataTypeDefinition = 4194304,
	RolePermissions = 8388608,
	AccessRestrictions = 16777216,
	AccessLevelEx = 33554432
};
struct opc_RelativePathElement  {
	rx_node_id ReferenceTypeId;
	bool IsInverse;
	bool IncludeSubtypes;
	qualified_name TargetName;
};
struct opc_RelativePath  {
	opc_RelativePathElement Elements;
};
struct opc_EndpointConfiguration  {
	int32_t OperationTimeout;
	bool UseBinaryEncoding;
	int32_t MaxStringLength;
	int32_t MaxByteStringLength;
	int32_t MaxArrayLength;
	int32_t MaxMessageSize;
	int32_t MaxBufferSize;
	int32_t ChannelLifetime;
	int32_t SecurityTokenLifetime;
};
enum opc_FilterOperator : int {

	Equals = 0,
	IsNull = 1,
	GreaterThan = 2,
	LessThan = 3,
	GreaterThanOrEqual = 4,
	LessThanOrEqual = 5,
	Like = 6,
	Not = 7,
	Between = 8,
	InList = 9,
	And = 10,
	Or = 11,
	Cast = 12,
	InView = 13,
	OfType = 14,
	RelatedTo = 15,
	BitwiseAnd = 16,
	BitwiseOr = 17
};
struct opc_ContentFilterElement  {
	opc_FilterOperator FilterOperator;
	localized_text FilterOperands;
};
struct opc_ContentFilter  {
	opc_ContentFilterElement Elements;
};
struct opc_ElementOperand  {
	uint32_t Index;
};
struct opc_AttributeOperand  {
	rx_node_id NodeId;
	string_type Alias;
	opc_RelativePath BrowsePath;
	uint32_t AttributeId;
	 IndexRange;
};
struct opc_SimpleAttributeOperand  {
	rx_node_id TypeDefinitionId;
	qualified_name BrowsePath;
	uint32_t AttributeId;
	 IndexRange;
};
struct opc_HistoryEvent  {
	opc_HistoryEventFieldList Events;
};
enum opc_HistoryUpdateType : int {

	Insert = 1,
	Replace = 2,
	Update = 3,
	Delete = 4
};
enum opc_PerformUpdateType : int {

	Insert = 1,
	Replace = 2,
	Update = 3,
	Remove = 4
};
struct opc_EventFilter  {
	opc_SimpleAttributeOperand SelectClauses;
	opc_ContentFilter WhereClause;
};
struct opc_AggregateConfiguration  {
	bool UseServerCapabilitiesDefaults;
	bool TreatUncertainAsBad;
	uint8_t PercentDataBad;
	uint8_t PercentDataGood;
	bool UseSlopedExtrapolation;
};
struct opc_BuildInfo  {
	string_type ProductUri;
	string_type ManufacturerName;
	string_type ProductName;
	string_type SoftwareVersion;
	string_type BuildNumber;
	rx_time BuildDate;
};
enum opc_RedundancySupport : int {

	None = 0,
	Cold = 1,
	Warm = 2,
	Hot = 3,
	Transparent = 4,
	HotAndMirrored = 5
};
enum opc_ServerState : int {

	Running = 0,
	Failed = 1,
	NoConfiguration = 2,
	Suspended = 3,
	Shutdown = 4,
	Test = 5,
	CommunicationFault = 6,
	Unknown = 7
};
struct opc_RedundantServerDataType  {
	string_type ServerId;
	uint8_t ServiceLevel;
	opc_ServerState ServerState;
};
struct opc_EndpointUrlListDataType  {
	string_type EndpointUrlList;
};
struct opc_NetworkGroupDataType  {
	string_type ServerUri;
	opc_EndpointUrlListDataType NetworkPaths;
};
struct opc_SamplingIntervalDiagnosticsDataType  {
	double SamplingInterval;
	uint32_t MonitoredItemCount;
	uint32_t MaxMonitoredItemCount;
	uint32_t DisabledMonitoredItemCount;
};
struct opc_ServerDiagnosticsSummaryDataType  {
	uint32_t ServerViewCount;
	uint32_t CurrentSessionCount;
	uint32_t CumulatedSessionCount;
	uint32_t SecurityRejectedSessionCount;
	uint32_t RejectedSessionCount;
	uint32_t SessionTimeoutCount;
	uint32_t SessionAbortCount;
	uint32_t CurrentSubscriptionCount;
	uint32_t CumulatedSubscriptionCount;
	uint32_t PublishingIntervalCount;
	uint32_t SecurityRejectedRequestsCount;
	uint32_t RejectedRequestsCount;
};
struct opc_ServerStatusDataType  {
	rx_time StartTime;
	rx_time CurrentTime;
	opc_ServerState State;
	opc_BuildInfo BuildInfo;
	uint32_t SecondsTillShutdown;
	localized_text ShutdownReason;
};
struct opc_SessionDiagnosticsDataType  {
	rx_node_id SessionId;
	string_type SessionName;
	opc_ApplicationDescription ClientDescription;
	string_type ServerUri;
	string_type EndpointUrl;
	string_type LocaleIds;
	double ActualSessionTimeout;
	uint32_t MaxResponseMessageSize;
	rx_time ClientConnectionTime;
	rx_time ClientLastContactTime;
	uint32_t CurrentSubscriptionsCount;
	uint32_t CurrentMonitoredItemsCount;
	uint32_t CurrentPublishRequestsInQueue;
	opc_ServiceCounterDataType TotalRequestCount;
	uint32_t UnauthorizedRequestCount;
	opc_ServiceCounterDataType ReadCount;
	opc_ServiceCounterDataType HistoryReadCount;
	opc_ServiceCounterDataType WriteCount;
	opc_ServiceCounterDataType HistoryUpdateCount;
	opc_ServiceCounterDataType CallCount;
	opc_ServiceCounterDataType CreateMonitoredItemsCount;
	opc_ServiceCounterDataType ModifyMonitoredItemsCount;
	opc_ServiceCounterDataType SetMonitoringModeCount;
	opc_ServiceCounterDataType SetTriggeringCount;
	opc_ServiceCounterDataType DeleteMonitoredItemsCount;
	opc_ServiceCounterDataType CreateSubscriptionCount;
	opc_ServiceCounterDataType ModifySubscriptionCount;
	opc_ServiceCounterDataType SetPublishingModeCount;
	opc_ServiceCounterDataType PublishCount;
	opc_ServiceCounterDataType RepublishCount;
	opc_ServiceCounterDataType TransferSubscriptionsCount;
	opc_ServiceCounterDataType DeleteSubscriptionsCount;
	opc_ServiceCounterDataType AddNodesCount;
	opc_ServiceCounterDataType AddReferencesCount;
	opc_ServiceCounterDataType DeleteNodesCount;
	opc_ServiceCounterDataType DeleteReferencesCount;
	opc_ServiceCounterDataType BrowseCount;
	opc_ServiceCounterDataType BrowseNextCount;
	opc_ServiceCounterDataType TranslateBrowsePathsToNodeIdsCount;
	opc_ServiceCounterDataType QueryFirstCount;
	opc_ServiceCounterDataType QueryNextCount;
	opc_ServiceCounterDataType RegisterNodesCount;
	opc_ServiceCounterDataType UnregisterNodesCount;
};
struct opc_SessionSecurityDiagnosticsDataType  {
	rx_node_id SessionId;
	string_type ClientUserIdOfSession;
	string_type ClientUserIdHistory;
	string_type AuthenticationMechanism;
	string_type Encoding;
	string_type TransportProtocol;
	opc_MessageSecurityMode SecurityMode;
	string_type SecurityPolicyUri;
	byte_string ClientCertificate;
};
struct opc_ServiceCounterDataType  {
	uint32_t TotalCount;
	uint32_t ErrorCount;
};
struct opc_StatusResult  {
	uint32_t StatusCode;
	diagnostics_info DiagnosticInfo;
};
struct opc_SubscriptionDiagnosticsDataType  {
	rx_node_id SessionId;
	uint32_t SubscriptionId;
	uint8_t Priority;
	double PublishingInterval;
	uint32_t MaxKeepAliveCount;
	uint32_t MaxLifetimeCount;
	uint32_t MaxNotificationsPerPublish;
	bool PublishingEnabled;
	uint32_t ModifyCount;
	uint32_t EnableCount;
	uint32_t DisableCount;
	uint32_t RepublishRequestCount;
	uint32_t RepublishMessageRequestCount;
	uint32_t RepublishMessageCount;
	uint32_t TransferRequestCount;
	uint32_t TransferredToAltClientCount;
	uint32_t TransferredToSameClientCount;
	uint32_t PublishRequestCount;
	uint32_t DataChangeNotificationsCount;
	uint32_t EventNotificationsCount;
	uint32_t NotificationsCount;
	uint32_t LatePublishRequestCount;
	uint32_t CurrentKeepAliveCount;
	uint32_t CurrentLifetimeCount;
	uint32_t UnacknowledgedMessageCount;
	uint32_t DiscardedMessageCount;
	uint32_t MonitoredItemCount;
	uint32_t DisabledMonitoredItemCount;
	uint32_t MonitoringQueueOverflowCount;
	uint32_t NextSequenceNumber;
	uint32_t EventQueueOverFlowCount;
};
struct opc_ModelChangeStructureDataType  {
	rx_node_id Affected;
	rx_node_id AffectedType;
	uint8_t Verb;
};
struct opc_SemanticChangeStructureDataType  {
	rx_node_id Affected;
	rx_node_id AffectedType;
};
struct opc_Range  {
	double Low;
	double High;
};
struct opc_EUInformation  {
	string_type NamespaceUri;
	int32_t UnitId;
	localized_text DisplayName;
	localized_text Description;
};
enum opc_AxisScaleEnumeration : int {

	Linear = 0,
	Log = 1,
	Ln = 2
};
struct opc_ComplexNumberType  {
	float Real;
	float Imaginary;
};
struct opc_DoubleComplexNumberType  {
	double Real;
	double Imaginary;
};
struct opc_AxisInformation  {
	opc_EUInformation EngineeringUnits;
	opc_Range EURange;
	localized_text Title;
	opc_AxisScaleEnumeration AxisScaleType;
	double AxisSteps;
};
struct opc_XVType  {
	double X;
	float Value;
};
struct opc_ProgramDiagnosticDataType  {
	rx_node_id CreateSessionId;
	string_type CreateClientName;
	rx_time InvocationCreationTime;
	rx_time LastTransitionTime;
	string_type LastMethodCall;
	rx_node_id LastMethodSessionId;
	opc_Argument LastMethodInputArguments;
	opc_Argument LastMethodOutputArguments;
	rx_time LastMethodCallTime;
	opc_StatusResult LastMethodReturnStatus;
};
struct opc_ProgramDiagnostic2DataType  {
	rx_node_id CreateSessionId;
	string_type CreateClientName;
	rx_time InvocationCreationTime;
	rx_time LastTransitionTime;
	string_type LastMethodCall;
	rx_node_id LastMethodSessionId;
	opc_Argument LastMethodInputArguments;
	opc_Argument LastMethodOutputArguments;
	rx_time LastMethodCallTime;
	uint32_t LastMethodReturnStatus;
};
struct opc_Annotation  {
	string_type Message;
	string_type UserName;
	rx_time AnnotationTime;
};
enum opc_ExceptionDeviationFormat : int {

	AbsoluteValue = 0,
	PercentOfValue = 1,
	PercentOfRange = 2,
	PercentOfEURange = 3,
	Unknown = 4
};

} //namespace protocols
} //namespace opcua
} //namespace opcua_addr_space
