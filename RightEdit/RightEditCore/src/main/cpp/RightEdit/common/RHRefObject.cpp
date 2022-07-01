
#include "RHRefObject.h"

RHRefObject::RHRefObject(): mRef(1) {

}

RHRefObject::~RHRefObject() {
    mRef = 0;
}

int RHRefObject::count() {
    return mRef;
}

int RHRefObject::retain() {
    ++mRef;
    return mRef;
}

int RHRefObject::release() {
    --mRef;
    if(mRef == 0) {
        delete this;
        return 0;
    }
    return mRef;
}