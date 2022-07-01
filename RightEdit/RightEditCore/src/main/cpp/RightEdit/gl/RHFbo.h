//
// Created by macbookpro on 4/23/21.
//

#ifndef LIBV6LIVE_SRFBO_H
#define LIBV6LIVE_SRFBO_H

#include <string>
#include "../app/RHApplication.h"
#include "../common/RAppObject.h"

class RHGLTexture;
class RHFbo: public RAppObject {
public:
    RHFbo(RHApplication * _app);
    virtual ~RHFbo();

    void createFbo(const char *_name, int _width, int _height);
    void createFbo(const char *_name, int _texId, int _width, int _height);

    unsigned int getFboId();

    void setFboId(unsigned int _fboId);

    unsigned int getTexId();

    RHGLTexture *getMainTexture();
    RHGLTexture *getSecondTexture();

    //开启深度
    virtual bool enableDepthBuffer();

    //开启模板
    virtual bool enableStencilBuffer();

    virtual void disableDepthBuffer();

    virtual void disableStencilBufer();

    //是否开启第二张纹理
    virtual bool enableSwapBuffer();

    virtual void swap();

    void bind();
    void bindNoClear();
    void unbind();

    DEFINE_PARAM_NOR(int, FrameWidth);
    DEFINE_PARAM_NOR(int, FrameHeight);
    DEFINE_PARAM_NOR(std::string, SceneName);
    DEFINE_PARAM_NOR(std::string, FboName);

protected:
    bool                  mUseDepth;
    bool                  mUseStencil;
    bool                  mUseSwap;
    unsigned int          m_fboID;
    unsigned int          mDepthID;

    RHGLTexture         * mTexture;
    RHGLTexture         * mSecondTexture;

};
#endif //LIBV6LIVE_SRFBO_H
