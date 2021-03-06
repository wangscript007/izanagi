﻿#include "std/StdBase64.h"
#include "std/StdUtil.h"

namespace izanagi {
    // Character table.
    const IZ_CHAR CBase64::table[] =
    {
        // Large alphabet.
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
        'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
        'U', 'V', 'W', 'X', 'Y', 'Z',

        // Small alphabet.
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
        'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
        'u', 'v', 'w', 'x', 'y', 'z', 

        // Numbers.
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 

        // Plus.
        '+',

        // Slush.
        '/',
    };

    // base64 エンコード
    IZ_BOOL CBase64::Encode(
        const std::vector<IZ_BYTE>& src,
        std::vector<IZ_CHAR>& dst)
    {
        std::vector<IZ_CHAR> cdst;

        for (std::size_t i = 0; i < src.size(); ++i) {
            switch (i % 3) {
            case 0:
                cdst.push_back(table[(src[i] & 0xFC) >> 2]);
                if (i + 1 == src.size()) {
                    cdst.push_back(table[(src[i] & 0x03) << 4]);
                    cdst.push_back('=');
                    cdst.push_back('=');
                }

                break;
            case 1:
                cdst.push_back(table[((src[i - 1] & 0x03) << 4) | ((src[i + 0] & 0xF0) >> 4)]);
                if (i + 1 == src.size()) {
                    cdst.push_back(table[(src[i] & 0x0F) << 2]);
                    cdst.push_back('=');
                }

                break;
            case 2:
                cdst.push_back(table[((src[i - 1] & 0x0F) << 2) | ((src[i + 0] & 0xC0) >> 6)]);
                cdst.push_back(table[src[i] & 0x3F]);

                break;
            }
        }

        dst.swap(cdst);

        return IZ_TRUE;
    }

    // Convert from code to a character in the character table.
    IZ_INT CBase64::FindTable(IZ_CHAR code)
    {
        IZ_INT ret = -1;

        if ('A' <= code && code <= 'Z') {
            ret = code - 'A' + LARGE_ALPHABET_POS;
        }
        else if ('a' <= code && code <= 'z') {
            ret = code - 'a' + SMALL_ALPHABET_POS;
        }
        else if ('0' <= code && code <= '9') {
            ret = code - '0' + NUMBER_POS;
        }
        else if (code == '+') {
            ret = PLUS_POS;
        }
        else if (code == '/') {
            ret = SLUSH_POS;
        }

        return ret;
    }


    // base64 デコード
    IZ_BOOL CBase64::Decode(
        const std::vector<IZ_CHAR>& src,
        std::vector<IZ_BYTE>& dst)
    {
        if (src.size() & 0x00000003) {
            return IZ_FALSE;
        }
        else {
            std::vector<IZ_BYTE> cdst;

            for (std::size_t i = 0; i < src.size(); i += 4) {
                if (src[i + 0] == '=') {
                    return IZ_FALSE;
                }
                else if (src[i + 1] == '=') {
                    return IZ_FALSE;
                }
                else if (src[i + 2] == '=') {
                    IZ_INT s1 = FindTable(src[i + 0]);
                    IZ_INT s2 = FindTable(src[i + 1]);

                    if (s1 < 0 || s2 < 0) {
                        return IZ_FALSE;
                    }

                    cdst.push_back(static_cast<IZ_BYTE>(((s1 & 0x3F) << 2) | ((s2 & 0x30) >> 4)));

                    break;
                }
                else if (src[i + 3] == '=') {
                    IZ_INT s1 = FindTable(src[i + 0]);
                    IZ_INT s2 = FindTable(src[i + 1]);
                    IZ_INT s3 = FindTable(src[i + 2]);

                    if (s1 < 0 || s2 < 0 || s3 < 0) {
                        return IZ_FALSE;
                    }

                    cdst.push_back(static_cast<IZ_BYTE>(((s1 & 0x3F) << 2) | ((s2 & 0x30) >> 4)));
                    cdst.push_back(static_cast<IZ_BYTE>(((s2 & 0x0F) << 4) | ((s3 & 0x3C) >> 2)));

                    break;
                }
                else {
                    const std::string::size_type s1 = FindTable(src[i + 0]);
                    const std::string::size_type s2 = FindTable(src[i + 1]);
                    const std::string::size_type s3 = FindTable(src[i + 2]);
                    const std::string::size_type s4 = FindTable(src[i + 3]);

                    if (s1 < 0 || s2 < 0 || s3 < 0 || s4 < 0) {
                        return IZ_FALSE;
                    }

                    cdst.push_back(static_cast<IZ_BYTE>(((s1 & 0x3F) << 2) | ((s2 & 0x30) >> 4)));
                    cdst.push_back(static_cast<IZ_BYTE>(((s2 & 0x0F) << 4) | ((s3 & 0x3C) >> 2)));
                    cdst.push_back(static_cast<IZ_BYTE>(((s3 & 0x03) << 6) | ((s4 & 0x3F) >> 0)));
                }
            }

            if (cdst.size() > 0) {
                dst.swap(cdst);
                return IZ_TRUE;
            }
        }

        return IZ_FALSE;
    }

