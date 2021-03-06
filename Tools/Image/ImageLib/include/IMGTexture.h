#if !defined(__IZANAGI_IMAGE_LIB_IMG_TEXTURE_H__)
#define __IZANAGI_IMAGE_LIB_IMG_TEXTURE_H__

#include <vector>
#include "izDefs.h"
#include "izGraph.h"
#include "izStd.h"
#include "izSceneGraph.h"
#include "izMath.h"
#include "IMGImage.h"
#include "PixelFormatConverter.h"

namespace izanagi {
    class IInputStream;
    class IOutputStream;

namespace tool {
    /**
    */
    class CIMGTexture {
        friend class CIMGMaster;

    public:
        /** 空のテクスチャを作成する
         */
        static CIMGTexture* CreateEmptyTexture(graph::E_GRAPH_TEX_TYPE type);

        /** テクスチャを削除する
         */
        static void Delete(CIMGTexture* tex);

    private:
        CIMGTexture(graph::E_GRAPH_TEX_TYPE nType = graph::E_GRAPH_TEX_TYPE_PLANE);
        ~CIMGTexture();

        NO_COPIABLE(CIMGTexture);

    public:
        // 初期化
        IZ_BOOL InitAsPlane(
            IZ_UINT nWidth,
            IZ_UINT nHeight,
            IZ_UINT nMipLevel,
            graph::E_GRAPH_PIXEL_FMT nFmt,
            IZ_BOOL bIsAllocBuffer = IZ_TRUE);
        IZ_BOOL InitAsCube(
            IZ_UINT nWidth,
            IZ_UINT nHeight,
            IZ_UINT nMipLevel,
            graph::E_GRAPH_PIXEL_FMT nFmt,
            IZ_BOOL bIsAllocBuffer = IZ_TRUE);
        IZ_BOOL InitAsVolume(
            IZ_UINT nDepth,
            IZ_UINT nWidth,
            IZ_UINT nHeight,
            IZ_UINT nMipLevel,
            graph::E_GRAPH_PIXEL_FMT nFmt,
            IZ_BOOL bIsAllocBuffer = IZ_TRUE);

    public:
        // クリア
        void Clear();

        // フォーマット変換
        IZ_BOOL ConvertPixelFormat(
            izanagi::E_PLATFORM type,
            graph::E_GRAPH_PIXEL_FMT nFmt);

        // MIPMAP作成
        IZ_BOOL CreateMipMap(IZ_UINT nMipLevel = 1000);

        // 最下位MIPMAP削除
        IZ_BOOL DeleteLawestMipMap();

        // mipmapとして追加.
        // TODO
        // ただし、現在は追加した順にmiplevelに登録される...
        // また、現在はcubemapは未サポート...
        void addImageAsMipmap(CIMGImage* image);

        /** 出力.
         */
        IZ_BOOL Write(IOutputStream* pOut);

        /** Cubeテクスチャの面として追加する.
         */
        IZ_BOOL AddImageAsCubeFace(
            izanagi::graph::E_GRAPH_CUBE_TEX_FACE face,
            std::vector<CIMGImage*>& images);

        /** 読み込み.
         */
        IZ_BOOL Read(IInputStream* pIn);

    public:
        // イメージ数取得
        inline IZ_UINT GetImageNum() const;

        // イメージサイズ取得
        inline IZ_UINT GetImageSize(IZ_UINT nPos, IZ_UINT nLevel) const;

        // 出力サイズ取得
        inline IZ_UINT GetExportSize() const;

        // ピッチサイズ取得
        IZ_UINT GetPitchSize() const
        {
            IZ_UINT bpp = izanagi::graph::CGraphUtil::GetBPP(m_TexInfo.fmt);
            IZ_UINT width = GetWidth();
            return bpp * width;
        }

        // イメージを取得
        inline std::vector<CIMGImage*>& GetImage(IZ_UINT nPos);

        // テクスチャ情報を取得
        inline const S_IMG_TEX_HEADER& GetTexInfo() const;

        // イメージ幅を取得
        IZ_UINT GetWidth() const { return (1 << m_TexInfo.w); }

        // イメージ高さを取得
        IZ_UINT GetHeight() const { return (1 << m_TexInfo.h); }

    public:
        // テクスチャアドレッシングをセット
        inline void SetTexAddressU(IZ_UINT nAddr);
        inline void SetTexAddressV(IZ_UINT nAddr);
        inline void SetTexAddress(
            IZ_UINT nAddrU,
            IZ_UINT nAddrV);

        // テクスチャフィルタをセット
        inline void SetTexMinFilter(IZ_UINT nFilter);
        inline void SetTexMagFilter(IZ_UINT nFilter);
        inline void SetTexMipFilter(IZ_UINT nFilter);
        inline void SetTexFilter(
            IZ_UINT nMinFilter,
            IZ_UINT nMagFilter,
            IZ_UINT nMipFilter);

    private:
        // テクスチャ情報をセット
        inline void SetTexInfo(
            graph::E_GRAPH_TEX_TYPE type,
            IZ_UINT nWidth,
            IZ_UINT nHeight,
            graph::E_GRAPH_PIXEL_FMT nFmt,
            IZ_UINT nLevel,
            IZ_UINT nDepth);

