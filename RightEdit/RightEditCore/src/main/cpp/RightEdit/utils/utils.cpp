#include "utils.h"


uint32_t MixINT16(uint8_t *pDst, uint8_t *pSrc1, uint8_t *pSrc2, uint32_t dwLength)
{
    if (dwLength <= 0 || pDst == NULL || pSrc1 == NULL || pSrc2 == NULL)
        return 0;
    if (pDst != pSrc1 && pDst != pSrc2)
    {
        memset(pDst, 0, dwLength);
    }
    //将数据各自读取到预混音缓冲中
    int32_t temp = 0;
    uint32_t num = dwLength / sizeof(short);
    short *pmix = (short *) pDst;
    short *ps1 = (short *) pSrc1;
    short *ps2 = (short *) pSrc2;
    for (uint32_t i = 0; i < num; i++)
    {
        temp = int32_t(*ps1) + int32_t(*ps2);
        if (temp > 32767)
            *pmix = 32767;
        else if (temp < -32768)
            *pmix = -32768;
        else
            *pmix = (short) temp;
        pmix++;
        ps1++;
        ps2++;
    }
    return dwLength;
}

void SleepMS(int ms)
{
    usleep(ms * 1000);
}

uint32_t P2U32(unsigned char **pSrc)
{
    unsigned char *p = *pSrc;
    uint32_t i = (uint32_t) ((p[0]) << 24 | (p[1]) << 16 | (p[2]) << 8 | (p[3]));
    *pSrc += 4;
    return i;
}


uint32_t P2U32(unsigned char *pSrc)
{
    unsigned char *p = pSrc;
    uint32_t i = (uint32_t) ((p[0]) << 24 | (p[1]) << 16 | (p[2]) << 8 | (p[3]));
    return i;
}

