/*
 * A gentle introduction to Android reference counting mechanism by example
 *
 * copyright http://pierrchen.blogspot.com/
 *
 */

#define LOG_TAG "andrid-sp-demo"

#include <utils/RefBase.h>
#include <utils/Log.h>
#include <cstdlib>
#include <cassert>

using namespace android;

// class Memory subclass RefBase[1] so it can be reference counted
// and be accepted by template class sp<T> [2], where the sp stands
// for strong pointer
// [1]https://android.googlesource.com/platform/frameworks/native/+/jb-mr1-dev/include/utils/RefBase.h
// [2]https://android.googlesource.com/platform/frameworks/native/+/jb-mr1-dev/include/utils/StrongPointer.h

class Memory: public RefBase {
public:
    Memory(int size) : mSize(size), mData(NULL) {
        ALOGD("        Memory constructor %p ",this);
    }

    virtual ~Memory() {
        ALOGD("        Memory destructor %p", this);
        if (mData)  free(mData);
    }

    virtual void onFirstRef() {
        ALOGD("        onFirstRef on %p",this);
        mData = malloc(mSize);
    }

    virtual void onLastStrongRef(const void * /*id*/) {
        ALOGD("        onLastStrongRef");

    }

    int size() {return mSize;}
private:
    int mSize;
    void *mData;
};

// used as a MARK in the output
#define L(N)   ALOGD("LINE %d TRIGGER:",N);
// print out the strong counter numbers of the object
#define C(obj) ALOGD("        Count of %p : %x", (void*)obj, obj->getStrongCount());

int main()
{
    {
        L(1)
        Memory *m = new Memory(1);
        C(m)

        L(2)
        m->incStrong(m);
        C(m)

        L(4)    
        sp<Memory> spm1(m) ;
        sp<Memory> spm2(spm1);
        // ref count should be 2
        C(m)
        
        L(5)
        spm1.clear();
        // ref count should be 1
        C(m)
        
        L(6)
        spm2.clear();
        C(m)
        
        L(3)
        m->decStrong(m);
        // ref count should be 0
        // this will trigger onLastStrongRef and Memory
        // destructor 
        C(m)
       
        L(7)    
    }
    L(-1)
    return 0;
}