    private:
        CKey m_Key;

        std::vector< std::vector<CIMGImage*> > m_Images;

        S_IMG_TEX_HEADER m_TexInfo;
    };

    // inline ********************************

    /**
    * イメージ数取得
    */
    IZ_UINT CIMGTexture::GetImageNum() const
    {
        IZ_UINT ret = static_cast<IZ_UINT>(m_Images.size());
        return ret;
    }

    /**
    * イメージサイズ取得
    */
    IZ_UINT CIMGTexture::GetImageSize(IZ_UINT nPos, IZ_UINT nLevel) const
    {
        IZ_ASSERT(nPos < GetImageNum());

        std::vector< std::vector<CIMGImage*> >::const_iterator it = m_Images.begin();
        std::advance(it, nPos);

        const std::vector<CIMGImage*>& tImageList = *it;
        IZ_ASSERT(nLevel < tImageList.size());

        IZ_UINT ret = tImageList[nLevel]->GetSize();
        return ret;
    }

    /**
    * 出力サイズ取得
    */
    IZ_UINT CIMGTexture::GetExportSize() const
    {
        IZ_UINT ret = 0;

        IZ_UINT num = GetImageNum();

        std::vector< std::vector<CIMGImage*> >::const_iterator it = m_Images.begin();

        for (; it != m_Images.end(); it++) {
            const std::vector<CIMGImage*>& tImageList = *it;
            IZ_UINT nLevelMax = static_cast<IZ_UINT>(tImageList.size());

            for (IZ_UINT nLevel = 0; nLevel < nLevelMax; nLevel++) {
                CIMGImage* pImage = tImageList[nLevel];
                ret += pImage->GetSize();
            }
        }

        return ret;
    }

    /**
    * イメージを取得
    */
    std::vector<CIMGImage*>& CIMGTexture::GetImage(IZ_UINT nPos)
    {
        IZ_ASSERT(nPos < GetImageNum());
        return m_Images[nPos];
    }

    /**
    * テクスチャ情報を取得
    */
    const S_IMG_TEX_HEADER& CIMGTexture::GetTexInfo() const
    {
        return m_TexInfo;
    }

    /**
    * テクスチャアドレッシングをセット
    */
    void CIMGTexture::SetTexAddressU(IZ_UINT nAddr)
    {
        m_TexInfo.addressU = nAddr;
    }

    /**
    * テクスチャアドレッシングをセット
    */
    void CIMGTexture::SetTexAddressV(IZ_UINT nAddr)
    {
        m_TexInfo.addressV = nAddr;
    }

    /**
    * テクスチャアドレッシングをセット
    */
    void CIMGTexture::SetTexAddress(
        IZ_UINT nAddrU,
        IZ_UINT nAddrV)
    {
        SetTexAddressU(nAddrU);
        SetTexAddressV(nAddrV);
    }

    /**
    * テクスチャフィルタをセット
    */
    void CIMGTexture::SetTexMinFilter(IZ_UINT nFilter)
    {
        m_TexInfo.minFilter = nFilter;
    }

    /**
    * テクスチャフィルタをセット
    */
    void CIMGTexture::SetTexMagFilter(IZ_UINT nFilter)
    {
        m_TexInfo.magFilter = nFilter;
    }

    /**
    * テクスチャフィルタをセット
    */
    void CIMGTexture::SetTexMipFilter(IZ_UINT nFilter)
    {
        m_TexInfo.mipFilter = nFilter;
    }

    /**
    * テクスチャフィルタをセット
    */
    void CIMGTexture::SetTexFilter(
        IZ_UINT nMinFilter,
        IZ_UINT nMagFilter,
        IZ_UINT nMipFilter)
    {
        SetTexMinFilter(nMinFilter);
        SetTexMagFilter(nMagFilter);
        SetTexMipFilter(nMipFilter);
    }

    // テクスチャ情報をセット
    void CIMGTexture::SetTexInfo(
        graph::E_GRAPH_TEX_TYPE type,
        IZ_UINT nWidth,
        IZ_UINT nHeight,
        graph::E_GRAPH_PIXEL_FMT nFmt,
        IZ_UINT nLevel,
        IZ_UINT nDepth)
    {
        m_TexInfo.type = type;
        m_TexInfo.fmt = nFmt;
        m_TexInfo.level = nLevel;
        m_TexInfo.depth = nDepth;

#if 0
        m_TexInfo.w = ComputeExponentBy2(static_cast<IZ_FLOAT>(nWidth));
        m_TexInfo.h = ComputeExponentBy2(static_cast<IZ_FLOAT>(nHeight));
#else
        m_TexInfo.w = math::CMath::ComputeNextLog2(nWidth);
        m_TexInfo.h = math::CMath::ComputeNextLog2(nHeight);
#endif

        m_TexInfo.size = CPixelFormatConverter::GetInstance()->ComputeByteSize(
                            nWidth,
                            nHeight,
                            nFmt);
    }
}   // namespace tool
}   // namespace izanagi

#endif  // #if !defined(__IZANAGI_IMAGE_LIB_IMG_TEXTURE_H__)
