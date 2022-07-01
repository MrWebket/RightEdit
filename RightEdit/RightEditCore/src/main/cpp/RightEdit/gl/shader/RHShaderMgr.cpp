//
// Created by macbookpro on 4/23/21.
//

#include "../../common/RHAppDef.h"
#include "RHShaderMgr.h"
#include "normal2d.h"
#include "RHGLProgram.h"
#include "../../app/RHApplication.h"
#include "nv21rgba.h"
#include "nv12rgba.h"
#include "I420rgba.h"

RHShaderMgr::RHShaderMgr(RHApplication *_app): RAppObject(_app) {
    mShaderMap.clear();
    mProgramMap.clear();
}

RHShaderMgr::~RHShaderMgr() {
    ALOGI("Release ShaderMgr\n");
    mShaderMap.clear();
}


RHGLProgram* RHShaderMgr::getProgram(const char *name) {
    auto it = mProgramMap.find(name);
    if(it != mProgramMap.end()) {
        return it->second;
    }

    auto shaderIt = mShaderMap.find(name);
    if(shaderIt != mShaderMap.end()) {
        RShader shader = shaderIt->second;

        auto *  _pProgram = new RHGLProgram(shader.m_vs.c_str(), shader.m_fs.c_str());
//        LOG_DEBUG("program name = %s, id = %d", name, tProgram->getProgram());
        mProgramMap.insert(std::make_pair(name, _pProgram));

        return _pProgram;
    }
    return nullptr;
}

void RHShaderMgr::deleteProgram(const char *name) {
    auto it = mProgramMap.find(name);
    if(it != mProgramMap.end()) {
        RHGLProgram * _program = it->second;
        if(_program) {
            _program->release();
            mProgramMap.erase(it);
        }
    }
}

void RHShaderMgr::insertProgramme(const char *key, const char *vs, const char *fs) {
    if(vs && fs) {
        auto iter = mShaderMap.find(key);
        if(iter == mShaderMap.end()) {
            mShaderMap.insert(std::make_pair(key, RShader(vs, fs)));
        }
    }
}

void RHShaderMgr::init() {
    mShaderMap.insert(std::make_pair("normal2d", RShader(vs_normal_2d, fs_normal_2d)));
    mShaderMap.insert(std::make_pair("normal2d_alpha", RShader(vs_normal_2d, fs_normal_2d_alpah)));
    mShaderMap.insert(std::make_pair("normal2d_vap", RShader(vs_normal_2d, fs_normal_2d_vap)));

    mShaderMap.insert(std::make_pair("nv21rgba", RShader(nv21rgba_vs, nv21rgba_fs)));
    mShaderMap.insert(std::make_pair("nv21rgba_alpha", RShader(nv21rgba_vs, nv21rgba_alpha_fs)));
    mShaderMap.insert(std::make_pair("nv21rgba_vap", RShader(nv21rgba_vs, nv21rgba_vap_fs)));

    mShaderMap.insert(std::make_pair("nv12rgba", RShader(nv12rgba_vs, nv12rgba_fs)));
    mShaderMap.insert(std::make_pair("nv12rgba_alpha", RShader(nv12rgba_vs, nv12rgba_alpha_fs)));
    mShaderMap.insert(std::make_pair("nv12rgba_vap", RShader(nv12rgba_vs, nv12rgba_vap_fs)));

    mShaderMap.insert(std::make_pair("i420rgba", RShader(i420_rgb_vs, i420_rgb_fs)));
    mShaderMap.insert(std::make_pair("i420rgba_alpha", RShader(i420_rgb_vs, i420_alpha_fs)));
    mShaderMap.insert(std::make_pair("i420rgba_vap", RShader(i420_rgb_vs, i420_vap_fs)));
}

void RHShaderMgr::destroy() {
    mShaderMap.clear();

    auto it = mProgramMap.begin();
    while (it != mProgramMap.end()) {
        RHGLProgram * tProgram = it->second;
        if(tProgram) {
            tProgram->release();
        }
        it++;
    }
    mProgramMap.clear();
}
