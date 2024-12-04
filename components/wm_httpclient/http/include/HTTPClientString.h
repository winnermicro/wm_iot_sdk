
#ifndef _HTTP_CLIENT_STRING
#define _HTTP_CLIENT_STRING

#include "HTTPClientWrapper.h" // Cross platform support
#include "HTTPClient.h" 

///////////////////////////////////////////////////////////////////////////////
//
// Section      : HTTP Api global definitions
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

int                     stricmp(const char *s1, const char *s2);
bool                    HTTPStrInsensitiveCompare   (const char *pSrc, const char* pDest, UINT32 nLength);
bool                    HTTPStrSearch               (char *pSrc, char *pSearched, UINT32 nOffset, UINT32 nScope,HTTP_PARAM *HttpParam);
char                    HTTPStrExtract              (char *pParam,UINT32 nOffset,char Restore);
char*                   HTTPStrCaseStr              (const char *pSrc, UINT32 nSrcLength, const char *pFind); 
char*                   HTTPStrGetToken             (char *pSrc, UINT32 nSrcLength, char *pDest, UINT32 *nDestLength);
uint32_t                HTTPStrGetDigestToken       (HTTP_PARAM pParamSrc, char *pSearched, HTTP_PARAM *pParamDest);
uint32_t                HTTPStrHToL                 (char * s); 
char*                   HTTPStrLToH                 (char * dest,UINT32 nSrc);        
#endif
