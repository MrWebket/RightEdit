//
// Created by macbookpro on 4/23/21.
//

#ifndef LIBV6LIVE_SRSHADERMGR_H
#define LIBV6LIVE_SRSHADERMGR_H


#include "../../common/RAppObject.h"
#include <string>
#include <map>

class RHGLProgram;

struct RShader {
    RShader() : m_vs(""), m_fs("") {}

    RShader(std::string vs, std::string fs) : m_vs(vs), m_fs(fs) {}

    std::string m_vs;
    std::string m_fs;
};

class RHShaderMgr: public RAppObject {
public:
    RHShaderMgr(RHApplication * _app);
    ~RHShaderMgr();

    virtual void init();
    virtual void destroy();

    RHGLProgram * getProgram(const char * name);
    void deleteProgram(const char * name);

    void insertProgramme(const char * key, const char * vs, const char * fs);

private:
    typedef std::map<std::string,RShader> SHADER_MAP;
    SHADER_MAP mShaderMap;

    typedef std::map<std::string, RHGLProgram *> PROGRAM_MAP;
    PROGRAM_MAP mProgramMap;
};
#endif //LIBV6LIVE_SRSHADERMGR_H
