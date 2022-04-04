#ifndef CUUSERDATA_H
#define CUUSERDATA_H

class CuUserData {
public:
    virtual ~CuUserData() {}
    virtual int type() const = 0;
};

#endif // CUUSERDATA_H