    IZ_UINT CBase64::Encode(
        const IZ_BYTE* src,
        IZ_UINT srcLength,
        IZ_CHAR* dst)
    {
        IZ_CHAR* cdst = dst;

        for (IZ_UINT i = 0; i < srcLength; ++i) {
            switch (i % 3) {
            case 0:
                *cdst++ = table[(src[i] & 0xFC) >> 2];
                if (i + 1 == srcLength) {
                    *cdst++ = table[(src[i] & 0x03) << 4];
                    *cdst++ = '=';
                    *cdst++ = '=';
                }

                break;
            case 1:
                *cdst++ = table[((src[i - 1] & 0x03) << 4) | ((src[i + 0] & 0xF0) >> 4)];
                if (i + 1 == srcLength) {
                    *cdst++ = table[(src[i] & 0x0F) << 2];
                    *cdst++ = '=';
                }

                break;
            case 2:
                *cdst++ = table[((src[i - 1] & 0x0F) << 2) | ((src[i + 0] & 0xC0) >> 6)];
                *cdst++ = table[src[i] & 0x3F];

                break;
            }
        }

        IZ_UINT ret = CStdUtil::GetPtrDistance(cdst, dst);

        return ret;
    }

    IZ_UINT CBase64::Decode(
        const IZ_CHAR* src,
        IZ_UINT srcLength,
        IZ_BYTE* dst)
    {
        if (srcLength & 0x00000003 || src == IZ_NULL) {
            IZ_ASSERT(IZ_FALSE);
            return 0;
        }
        else {
            IZ_BYTE* cdst = dst;

            for (IZ_UINT i = 0; i < srcLength; i += 4) {
                if (src[i + 0] == '=') {
                    return 0;
                }
                else if (src[i + 1] == '=') {
                    return 0;
                }
                else if (src[i + 2] == '=') {
                    IZ_INT s1 = FindTable(src[i + 0]);
                    IZ_INT s2 = FindTable(src[i + 1]);

                    if (s1 < 0 || s2 < 0) {
                        return 0;
                    }

                    *cdst++ = static_cast<IZ_BYTE>(((s1 & 0x3F) << 2) | ((s2 & 0x30) >> 4));

                    break;
                }
                else if (src[i + 3] == '=') {
                    IZ_INT s1 = FindTable(src[i + 0]);
                    IZ_INT s2 = FindTable(src[i + 1]);
                    IZ_INT s3 = FindTable(src[i + 2]);

                    if (s1 < 0 || s2 < 0 || s3 < 0) {
                        return 0;
                    }

                    *cdst++ = static_cast<IZ_BYTE>(((s1 & 0x3F) << 2) | ((s2 & 0x30) >> 4));
                    *cdst++ = static_cast<IZ_BYTE>(((s2 & 0x0F) << 4) | ((s3 & 0x3C) >> 2));

                    break;
                }
                else {
                    const std::string::size_type s1 = FindTable(src[i + 0]);
                    const std::string::size_type s2 = FindTable(src[i + 1]);
                    const std::string::size_type s3 = FindTable(src[i + 2]);
                    const std::string::size_type s4 = FindTable(src[i + 3]);

                    if (s1 < 0 || s2 < 0 || s3 < 0 || s4 < 0) {
                        return 0;
                    }

                    *cdst++ = static_cast<IZ_BYTE>(((s1 & 0x3F) << 2) | ((s2 & 0x30) >> 4));
                    *cdst++ = static_cast<IZ_BYTE>(((s2 & 0x0F) << 4) | ((s3 & 0x3C) >> 2));
                    *cdst++ = static_cast<IZ_BYTE>(((s3 & 0x03) << 6) | ((s4 & 0x3F) >> 0));
                }
            }

            IZ_UINT ret = CStdUtil::GetPtrDistance(cdst, dst);

            return ret;
        }
    }
}   // namespace izanagi
