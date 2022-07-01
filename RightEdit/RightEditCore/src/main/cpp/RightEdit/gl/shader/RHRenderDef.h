//
// Created by Hope liao on 2019/2/28.
//

#ifndef RIGHTHELPER_RRENDERDEF_H
#define RIGHTHELPER_RRENDERDEF_H

#define CHANNEL_POSITION    0
#define CHANNEL_TEXCOORD0   1
#define CHANNEL_TEXCOORD1   7
#define CHANNEL_TEXCOORD2   8
#define CHANNEL_COLOR       2
#define CHANNEL_NORMAL      3
#define CHANNEL_MORPH       4
#define CHANNEL_BONE        5
#define CHANNEL_WEIGHT      6

//内置变量
#define NAME_POSITION       "aPosition"
#define NAME_TEXCOORD0      "aTexcoord0"
#define NAME_TEXCOORD1      "aTexcoord1"
#define NAME_TEXCOORD2      "aTexcoord2"
#define NAME_COLOR0         "aColor"
#define NAME_NORMAL         "aNormal"
#define NAME_MORPH          "aMorph"
#define NAME_BONE           "aBoneIndex"
#define NAME_WEIGHT         "aWeight"

#define NAME_U_COLOR        "uColor"
#define NAME_U_BONE_MAT     "uBoneMat"

#define NAME_TEXCOORD_CLIP  "texcoordClip"
#define NAME_M_MATRIX       "m_matrix" //模型矩阵
#define NAME_V_MATRIX       "v_matrix" //相机
#define NAME_P_MATRIX       "p_matrix" //裁切矩阵
#define NAME_TEX0           "utexture0"
#define NAME_TEX1           "utexture1"
#define NAME_TEX2           "utexture2"
#define NAME_TEX3           "utexture3"

#endif //RIGHTHELPER_RRENDERDEF_H
