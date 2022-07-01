//
// Created by macbookpro on 4/23/21.
//

#include "RAppObject.h"
#include "../app/RHApplication.h"

RAppObject::RAppObject(RHApplication * _app): mApp(_app){

}
RAppObject::~RAppObject() {
    mApp = nullptr;
}

