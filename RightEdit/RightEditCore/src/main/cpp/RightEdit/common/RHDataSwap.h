#ifndef RIGHTEDIT_RHDATASWAP_H
#define RIGHTEDIT_RHDATASWAP_H


#include "RHRefObject.h"

class RHDataSwap : public RHRefObject{
public:
    RHDataSwap();
    ~RHDataSwap();

    void apply(long _size);
    void resize(long _size);
    void ensureSize(long size);
    void reback();
    void writeData(void *_data, long len);
    void fetchData(void *_data, long len);
    void setWrite();
    bool isWrite();
    void *getData();
    long getSize();

    void swap(RHDataSwap *_pTarget);
    void toString();

protected:
    void *m_pData;
    long m_Size;
public:
    bool m_Write;

};

#endif //RIGHTEDIT_RHDATASWAP_H