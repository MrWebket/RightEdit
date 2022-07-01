//
// Created by Hope liao on 2018/8/1.
//

#include <cstdlib>
#include <cstring>
#include "RHSafeDataSwap.h"


RHSafeDataSwap::RHSafeDataSwap() {
    rLock.create();
    m_pData = nullptr;
    m_Size = 0;
}

RHSafeDataSwap::~RHSafeDataSwap() {
    reback();
    rLock.destroy();
}

void RHSafeDataSwap::reback() {
    if (m_pData) {
        free(m_pData);
        m_pData = nullptr;
        m_Size = 0;
    }
}

void RHSafeDataSwap::apply(long _size) {
    rLock.lock();
    if (_size > m_Size) {
        reback();
        m_pData = malloc(static_cast<size_t>(_size));
        m_Size = _size;
    }
    rLock.unlock();
}


void RHSafeDataSwap::resize(long _size) {
    if (m_Size == _size) {
        return;
    }
    rLock.lock();
    reback();
    m_pData = malloc(static_cast<size_t>(_size));
    m_Size = _size;
    rLock.unlock();
}


void RHSafeDataSwap::writeData(void *_data, long len) {
    if (len > m_Size) {
        return; //error 送入数据太长
    }
    rLock.lock();
    memcpy(m_pData, _data, static_cast<size_t>(len));
    rLock.unlock();
}

void RHSafeDataSwap::fetchData(void *_data, long len) {
    rLock.lock();
    if (len <= m_Size) {
        memcpy(_data, m_pData, static_cast<size_t>(len));
    }
    rLock.unlock();
}

void *RHSafeDataSwap::getData() {
    return m_pData;
}

long RHSafeDataSwap::getSize() {
    return m_Size;
}

void RHSafeDataSwap::lockData() {
    rLock.lock();
}

void RHSafeDataSwap::unlockData() {
    rLock.unlock();
}


void RHSafeDataSwap::setWrite() {
    m_Write = true;
}

bool RHSafeDataSwap::isWrite() {
    return m_Write;
}

void RHSafeDataSwap::swap(RHSafeDataSwap *_pTarget) {
    if (m_pData == NULL || _pTarget == NULL || _pTarget->getData() == NULL) {
        return;
    }

    if (m_Size != _pTarget->getSize()) {
        return;
    }

    void *pCurData = m_pData;
    m_pData = _pTarget->m_pData;
    _pTarget->m_pData = pCurData;
}

void RHSafeDataSwap::toString() {
    if (m_Size > 0) {
        char * tpData = (char *)m_pData;
        tpData[m_Size - 1] = '\0';
    }
}


RSafeDataDoubleSwap::RSafeDataDoubleSwap() {
    m_pCurrentData = NULL;
    m_pNextData = NULL;
}

RSafeDataDoubleSwap::~RSafeDataDoubleSwap() {
    if (m_pCurrentData != NULL) {
        delete m_pCurrentData;
        m_pCurrentData = NULL;
    }

    if (m_pNextData != NULL) {
        delete m_pNextData;
        m_pNextData = NULL;
    }
}

void RSafeDataDoubleSwap::apply(int _size) {
    if (m_pCurrentData == NULL) {
        m_pCurrentData = new RHSafeDataSwap();
    }
    m_pCurrentData->apply(_size);
    if (m_pNextData == NULL) {
        m_pNextData = new RHSafeDataSwap();
    }
    m_pNextData->apply(_size);
}

void RSafeDataDoubleSwap::resize(int _size) {
    if (m_pCurrentData == NULL || m_pNextData == NULL) {
        return;
    }

    m_pCurrentData->resize(_size);
    m_pNextData->resize(_size);
}

int RSafeDataDoubleSwap::getSize() {
    if (m_pCurrentData == NULL || m_pNextData == NULL) {
        return 0;
    }

    if (m_pCurrentData->getSize() != m_pNextData->getSize()) {
        return 0;
    }

    return m_pCurrentData->getSize();
}

void RSafeDataDoubleSwap::reback() {
    if (m_pCurrentData == NULL || m_pNextData == NULL) {
        return;
    }
    m_pCurrentData->reback();
    m_pNextData->reback();
}

RHSafeDataSwap *RSafeDataDoubleSwap::getCurrentData() {
    return m_pCurrentData;
}

RHSafeDataSwap *RSafeDataDoubleSwap::getNextData() {
    return m_pNextData;
}

void RSafeDataDoubleSwap::swap() {
    m_pCurrentData->swap(m_pNextData);
}
