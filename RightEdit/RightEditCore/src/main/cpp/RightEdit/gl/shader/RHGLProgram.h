//
// Created by macbookpro on 4/23/21.
//

#ifndef LIBV6LIVE_SRGLPROGRAM_H
#define LIBV6LIVE_SRGLPROGRAM_H

#include <string>
#include "../../common/RHRefObject.h"

class RHGLProgram: public RHRefObject {
public:
    RHGLProgram(const char *vs, const char *fs);
    virtual ~RHGLProgram();

    void useProgram();
    bool verifyProgram();

    unsigned int getProgram() {
        return program;
    }

protected:
    unsigned int compileShader(unsigned int shaderType, const char *shaderName);
    unsigned int createProgram(unsigned int vs, unsigned int fs);

    unsigned int vs_id;
    unsigned int fs_id;
    unsigned int program;

    std::string vs;
    std::string fs;
};

#endif //LIBV6LIVE_SRGLPROGRAM_H
