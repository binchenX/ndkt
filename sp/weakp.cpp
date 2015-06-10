/*
 * A gentle introduction to Android reference counting mechanism by example
 *
 * copyright http://pierrchen.blogspot.com/
 *
 */

#define LOG_TAG "andrid-wp-demo"

#include <utils/RefBase.h>
#include <utils/Log.h>
#include <cstdlib>
#include <cassert>

using namespace android;

// class Memory subclass RefBase so it can be reference counted
// and be accepted by template class wp<T>, where the wp stands
// for weak pointer. A weak pointer won't prevent the object it points
// to from being deleted, and a dedicated api will be used to test if
// the underlying object still available before actually using it.

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

    int size() {return mSize;}
private:
    int mSize;
    void *mData;
};

// used as a MARK in the output
#define L(N)   ALOGD("LINE %d TRIGGER:",N);
// print out the strong counter numbers of the object
#define C(obj) ALOGD("        Count of %p : %d", (void*)obj.get(), obj->getStrongCount());

int main()
{
    wp<Memory> wpm1;
    {
        // create an Memory instance and let it managed by a sp
        L(1)
        sp<Memory> spm1 = new Memory(1);
        // assign the sp to the weak pointer, wpm1
        wpm1 = spm1;

        // you can't access wp's underlying raw pointer, since
        // there is no accessor apis defined for wp
        // (recall that for sp, there are three accessor api/operators
        // you can use: get(),*,->).
        // To access the raw pointer the wp pointing to, you need first
        // promote it to an sp, and use the returning sp if it is not NULL
        sp<Memory> spm2 = wpm1.promote();
        if (spm2 != NULL) {
            L(2)
            ALOGD("        Promotion to sp successfully");
            spm2->size();
            // should be two, since both spm1 and spm2 pointed to it
            C(spm2)
        }
    }

    // The promotion will fail, when the object managed by the wp
    // has already being desctructed since there is no strong reference to it.
    // Note that the weak reference to the object won't prevent the object
    // being pointed to from being desctructed, that is the exact reason why
    // it is called week reference.
    L(3)
    // Note that at the point, the two strong pointer are both out of scope.
    // So the object they pointing to (which is the same as wpm1 points to)
    // are already desctructed, so the promote will fail.
    sp<Memory> spm3 = wpm1.promote();
    if (spm3 == NULL) {
        ALOGD("        Promotion Fail. Objecte already being destructed");
    }

    L(-1)
    return 0;
}
