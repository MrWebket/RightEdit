//
// Created by macbookpro on 4/23/21.
//

#ifndef LIBV6LIVE_SRGLTEXTURE_H
#define LIBV6LIVE_SRGLTEXTURE_H

#include "../common/RHRefObject.h"
#include <string>
#include "../common/RHLock.h"

class RHSafeDataSwap;
class RHGLTexture: public RHRefObject {

public:
    static RHGLTexture * createExtTexture(const char *_name, int _tex, int _w, int _h);
    static RHGLTexture * createTexture(const char *name, int width, int height);
    static RHGLTexture * createTexture(const char *name, int width, int height, int _informat, int _dataFormat, int _texDataType);
public:
    RHGLTexture();

    virtual ~RHGLTexture() override;

    void apply();
    virtual void apply(void *data);

    virtual void putData(void *data, int len);

    virtual void commit();

DEFINE_PARAM_NOR(int, TexHeight);
DEFINE_PARAM_NOR(int, TexWidth);
DEFINE_PARAM_NOR(int, IsLoad);
DEFINE_PARAM_NOR(std::string, TexName);
DEFINE_PARAM_NOR(unsigned int, TexId);
DEFINE_PARAM_NOR(bool, IsExt);
DEFINE_PARAM_NOR(bool, EnableMipMaps);
DEFINE_PARAM_NOR(unsigned int, InFormat);
DEFINE_PARAM_NOR(unsigned int, DataFormat);
DEFINE_PARAM_NOR(unsigned int, TexDataType);
DEFINE_PARAM_NOR(int, WrapS);
DEFINE_PARAM_NOR(int, WrapT);
DEFINE_PARAM_NOR(int, MagFilter);
DEFINE_PARAM_NOR(int, MinFilter);
private:
    unsigned int m_target;

    RHSafeDataSwap *m_pDataSwap;

    RHLock rLock;

    bool mNeedUpdate;

};
#endif //LIBV6LIVE_SRGLTEXTURE_H
