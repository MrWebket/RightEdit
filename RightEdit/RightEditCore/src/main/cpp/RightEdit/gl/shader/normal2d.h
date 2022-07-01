//
// Created by Hope liao on 2019/3/8.
//

#ifndef RIGHTHELPER_NORMAL2D_H
#define RIGHTHELPER_NORMAL2D_H


const char * vs_normal_2d =
        "#version 300 es\n"
        "precision highp float;\n"
        "layout(location = 0) in vec2 aPosition;\n"
        "layout(location = 1) in vec2 aTexcoord0;\n"
        " uniform vec2 texcoordClip;\n"
        "uniform mat4 m_matrix;\n"
        "out vec2 vTexcoord0;\n"
        "void main()\n"
        "{\n"
        " gl_Position =  m_matrix * vec4(aPosition, 0.0, 1.0);\n"
        "  vTexcoord0 = (aTexcoord0 - 0.5)*texcoordClip + 0.5;;\n"
        "}\n";


const char *fs_normal_2d =
        "#version 300 es\n"
        "precision highp float;\n"
        "uniform sampler2D utexture0;\n"
        "in vec2 vTexcoord0;\n"
        "out vec4 outColor;\n"
        "void main()\n"
        "{\n"
        "    outColor = texture(utexture0, vTexcoord0);\n"
        "}\n";

const char *fs_normal_2d_alpah =
        "#version 300 es\n"
        "precision highp float;\n"
        "uniform sampler2D utexture0;\n"
        "in vec2 vTexcoord0;\n"
        "out vec4 outColor;\n"
        "void main()\n"
        "{\n"
        "        vec2 alphaCoord = vec2(vTexcoord0.x * 0.5, vTexcoord0.y);\n"
        "        vec2 rgbCoord = vec2(vTexcoord0.x * 0.5 + 0.5, vTexcoord0.y);\n"
        "        outColor.rgb = texture(utexture0, rgbCoord);\n"
        "        outColor.a = texture(utexture0, alphaCoord);\n"
        "}\n";

const char *fs_normal_2d_vap =
        "#version 300 es\n"
        "precision highp float;\n"
        "uniform sampler2D utexture0;\n"
        "uniform vec4 uAlphaRect;\n"
        "uniform vec4 uRGBRect;\n"
        "in vec2 vTexcoord0;\n"
        "out vec4 outColor;\n"
        "void main()\n"
        "        vec2 alphaCoord = vec2("
        "                                  vTexcoord0.x * (uAlphaRect.z - uAlphaRect.x) + uAlphaRect.x, "
        "                                  vTexcoord0.y * (uAlphaRect.w - uAlphaRect.y) + uAlphaRect.y"
        "                              );\n"
        "        vec2 rgbCoord = vec2("
        "                                  vTexcoord0.x * (uRGBRect.z - uRGBRect.x) + uRGBRect.x, "
        "                                  vTexcoord0.y * (uRGBRect.w - uRGBRect.y) + uRGBRect.y"
        "                             );\n"
        "        outColor.rgb = texture(utexture0, rgbCoord);\n"
        "        outColor.a = texture(utexture0, alphaCoord);\n"
        "}\n";

static const  char * normal_vs =
        "attribute vec2 aPosition;\n "
        "attribute vec2 aTexcoord0;\n"
        " uniform vec2 texcoordClip;\n"
        "uniform mat4 m_matrix;\n"
        "uniform mat4 v_matrix;\n"
        "uniform mat4 p_matrix;\n"
        "varying vec2 sTextureCoordinate;\n"
        "void main() {\n"
        "gl_Position =  vec4(aPosition, 0.0, 1.0);\n"
        "sTextureCoordinate = (aTexcoord0 - 0.5)*texcoordClip + 0.5;\n"
        "}\n";

static const   char * normal_fs =
        " precision mediump float;\n"
        "varying vec2 sTextureCoordinate;\n"
        "uniform sampler2D utexture0;\n"
        "void main() {\n"
        "lowp vec4 color = texture2D(utexture0, sTextureCoordinate);\n"
        "gl_FragColor = color;\n"
        "}";


#endif //RIGHTHELPER_NORMAL2D_H
