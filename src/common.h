#ifndef COMMON_H
#define COMMON_H

#define CLASSNAME(name) \
QString name::className() const { \
    return #name; \
}

#endif
