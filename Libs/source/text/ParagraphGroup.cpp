#include "text/ParagraphGroup.h"
#include "text/Paragraph.h"
#include "text/FontHost.h"
#include "text/UString.h"
#include "izGraph.h"

namespace izanagi
{
namespace text
{
    CParagraphGroup::CParagraphGroup()
    {
        m_Width = 0;
        m_Height = 0;
        
        m_LineHeight = 0;
        m_Ascent = 0;
    }

    CParagraphGroup::~CParagraphGroup()
    {
    }

    void CParagraphGroup::Layout(
        IZ_UINT width,
        IZ_UINT height)
    {
        m_Height = height;

        if (m_Width == width)
        {
            // Don't layout if width is not changed.
            return;
        }

        // 2 is margin.
        IZ_UINT lineWidth = width - 2;

        CStdList<CParagraph>::Item* paragraphListItem = m_Paragraphs.GetTop();
        while (paragraphListItem != IZ_NULL)
        {
            CParagraph* paragraph = paragraphListItem->GetData();

            paragraph->Layout(lineWidth);

            paragraphListItem = paragraphListItem->GetNext();
        }
    }

    void CParagraphGroup::Prepare(graph::CGraphicsDevice* device)
    {
        CStdList<CParagraph>::Item* paragraphListItem = m_Paragraphs.GetTop();
        while (paragraphListItem != IZ_NULL)
        {
            CParagraph* paragraph = paragraphListItem->GetData();

            paragraph->Prepare(m_LineHeight, m_Ascent, device);

            paragraphListItem = paragraphListItem->GetNext();
        }
    }

    void CParagraphGroup::Render(
        IZ_UINT x,
        IZ_UINT y,
        graph::CGraphicsDevice* device)
    {
        CStdList<CParagraph>::Item* listItem = m_Paragraphs.GetTop();
        while (listItem != IZ_NULL)
        {
            CParagraph* paragraph = listItem->GetData();

            paragraph->Render(x, y, m_LineHeight, device);

            listItem = listItem->GetNext();
        }
    }

    void CParagraphGroup::AddParagraph(CParagraph* paragraph)
    {
        m_Paragraphs.AddTail(paragraph->GetListItem());
    }

    /////////////////////////////////////////////////////////

    CDefaultParagraphGroup::CDefaultParagraphGroup()
    {
        m_FontHost = IZ_NULL;
    }

    CDefaultParagraphGroup::~CDefaultParagraphGroup()
    {
        SAFE_RELEASE(m_FontHost);
    }

    #define CH_LF   (0x000A)
    #define CH_CR   (0x000D)
    #define CH_LSEP (0x2028)
    #define CH_PSEP (0x2029)

    // ���s�R�[�h
	// Mac OS X ���܂� Unix�iLF�j
	// �N���V�b�N Mac OS�iCR�j
	// Windows�iCR + LF�j
	// ���j�R�[�h�s�^�i����؂�iLSEP�^PSEP�j
    static const IZ_UINT separators[] = {
        CH_LF,      // LF
        CH_CR,      // CR
        CH_LSEP,    // LSEP
        CH_PSEP,    // PSEP
    };

    namespace
    {
        inline IZ_BOOL _IsSeparator(IZ_UINT code)
        {
            for (size_t i = 0; i < COUNTOF(separators); i++)
            {
                if (separators[i] == code)
                {
                    return IZ_TRUE;
                }
            }

            return IZ_FALSE;
        }
    }

    IZ_BOOL CDefaultParagraphGroup::Init(
        IFontHost* host,
        CUString& str, 
        void* userData)
    {
        SAFE_REPLACE(m_FontHost, host);

        IZ_UINT prev = 0;
        IZ_UINT code = 0;

        IZ_BOOL isSeparated = IZ_FALSE;

        const IZ_UINT8* start = str.GetTextPtr();

        str.BeginIter();

        for (;;)
        {
            code = str.GetNext();

            if (prev == CH_CR && code == CH_LF)
            {
                // skip
                code = str.GetNext();
            }

            prev = code;

            if (isSeparated)
            {
                start = str.GetTextPtr();

                isSeparated = IZ_FALSE;
            }

            if (_IsSeparator(code) || code == 0)
            {
                IZ_UINT bytes = static_cast<IZ_UINT>(str.GetIterDistance());

                // TODO
                // Create paragraph

                if (code == 0)
                {
                    break;
                }
                else
                {
                    isSeparated = IZ_TRUE;
                }
            }
        }

        str.EndIter();

        return IZ_TRUE;
    }
}   // namespace text
}   // namespace izanagi