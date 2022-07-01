//
// Created by macbookpro on 4/23/21.
//

#include "RHGLProgram.h"
#include "../../common/RHAppDef.h"
#include "RHGLProgram.h"
#include "RHRenderDef.h"

RHGLProgram::RHGLProgram(const char *vs, const char *fs) :
        RHRefObject(), vs(vs), fs(fs) {
    _GL_CHECK_ERROR_
    vs_id = compileShader(GL_VERTEX_SHADER, vs);
    _GL_CHECK_ERROR_
    fs_id = compileShader(GL_FRAGMENT_SHADER, fs);
    _GL_CHECK_ERROR_
    program = createProgram(vs_id, fs_id);
    _GL_CHECK_ERROR_
}

RHGLProgram::~RHGLProgram() {
    if (fs_id != GL_DEF_CODE) {
        glDeleteShader(fs_id);
        _GL_CHECK_ERROR_
    }
    if (vs_id != GL_DEF_CODE) {
        glDeleteShader(vs_id);
        _GL_CHECK_ERROR_
    }
    if (program != GL_DEF_CODE) {
        glDeleteProgram(program);
        _GL_CHECK_ERROR_
    }
}

unsigned int RHGLProgram::compileShader(unsigned int shaderType, const char *shaderName) {
    const char *shader = shaderName;

    unsigned int shaderID = glCreateShader(shaderType);
    glShaderSource(shaderID, 1, &shader, 0);
    glCompileShader(shaderID);
    _GL_CHECK_ERROR_
    GLint compileErr = 0;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileErr);
    if (GL_FALSE == compileErr) {
        GLint logLen;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLen);
        if (logLen > 0) {
            char *log = (char *) malloc(static_cast<size_t>(logLen));
            GLsizei written;
            glGetShaderInfoLog(shaderID, logLen, &written, log);

            ALOGE("shader error in program: %s\n", shaderName);
            ALOGE("shader error: \n %s", log);

            free(log);
        }
        shaderID = 0;
    } else {
//        LOG_INFO("shader = %s \ncompile succ", shaderName);
    }
    return shaderID;
}

unsigned int RHGLProgram::createProgram(unsigned int vs, unsigned int fs) {
    unsigned int t_program = glCreateProgram();
    _GL_CHECK_ERROR_

    glAttachShader(t_program, vs);
    _GL_CHECK_ERROR_

    glAttachShader(t_program, fs);
    _GL_CHECK_ERROR_

    //vbo 绑定序号
    glBindAttribLocation(t_program, CHANNEL_POSITION, NAME_POSITION);
    _GL_CHECK_ERROR_
    glBindAttribLocation(t_program, CHANNEL_TEXCOORD0, NAME_TEXCOORD0);
    _GL_CHECK_ERROR_
    glBindAttribLocation(t_program, CHANNEL_TEXCOORD1, NAME_TEXCOORD1);
    _GL_CHECK_ERROR_
    glBindAttribLocation(t_program, CHANNEL_TEXCOORD2, NAME_TEXCOORD2);
    _GL_CHECK_ERROR_
    glBindAttribLocation(t_program, CHANNEL_COLOR, NAME_COLOR0);
    _GL_CHECK_ERROR_
    glBindAttribLocation(t_program, CHANNEL_NORMAL, NAME_NORMAL);
    _GL_CHECK_ERROR_
    glBindAttribLocation(t_program, CHANNEL_MORPH, NAME_MORPH);
    _GL_CHECK_ERROR_
    glBindAttribLocation(t_program, CHANNEL_BONE, NAME_BONE);
    _GL_CHECK_ERROR_
    glBindAttribLocation(t_program, CHANNEL_WEIGHT, NAME_WEIGHT);

    glLinkProgram(t_program);
    _GL_CHECK_ERROR_
    GLint linkstatus;
    glGetProgramiv(t_program, GL_LINK_STATUS, &linkstatus);
    if (linkstatus == GL_FALSE) {
        GLint logLen = 1000;
        if (logLen > 0) {
            char *log = (char *) malloc(static_cast<size_t>(logLen));
            GLsizei written;
            glGetProgramInfoLog(t_program, logLen, &written, log);
            ALOGE("createProgram faild:%s\n", log);
            free(log);
        }
    } else {
        glUseProgram(t_program);
    }
    _GL_CHECK_ERROR_
    return t_program;
}

void RHGLProgram::useProgram() {
    if (program) {
        glUseProgram(program);
        _GL_CHECK_ERROR_
    }
}

bool RHGLProgram::verifyProgram() {
    return program > 0;
}
