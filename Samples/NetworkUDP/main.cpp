#include "izSampleKit.h"
#include "izNetwork.h"

IZ_UINT8 buffer[2 * 1024 * 1024];

static const IZ_CHAR* serverIp = "127.0.0.1";

static const IZ_UINT ServerPort = 30000;

static IZ_BOOL isAsync = IZ_FALSE;

void runAsServer(izanagi::IMemoryAllocator* allocator)
{
    izanagi::net::IPv4Endpoint host(
        izanagi::net::IPv4Address::Any,
        ServerPort);

	izanagi::net::Udp udp;
    udp.start(allocator, host);

    static const IZ_UINT size = 1024;
    IZ_CHAR buf[size] = { 0 };

    izanagi::net::IPv4Endpoint remote;

    while (udp.recieveFrom(buf, size, remote) < 0) {
    }

    IZ_PRINTF("%s\n", buf);

	while (!udp.stop()) {
	}
}

void runAsClient(izanagi::IMemoryAllocator* allocator)
{
    izanagi::net::IPv4Endpoint remote(
        izanagi::net::IPv4Address(serverIp),
        ServerPort);

    izanagi::net::Udp udp;
    udp.start(allocator);
    udp.connectTo(remote);

    static const IZ_CHAR* str = "test";

    udp.sendData(str, strlen(str));

	while (!udp.stop()) {
	}
}

IzMain(0, 0)
{
    IZ_BOOL isServer = IZ_FALSE;

	if (argc > 1 && strcmp(argv[1], "s" ) == 0) {
		isServer = IZ_TRUE;
	}

	izanagi::CStandardMemoryAllocator allocator;
	allocator.Init(sizeof(buffer), buffer);

	izanagi::net::Network::begin();

	if (isServer) {
		runAsServer(&allocator);
	}
	else {
		runAsClient(&allocator);
	}

	izanagi::net::Network::end();

	allocator.Dump();

	return 0;
}
