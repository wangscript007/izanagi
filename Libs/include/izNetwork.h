#if !defined(__IZANAGI_NETWORK_H__)
#define __IZANAGI_NETWORK_H__

#include "network/NetworkDefs.h"
#include "network/Network.h"
#include "network/IPv4Endpoint.h"
#include "network/NetworkPacket.h"
//#include "network/NetworkTCP.h"
#include "network/NetworkTCP_libuv.h"
//#include "network/NetworkUDP.h"
#include "network/NetworkUDP_libuv.h"

// RUDP
#include "network/rudp//ReliableUDP.h"
#include "network/rudp//ReliableUDPListener.h"
#include "network/rudp//RUDPParameter.h"

// ReplicatedProperty
#include "network/ReplicatedProperty/ReplicatedProperty.h"
#include "network/ReplicatedProperty/ReplicatedPropertyObject.h"
#include "network/ReplicatedProperty/ReplicatedPropertyManager.h"

#endif  // #if !defined(__IZANAGI_NETWORK_H__)
