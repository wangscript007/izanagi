#if !defined(__SEAT_H__)
#define __SEAT_H__

#include "izSampleKit.h"

/** Photos's seat.
 */
class Seat : public izanagi::CObject {
public:
    // Create an instance.
    static Seat* Create(
        izanagi::IMemoryAllocator* allocator,
        izanagi::graph::CGraphicsDevice* device);

private:
    // Flags for vertex semantics.
    static const IZ_UINT MeshFlags = izanagi::E_DEBUG_MESH_VTX_FORM_POS
        | izanagi::E_DEBUG_MESH_VTX_FORM_COLOR
        | izanagi::E_DEBUG_MESH_VTX_FORM_NORMAL;

    // Slices of seat.
    static const IZ_UINT Slices = 20;

    // Color for seat.
    static const IZ_COLOR Color = IZ_COLOR_RGBA(0xff, 0xff, 0xff, 0xff);

    // Raduis.
    static const IZ_FLOAT InnerRadius;
    static const IZ_FLOAT OuterRadius;
    static const IZ_FLOAT MostOuterRadius;

    // Height.
    static const IZ_FLOAT Height;

private:
    Seat();
    virtual ~Seat();

    NO_COPIABLE(Seat);

    IZ_DEFINE_INTERNAL_RELEASE();

private:
    // Initialization.
    IZ_BOOL Init(
        izanagi::IMemoryAllocator* allocator, 
        izanagi::graph::CGraphicsDevice* device);

public:
    /** Render seat.
     */
    void Render(izanagi::graph::CGraphicsDevice* device);

private:
    // Parts of photo's seat.
    class SeatPart : public izanagi::CDebugMesh {
    public:
        static SeatPart* Create(
            izanagi::IMemoryAllocator* allocator,
            izanagi::graph::CGraphicsDevice* device,
            IZ_FLOAT innerRadius, IZ_FLOAT outerRadius,
            IZ_FLOAT height,
            IZ_UINT slices);

    private:
        SeatPart() {}
        virtual ~SeatPart() {}

    private:
        IZ_BOOL Init(
            izanagi::graph::CGraphicsDevice* device,
            IZ_UINT flag,
            IZ_FLOAT innerRadius, IZ_FLOAT outerRadius,
            IZ_FLOAT height,
            IZ_UINT slices);

        // Set vertices.
        IZ_BOOL SetVtx(
            IZ_UINT flag,
            IZ_COLOR color,
            IZ_FLOAT innerRadius, IZ_FLOAT outerRadius,
            IZ_FLOAT height,
            IZ_UINT slices);

        // Compute vertices.
        void ComputeVtx(
            SMeshVtx* vtx,
            IZ_UINT flag,
            IZ_COLOR color,
            IZ_FLOAT radius,
            IZ_FLOAT y,
            IZ_FLOAT longitude);

        // Set indices.
        IZ_BOOL SetIdx(IZ_UINT slices);
    };

private:
    izanagi::IMemoryAllocator* m_Allocator;

    izanagi::CDebugMeshRectangle* m_Floor;
    SeatPart* m_FlontSeat;
    SeatPart* m_RearSeat;
};

#endif    // #if !defined(__SEAT_H__)