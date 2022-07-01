//
// Created by Hope liao on 2019/2/21.
//

#include "../common/RHAppDef.h"
#include "RHGLTexture.h"
#include "RHGLUtil.h"
#include "../common/RHSafeDataSwap.h"
#include "../common/RHAppDef.h"

RHGLTexture *RHGLTexture::createExtTexture(const char* _name, int _tex, int _w, int _h) {
    auto * _pTex = new RHGLTexture();
    _pTex->setTexWidth(_w);
    _pTex->setTexHeight(_h);
    _pTex->setTexName(_name);
    _pTex->setIsExt(true);
    _pTex->setTexId(_tex);
    _pTex->setIsLoad(true);
    return _pTex;
}
RHGLTexture * RHGLTexture::createTexture(const char *name, int width, int height) {
    auto * _pTex = new RHGLTexture();
    _pTex->setTexWidth(width);
    _pTex->setTexHeight(height);
    _pTex->setTexName(name);
    _pTex->apply();

    return _pTex;
}

RHGLTexture * RHGLTexture::createTexture(const char *name, int width, int height, int _informat, int _dataFormat, int _texDataType) {
    auto * _pTex = new RHGLTexture();
    _pTex->setTexWidth(width);
    _pTex->setTexHeight(height);
    _pTex->setTexName(name);
    _pTex->setDataFormat(_dataFormat);
    _pTex->setInFormat(_informat);
    _pTex->setTexDataType(_texDataType);
    _pTex->apply();
    return _pTex;
}

RHGLTexture::RHGLTexture() {
    rLock.create();

    mTexWidth = 0;
    mTexHeight = 0;
    mTexName = "";
    m_target = GL_TEXTURE_2D;
    mInFormat = GL_RGBA;
    mDataFormat = GL_RGBA;
    mIsLoad = false;
    mTexId = static_cast<unsigned int>(GL_DEF_CODE);

    m_pDataSwap = nullptr;
    mTexDataType = GL_UNSIGNED_BYTE;

    mEnableMipMaps = false;
    mNeedUpdate = false;
    mIsExt = false;

    mWrapS = GL_CLAMP_TO_EDGE;
    mWrapT = GL_CLAMP_TO_EDGE;
    mMagFilter = GL_LINEAR;
    mMinFilter = GL_LINEAR;
}

RHGLTexture::~RHGLTexture() {
    rLock.destroy();
    if (m_pDataSwap) {
        m_pDataSwap->reback();
        m_pDataSwap->release();
        m_pDataSwap = nullptr;
    }
    if (!getIsExt()) { //代表不是外部引用纹理则进行删除
        RHGLUtil::deleteTexture(mTexId);
        ALOGI("delete tex:%s, id = %d \n", mTexName.c_str(), mTexId);
        _GL_CHECK_ERROR_
    }
}

void RHGLTexture::apply() {
    if (getTexId() == GL_DEF_CODE) {
        mTexId = RHGLUtil::makeTexture2D(mTexWidth, mTexHeight, mInFormat, mDataFormat,
                                         mWrapS,
                                         mWrapT, mMinFilter, mMagFilter, nullptr,
                                         mTexDataType,
                                         mEnableMipMaps);
        _GL_CHECK_ERROR_
    }
}

void RHGLTexture::apply(void *data) {
    if (getTexId() == GL_DEF_CODE) {
        mTexId = RHGLUtil::makeTexture2D(mTexWidth, mTexHeight, mInFormat, mDataFormat,
                                         mWrapS,
                                         mWrapT, mMinFilter, mMagFilter, data, mTexDataType,
                                         mEnableMipMaps);
        _GL_CHECK_ERROR_
    }
}

void RHGLTexture::putData(void *data, int len) {
    rLock.lock();
    if (m_pDataSwap == nullptr) {
        m_pDataSwap = new RHSafeDataSwap();
    }

    if (len == 0) {
        if (mInFormat == GL_RGBA) {
            len = getTexWidth() * getTexHeight() * 4;
        } else if (mInFormat == GL_LUMINANCE) {
            len = getTexWidth() * getTexHeight();
        } else if (mInFormat == GL_LUMINANCE_ALPHA) {
            len = getTexWidth() * getTexHeight() * 2;
        } else if (mInFormat == GL_RGB) {
            len = getTexWidth() * getTexHeight() * 3;
        }
    }

    m_pDataSwap->apply(len);
    m_pDataSwap->writeData(data, len);

    mNeedUpdate = true;
    setIsLoad(true);
    rLock.unlock();
}

void RHGLTexture::commit() {
    rLock.lock();
    if (m_pDataSwap && getIsLoad()) {
        if (getTexId() == GL_DEF_CODE) {
            m_pDataSwap->lockData();
            apply(m_pDataSwap->getData());
            m_pDataSwap->unlockData();
        } else {
            if (mNeedUpdate) {
                m_pDataSwap->lockData();
                RHGLUtil::subTextureBuffer(m_target, mInFormat, getTexId(),
                                           getTexWidth(), getTexHeight(),
                                           mTexDataType, m_pDataSwap->getData());
                _GL_CHECK_ERROR_
                mNeedUpdate = false;
                m_pDataSwap->unlockData();
            }
        }
    }
    _GL_CHECK_ERROR_
    rLock.unlock();
}


