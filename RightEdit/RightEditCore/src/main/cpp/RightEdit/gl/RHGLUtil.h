//
// Created by Hope liao on 2019/2/21.
//

#ifndef RIGHTHELPER_RGLUTIL_H
#define RIGHTHELPER_RGLUTIL_H

class RHGLUtil {
public :
    static unsigned int
    makeTexture2D(int width, int height, unsigned int _internalformat, unsigned int _format, void *_pdata,int _dataType,
                  bool _enableMipMaps = false);

    static unsigned int
    makeTexture2D(int width, int height, unsigned int _internalformat, unsigned int _format,  int wrapS, int wrapT, int minFilter,
                  int magFilter, void *_pdata,int _dataType,
                  bool _enableMipMaps = false);


    static void
    subTextureBuffer(unsigned int target, unsigned int format, unsigned int texId, int texWidth,
                     int texHeight, int _dataFormat, void *_buff);

    static void deleteTexture(unsigned int textureId);
};

#endif //RIGHTHELPER_RGLUTIL_H
