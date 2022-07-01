#include "RHDataSwap.h"
#include <cstdlib>
#include <cstring>

RHDataSwap::RHDataSwap() {
    m_pData = nullptr;
    m_Size = 0;
}

RHDataSwap::~RHDataSwap() {
    reback();
}

void RHDataSwap::reback() {
    if (m_pData) {
        free(m_pData);
        m_pData = nullptr;
        m_Size = 0;
    }
}

void RHDataSwap::apply(long _size) {
    if (_size > m_Size) {
        reback();
        m_pData = malloc(static_cast<size_t>(_size));
        m_Size = _size;
    }
}


void RHDataSwap::resize(long _size) {
    if (m_Size == _size) {
        return;
    }
    reback();
    m_pData = malloc(static_cast<size_t>(_size));
    m_Size = _size;
}

void RHDataSwap::ensureSize(long size) {
    if(m_Size == 0) {
        resize(size);
    } else {
        if (size > m_Size)
        {
            m_pData = (uint8_t *) realloc(m_pData, size);
            m_Size = size;
        }
    }
}


void RHDataSwap::writeData(void *_data, long len) {
    if (len > m_Size) {
        return; //error 送入数据太长
    }
    memcpy(m_pData, _data, static_cast<size_t>(len));
}

void RHDataSwap::fetchData(void *_data, long len) {
    if (len <= m_Size) {
        memcpy(_data, m_pData, static_cast<size_t>(len));
    }
}

void *RHDataSwap::getData() {
    return m_pData;
}

long RHDataSwap::getSize() {
    return m_Size;
}


void RHDataSwap::setWrite() {
    m_Write = true;
}

bool RHDataSwap::isWrite() {
    return m_Write;
}

void RHDataSwap::swap(RHDataSwap *_pTarget) {
    if (_pTarget == NULL || _pTarget->getData() == NULL) {
        return;
    }

    void *pCurData = m_pData;
    int pCurSize = m_Size;

    m_pData = _pTarget->m_pData;
    m_Size = _pTarget->m_Size;

    _pTarget->m_Size = pCurSize;
    _pTarget->m_pData = pCurData;
}

void RHDataSwap::toString() {
    if (m_Size > 0) {
        char * tpData = (char *)m_pData;
        tpData[m_Size - 1] = '\0';
    }
}