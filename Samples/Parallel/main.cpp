#include "izSampleKit.h"
#include "izThreadModel.h"

static IZ_INT Data[100];

static void ParallelForFunc(IZ_INT idx)
{
    IZ_PRINTF("For Func : %d\n", idx);
    Data[idx] -= 1;
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

static void ParallelForEachFunc(void* data)
{
    IZ_UINT value = *(IZ_UINT*)data;
    IZ_PRINTF("ForEach Func : %d\n", value);
    Data[value] -= 1;
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

IZ_UINT8 buffer[1 * 1024 * 1024];

IzMain(0, 0)
{
    izanagi::CStandardMemoryAllocator allocator;
    allocator.Init(sizeof(buffer), buffer);

	izanagi::threadmodel::CThreadPool theadPool;
	theadPool.Init(&allocator, 4);

    {
        for (IZ_UINT i = 0; i < COUNTOF(Data); i++) {
            Data[i] = i;
        }

        izanagi::threadmodel::CParallelFor forTasks[10];

        izanagi::threadmodel::CParallel::For(
			theadPool,
            //&allocator,
            forTasks,
            0, 100,
            ParallelForFunc);

        izanagi::threadmodel::CParallel::waitFor(forTasks, COUNTOF(forTasks));
    
        for (IZ_INT i = 0; i < COUNTOF(Data); i++) {
            if (Data[i] != i - 1) {
                IZ_PRINTF("Failed Parallel For Func!!!\n");
                IZ_ASSERT(IZ_FALSE);
            }
        }
    }

    {
        for (IZ_UINT i = 0; i < COUNTOF(Data); i++) {
            Data[i] = i;
        }

        izanagi::threadmodel::CParallelForEach forEachTasks[10];

        izanagi::threadmodel::CParallel::ForEach(
			theadPool,
            //&allocator,
            forEachTasks,
            Data, sizeof(IZ_UINT),
            COUNTOF(Data),
            ParallelForEachFunc);

        izanagi::threadmodel::CParallel::waitForEach(forEachTasks, COUNTOF(forEachTasks));
    
        for (IZ_UINT i = 0; i < COUNTOF(Data); i++) {
            if (Data[i] != i - 1) {
                IZ_PRINTF("Failed Parallel ForEach Func!!!\n");
                IZ_ASSERT(IZ_FALSE);
            }
        }
    }

	theadPool.Terminate();

    allocator.Dump();

    return 0;
}
