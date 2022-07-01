//
// Created by macbookpro on 4/23/21.
//

#ifndef LIBV6LIVE_SRAPPOBJECT_H
#define LIBV6LIVE_SRAPPOBJECT_H


#include "../app/RHApplication.h"
#include "RHRefObject.h"

class RAppObject: public RHRefObject {
public:
    RAppObject(RHApplication * _app);
    virtual ~RAppObject();

protected:
    RHApplication * mApp;
};

#endif //LIBV6LIVE_SRAPPOBJECT_H
