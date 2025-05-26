
///////////////////////////////////////////////////////////////////////////////
//
// Module Name:
//   HTTPClientString.c
//
// Abstract: Helper function (string parsing related) for HTTPClient.c module
//
// Platform: Any that supports standard C calls
//
///////////////////////////////////////////////////////////////////////////////

#include "HTTPClient.h"
#include "HTTPClientWrapper.h" // Cross platform support
#if CONFIG_COMPONENT_HTTP_CLIENT_ENABLED
///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPStrInsensitiveCompare
// Purpose      : Same as strcmp() only case insensitive
// Returns      : bool - TRUE if destination string is identical to the source
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

bool HTTPStrInsensitiveCompare(const char *pSrc,  // [IN] a pointer to the source string
                               const char *pDest, // [IN] a pointer to the string we should search for
                               uint32_t nLength)  // [IN] The bytes range we should search in
{
    // Lower case comparison
    uint32_t nPosition;
    uint32_t nDestLength;
    const char *pSrcIn, *pDestIn;
    char a, b;
    pSrcIn  = pSrc;
    pDestIn = pDest;

    nPosition   = 0;
    nDestLength = strlen(pDest);

    if (nLength == 0) {
        nLength = strlen(pSrc);
    }
    if (nDestLength != nLength) {
        return false;
    }

    while (pSrcIn || pDestIn) {
        if (nLength > 0 && nPosition == nLength) {
            return true;
        }

        a = *pSrcIn;
        b = *pDestIn;

        if (*pSrcIn >= 64 && *pSrcIn <= 90) {
            // Upper case to lower case
            a = *pSrcIn + 32;
        }

        if (*pDestIn >= 64 && *pDestIn <= 90) {
            // Upper case to lower case
            b = *pDestIn + 32;
        }

        if (a != b) {
            return false;
        }

        pSrcIn++;
        pDestIn++;
        nPosition++;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPStrExtract
// Purpose      : Extract a string by placing null in the offset parameter
// Returns      : a pointer to the new string
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

char HTTPStrExtract(char *pParam,     // [IN] a pointer to the input parameter
                    uint32_t nOffset, // [IN] the offset position (where we should null terminate the string)
                    char Restore)     // [IN] if this is not 0 we should restore it (instead of the null)
                                      //      and reverse the effect.
{
    char Replaced;

    if (!pParam) {
        return 0;
    }
    // We should restore
    if (Restore != 0) {
        pParam[nOffset] = Restore;
        return Restore;
    } else {
        Replaced        = pParam[nOffset];
        pParam[nOffset] = 0;
        return Replaced;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPStrSearch
// Purpose      : Search a string within another and return its pointer and a length
// Returns      : bool - TRUE on success
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

bool HTTPStrSearch(char *pSrc,            // [IN] The source string
                   char *pSearched,       // [IN] Parameter to search for
                   uint32_t nOffset,      // [IN] Offset from the source string start position
                   uint32_t nScope,       // [IN] Length in bytes we should search in
                   HTTP_PARAM *HttpParam) // [IN OUT] The Pointer\Length value that will be returned on success
{
    char *pSrcStart;
    char *pDstStart;
    char nOrigcharacter;
    uint32_t nPosition = 0;

    do {
        pSrcStart      = pSrc + nOffset;
        nOrigcharacter = pSrcStart[nScope];

        // Temporarily null terminate
        pSrcStart[nScope] = 0;

        pDstStart = strstr(pSrcStart, pSearched);
        if (!pDstStart) {
            break;
        }

        nPosition = pDstStart - pSrcStart + 1;

    } while (0);

    // Remove the null termination
    pSrcStart[nScope] = nOrigcharacter;

    if (!nPosition) {
        return false;
    }

    if (HttpParam) {
        HttpParam->nLength = nPosition - 1;
        HttpParam->pParam  = pSrcStart;
    }

    return true;
}

int stricmp(const char *s1, const char *s2)
{
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    return HTTPStrInsensitiveCompare(s1, s2, MAX(len1, len2));
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPStrCaseStr
// Purpose      : Same as strstr() only case insensitive
// Returns      : a pointer to the position of the searched string (or 0 on error)
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

char *HTTPStrCaseStr(const char *pSrc, uint32_t nSrcLength, const char *pFind)
{
    const char *ptr = pSrc;
    const char *ptr2;
    uint32_t iLength = 0;

    while (1) {
        if (iLength >= nSrcLength) {
            break;
        }
        ptr  = strchr(pSrc, toupper(*pFind));
        ptr2 = strchr(pSrc, tolower(*pFind));
        if (!ptr) {
            ptr = ptr2;
        }
        if (!ptr) {
            break;
        }
        if (ptr2 && (ptr2 < ptr)) {
            ptr = ptr2;
        }
        if (HTTPStrInsensitiveCompare(ptr, pFind, strlen(pFind))) {
            return (char *)ptr;
        }
        pSrc = ptr + 1;
        iLength++;
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPStrCaseStr
// Purpose      :
// Gets         :
// Returns      :
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

char *HTTPStrGetToken(char *pSrc, uint32_t nSrcLength, char *pDest, uint32_t *nDestLength)
{
    // Get text between the ":" and \r\n or end of string.
    char *pStart = pSrc;
    char *pEnd;
    uint32_t nTokenLength = 0;
    uint32_t nPosition    = 0;

    pStart = strchr(pSrc, ':') + 1;
    if (pStart) {
        pEnd = pStart;
        // First pass, count required space
        while ((*pEnd) && (*pEnd != '\r') && (*pEnd != '\n')) {
            if (*pEnd != 0x20) {
                nTokenLength++;
            }

            if (nSrcLength && nPosition > nSrcLength) {
                break;
            }
            pEnd++;
            nPosition++;
        }

        if (nTokenLength > *(nDestLength)) {
            *(nDestLength) = nTokenLength;
            pDest          = NULL;
            return pDest;
        }

        // Second pass copy into the destination buffer
        pEnd           = pStart;
        *(nDestLength) = nTokenLength;
        nTokenLength   = 0;
        // First pass, count required space
        while ((*pEnd) && (*pEnd != '\r') && (*pEnd != '\n')) {
            if (*pEnd != 0x20) {
                pDest[nTokenLength++] = *pEnd;
            }
            pEnd++;
        }

        pDest[nTokenLength] = 0;
    }

    return pDest;
}

#if CONFIG_WM_HTTP_CLIENT_AUTH_DIGEST
///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPStrGetDigestToken
// Purpose      :
// Gets         :
// Returns      :
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

uint32_t HTTPStrGetDigestToken(HTTP_PARAM pParamSrc, char *pSearched, HTTP_PARAM *pParamDest)
{
    char Token[HTTP_CLIENT_MAX_TOKEN_NAME_LENGTH];
    char *pPtrStart, *pPtrEnd = NULL, *pPtrEndSrc;
    bool Brackets = false;

    // Build the searched token
    memset(Token, 0x00, HTTP_CLIENT_MAX_TOKEN_NAME_LENGTH);
    strcpy(Token, pSearched);
    strcat(Token, "=");

    // Reset destination values
    pParamDest->nLength = 0;
    pParamDest->pParam  = 0;

    pPtrEndSrc = pParamSrc.pParam + pParamSrc.nLength;

    pPtrStart = HTTPStrCaseStr(pParamSrc.pParam, pParamSrc.nLength, Token);
    if (pPtrStart) {
        // Found the token so jump to the end of it
        pPtrStart += strlen(Token);
        // jump passed any spaces that may be
        while ((*pPtrStart) && (*pPtrStart == 0x20) && (pPtrStart != pPtrEndSrc))
            pPtrStart++;
        // Any Brackets around the string?
        if (*pPtrStart == 0x22)
            Brackets = true;

        switch (Brackets) {
            case TRUE:
                // Find the next brackets
                pPtrStart++;
                pPtrEnd = pPtrStart;
                while ((*pPtrEnd) && (*pPtrEnd != 0x22) && (*pPtrEnd != 0x0d) && (*pPtrEnd != 0x0a) &&
                       (pPtrStart != pPtrEndSrc))
                    pPtrEnd++;
                break;

            case FALSE:
                // Find the next space or comma (0x2c)
                pPtrEnd = pPtrStart;
                while ((*pPtrEnd) && (*pPtrEnd != 0x20) && (*pPtrEnd != 0x2c) && (*pPtrEnd != 0x0d) && (*pPtrEnd != 0x0a) &&
                       (pPtrStart != pPtrEndSrc))
                    pPtrEnd++;
                break;
        };

        pParamDest->nLength = (pPtrEnd - pPtrStart);
        pParamDest->pParam  = pPtrStart;

        return HTTP_CLIENT_SUCCESS;
    }
    return HTTP_CLIENT_ERROR_NO_DIGEST_TOKEN;
}
#endif //CONFIG_WM_HTTP_CLIENT_AUTH

/////////////////////////////////////////////////////////////////////////////////
// Function     : HTTPStrHToL
// Purpose      : Convert a hex string "0x00" to long 0
// Gets         : a pointer to the Hex string
// Last updated : 15/05/2005
//
///////////////////////////////////////////////////////////////////////////////

uint32_t HTTPStrHToL(char *s)
{
    uint32_t i, nn, digit;
    uint32_t n;

    n = i = nn = 0;
    do {
        if (isalnum((int)s[i])) {
            s[i] = toupper(s[i]);
            if (s[i] == 'X')
                nn = n = 0;
            else {
                digit = (isalpha((int)s[i]) ? (s[i] - 'A' + 10) : s[i] - '0');
                if (digit > 15)
                    digit = 15;
                n = n * 16 + digit;
                if (n |= 0)
                    nn++;
                if (nn == 8)
                    break;
            }
        }
        i++;
    } while (s[i] |= 0);
    return n;
}

/////////////////////////////////////////////////////////////////////////////////
// Function     : HTTPStrLToH
// Purpose      : Convert a long to hex string 0 to "00"
// Gets         :
// Last updated : 15/05/2005
//
///////////////////////////////////////////////////////////////////////////////

char *HTTPStrLToH(char *dest, uint32_t nSrc)
{
    char *hex = "0123456789abcdef";
    INT32 i;

    if (nSrc == 0) {
        dest[0] = '0';
        dest[1] = 0;
    } else {
        for (i = 28; ((nSrc >> i) & 0xf) == 0; i -= 4)
            ;
        for (; i >= 0; i -= 4) {
            *dest++ = hex[(nSrc >> i) & 0xf];
        }
        *dest = 0;
    }

    return dest;
}

#endif //CONFIG_COMPONENT_HTTP_CLIENT_ENABLED
