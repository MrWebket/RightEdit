#ifndef V6CORESDK_RREFOBJECT_H
#define V6CORESDK_RREFOBJECT_H

#define DEFINE_PARAM_NOR(type, param) \
protected:\
type m##param;\
public:\
type get##param(){ return m##param; }\
void set##param(type _##param){ m##param = _##param; };


class RHRefObject {
public:
    RHRefObject();
    virtual ~RHRefObject();

public:
    int count();
    int retain();
    int release();

protected:
    int mRef;
};


#endif //V6CORESDK_RREFOBJECT_H