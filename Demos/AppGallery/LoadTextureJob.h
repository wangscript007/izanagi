#if !defined(__LOAD_TEXTURE_JOB_H__)
#define __LOAD_TEXTURE_JOB_H__

#include "izSampleKit.h"
#include "izThreadModel.h"

class Item;

/** Manage loading texture jobs.
 */
class TextureLoader {
    friend class LoadTextureJob;

private:
    static TextureLoader s_Instance;

public:
    static TextureLoader& Instance();

private:
    TextureLoader();
    ~TextureLoader();

public:
    /** Initialize manager.
     */
    void Init(izanagi::IMemoryAllocator* allocator);

    /** Terminage manager.
     */
    void Terminate();

    /** Enqueue loading request.
     */
    izanagi::threadmodel::CJob* EnqueueLoadingRequest(
        izanagi::graph::CGraphicsDevice* device,
        const char* path,
        Item* target);

private:
    izanagi::IMemoryAllocator* m_Allocator;
    izanagi::threadmodel::CJobQueue m_JobQueue;

    // Thread safe allocator for jobs.
    izanagi::threadmodel::CTheadSafeAllocator<izanagi::CStandardMemoryAllocator> m_JobSafeAllocator;
};

/** Job to load texture on worker thread.
 */
class LoadTextureJob : public izanagi::threadmodel::CJob {
    friend class TextureLoader;

public:
    LoadTextureJob();
    virtual ~LoadTextureJob();

private:
    /** Initialize job.
     */
    IZ_BOOL Init(
        izanagi::IMemoryAllocator* allocator,
        izanagi::graph::CGraphicsDevice* device,
        const char* path,
        Item* target);

    /** Run job on worker thread.
     */
    virtual IZ_BOOL OnRun();

    /** Finish job on main thread.
     */
    virtual void OnFinish(IZ_BOOL runResult);

    /** Cancel job.
     */
    virtual void OnCancel();

private:
    izanagi::IMemoryAllocator* m_InternalAllocator;
    izanagi::graph::CGraphicsDevice* m_Device;

    IZ_UINT8* m_PixelData;  //< rgba pixel data.
    IZ_UINT m_TexWidth;     //< image width.
    IZ_UINT m_TexHeight;    //< image height.

    const char* m_Path;         //< image file path.

    Item* m_TargetItem;
};

#endif    // #if !defined(__LOAD_TEXTURE_JOB_H__)