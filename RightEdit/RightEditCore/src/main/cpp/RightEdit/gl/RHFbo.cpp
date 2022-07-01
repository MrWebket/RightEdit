//
// Created by macbookpro on 4/23/21.
//

#include "../common/RHAppDef.h"
#include "RHFbo.h"
#include "RHGLTexture.h"

RHFbo::RHFbo(RHApplication *_app) : RAppObject(_app),
                                    mFrameWidth(0),
                                    mFrameHeight(0),
                                    mTexture(nullptr),
                                    mSecondTexture(nullptr),
                                    mSceneName(""),
                                    mFboName(""),
                                    m_fboID(static_cast<unsigned int>(GL_DEF_CODE)),
                                    mUseDepth(false),
                                    mUseStencil(false),
                                    mUseSwap(false) {
}

RHFbo::~RHFbo() {
    if (mTexture) {
        mTexture->release();
        mTexture = nullptr;
    }
    if (mSecondTexture) {
        mSecondTexture->release();
        mSecondTexture = nullptr;
    }
    disableDepthBuffer();
    if (m_fboID != GL_DEF_CODE) {
        glDeleteFramebuffers(1, &m_fboID);
        m_fboID = static_cast<unsigned int>(GL_DEF_CODE);
    }
}

void RHFbo::createFbo(const char *_name, int _width, int _height) {
    if (_width == 0 || _height == 0)
        return;
    mFrameWidth = _height;
    mFrameHeight = _width;

    if (m_fboID == GL_DEF_CODE) {
        if (mTexture) {
            mTexture->release();
        }
        mTexture = RHGLTexture::createTexture(_name, _width, _height);
        mTexture->setIsLoad(true);
        _GL_CHECK_ERROR_
        glGenFramebuffers(1, &m_fboID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                               mTexture->getTexId(), 0);

        setFboName(_name);
        _GL_CHECK_ERROR_
        //开启深度
        enableDepthBuffer();
        //开启模板
        enableStencilBuffer();
    }
}

void RHFbo::createFbo(const char *_name, int _texId, int _width, int _height) {
    if (_width == 0 || _height == 0)
        return;

    setFrameHeight(_height);
    setFrameWidth(_width);
    if (m_fboID == GL_DEF_CODE) {
        if (mTexture) {
            mTexture->release();
        }
        mTexture = RHGLTexture::createExtTexture(_name,
                                                 _texId,
                                                 getFrameWidth(),
                                                 getFrameHeight());

        glGenFramebuffers(1, &m_fboID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                               mTexture->getTexId(), 0);

        setFboName(_name);
        ALOGI("fbo texId = %d,m_fboID = %d \n", mTexture->getTexId(), m_fboID);
    }
}

bool RHFbo::enableDepthBuffer() {
    if (m_fboID == 0) {
        return false;
    }
    if (mUseDepth) {
        disableDepthBuffer();
    }
    glGenRenderbuffers(1, &mDepthID);
    glBindRenderbuffer(GL_RENDERBUFFER, mDepthID);
#if defined(GL_OES_packed_depth_stencil)
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, mFrameWidth, mFrameHeight);
#else
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_FrameWidth, m_FrameHeight);
    //GL_DEPTH_COMPONENT16
#endif
    _GL_CHECK_ERROR_
    //
    glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthID);
    _GL_CHECK_ERROR_
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
        mUseDepth = true;
        return true;
    }
    _GL_CHECK_ERROR_
    unbind();

    return false;
}

bool RHFbo::enableStencilBuffer() {
    if (mDepthID) {
        mUseStencil = true;
        glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthID);
        _GL_CHECK_ERROR_
        return true;
    }
    return false;
}

void RHFbo::disableDepthBuffer() {
    //没有深度 就没有模板 所以 一定先关闭模板 在关闭深度
    disableStencilBufer();
    if (mDepthID && mUseDepth) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
        unbind();
        glDeleteRenderbuffers(1, &mDepthID);
        mDepthID = 0;
    }
    _GL_CHECK_ERROR_
    mUseDepth = false;
}

void RHFbo::disableStencilBufer() {
    if (mDepthID && mUseStencil) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
        _GL_CHECK_ERROR_
        unbind();
    }
    mUseStencil = false;
}

bool RHFbo::enableSwapBuffer() {
    //开启第二张纹理
    if (mUseSwap) {
        if (mSecondTexture) {
            mSecondTexture->release();
            mSecondTexture = nullptr;
        }
    }

    std::string _name = getSceneName() + getFboName() + "_second";
    mSecondTexture = RHGLTexture::createTexture(_name.c_str(), getFrameWidth(),
                                                getFrameHeight());
    mSecondTexture->setIsLoad(true);
    mUseSwap = true;
    return true;
}

void RHFbo::swap() {
    if (mTexture && mSecondTexture) {
        unsigned int tmpid = mTexture->getTexId();
        mTexture->setTexId(mSecondTexture->getTexId());
        mSecondTexture->setTexId(tmpid);

        glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                               static_cast<GLuint>(mTexture->getTexId()), 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {

        }
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
}

void RHFbo::bind() {
    if (m_fboID != GL_DEF_CODE) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);
        glViewport(0, 0, mFrameWidth, mFrameHeight);
        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        _GL_CHECK_ERROR_
    }
}

void RHFbo::bindNoClear() {
    if (m_fboID != GL_DEF_CODE) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);
        glViewport(0, 0, mFrameWidth, mFrameHeight);
        _GL_CHECK_ERROR_
    }
}

void RHFbo::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    _GL_CHECK_ERROR_
}

unsigned int RHFbo::getTexId() {
    return mTexture->getTexId();
}

RHGLTexture *RHFbo::getMainTexture() {
    return mTexture;
}

RHGLTexture *RHFbo::getSecondTexture() {
    return mSecondTexture;
}

unsigned int RHFbo::getFboId() {
    return m_fboID;
}

void RHFbo::setFboId(unsigned int _fboId) {
    m_fboID = _fboId;
}
