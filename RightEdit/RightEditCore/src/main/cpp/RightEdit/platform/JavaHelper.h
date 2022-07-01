//
// Created by Hope liao on 2022/6/30.
//

#ifndef RIGHTEDIT_JAVAHELPER_H
#define RIGHTEDIT_JAVAHELPER_H

void BindJavaJVM(void *env);
void BindJavaVM(void *vm);

JNIEnv * GetEnv();

void DetachThreadEnv();
#endif //RIGHTEDIT_JAVAHELPER_H
