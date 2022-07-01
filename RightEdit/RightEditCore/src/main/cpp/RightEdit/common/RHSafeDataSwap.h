//
// Created by Hope liao on 2018/8/1.
//

#ifndef TESTMAGICENGINE_DATASWAP_H
#define TESTMAGICENGINE_DATASWAP_H

#include "RHLock.h"
#include "../common/RHRefObject.h"


class RHSafeDataSwap : public RHRefObject{

public:
    RHSafeDataSwap();

    ~RHSafeDataSwap();

    void apply(long _size);

    void resize(long _size);

    void reback();

    void writeData(void *_data, long len);

    void fetchData(void *_data, long len);

    void setWrite();

    bool isWrite();

    void *getData();

    long getSize();

    void lockData();

    void unlockData();

    void swap(RHSafeDataSwap *_pTarget);

    void toString();

protected:
    void *m_pData;
    long m_Size;

    RHLock rLock;
public:
    bool m_Write;

};

class RSafeDataDoubleSwap : public RHRefObject{
public:
    RSafeDataDoubleSwap();
    ~RSafeDataDoubleSwap();

    void apply(int _size);
    void resize(int _size);
    void reback();
    int getSize();

    RHSafeDataSwap *getCurrentData();
    RHSafeDataSwap *getNextData();

    void swap();

private:
    RHSafeDataSwap *m_pCurrentData;
    RHSafeDataSwap *m_pNextData;
};



#endif //TESTMAGICENGINE_DATASWAP_H
