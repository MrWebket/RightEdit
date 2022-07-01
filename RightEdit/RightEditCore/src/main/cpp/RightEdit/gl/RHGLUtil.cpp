//
// Created by Hope liao on 2019/2/21.
//

#include "../common/RHAppDef.h"
#include "RHGLUtil.h"


unsigned int RHGLUtil::makeTexture2D(int width, int height, unsigned int _internalformat,
                                     unsigned int _format, void *_pdata, int _dataType,
                                     bool _enableMipMaps) {
    return RHGLUtil::makeTexture2D(width, height, _internalformat, _format, GL_CLAMP_TO_EDGE,
                                   GL_CLAMP_TO_EDGE,
                                   GL_LINEAR, GL_LINEAR, _pdata, _dataType, _enableMipMaps);
}

unsigned int RHGLUtil::makeTexture2D(int width, int height, unsigned int _internalformat,
                                     unsigned int _format, int wrapS, int wrapT, int minFilter,
                                     int magFilter, void *_pdata, int _dataType,
                                     bool _enableMipMaps) {
    unsigned int m_texid;
    glGenTextures(1, &m_texid);
    glBindTexture(GL_TEXTURE_2D, m_texid);
    glTexImage2D(GL_TEXTURE_2D, 0, _internalformat, width, height, 0, _format,
                 static_cast<GLenum>(_dataType), _pdata);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    if (_enableMipMaps) {
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D,
                        GL_TEXTURE_MIN_FILTER, minFilter);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    }
    return m_texid;
}


void RHGLUtil::subTextureBuffer(unsigned int target, unsigned int format, unsigned int texId,
                                int texWidth,
                                int texHeight, int _dataFormat, void *_buff) {
    glBindTexture(target, texId);
    glTexSubImage2D(target, 0, 0, 0,
                    texWidth, texHeight,
                    static_cast<GLenum>(format),
                    static_cast<GLenum>(_dataFormat),
                    _buff);
}

void RHGLUtil::deleteTexture(unsigned int textureId) {
    glDeleteTextures(1, &textureId);
}
