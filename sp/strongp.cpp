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
// and be accepted by template class sp<T> [2], where the sp stands for
// strong pointer
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

    int size() {return mSize;}
private:
    int mSize;
    void *mData;
};

// used as a MARK in the output so that you can associate the output with the line of code
#define L(N)   ALOGD("LINE %d TRIGGER:",N);
// print out the strong counter number of the object
#define C(obj) ALOGD("        Count of %p : %d", (void*)obj, obj->getStrongCount());

int main()
{
    {
        // create a Memory instance and assign it to a raw pointer
        L(1)
        Memory *m1 = new Memory(4);
        // create a strong pointer, using constructor sp(T* other),
        // which will increase m1's reference counter by 1, and
        // call m1::onFirstRef, where you can do the lazy initialzation
        L(2)
        sp<Memory> spm1 = m1;
        C(m1);

        // usually, we will combine previous two steps into one single statement.
        // create another strong pointer, spm2, and initialize it.
        // To get the raw object, use sp<T>::get()
        L(3)
        sp<Memory> spm2 = new Memory(128);
        Memory *m2 = spm2.get();
        // to access the method, use sp as if you are working with raw pointer
        int size = spm2->size();

        // create a third strong pointer, spm3, using construcotr sp(const sp<T>& other),
        // which will increase the reference counter pointed by spm1 by 1.
        // now, m1 is pointed by two strong pointers, spm1 and spm3
        L(4)
        sp<Memory> spm3 = spm1;
        C(m1);

        // below are same as L(4), except that the scope of spm4 is within the block
        L(5)
        {
            sp<Memory> spm4 = spm1;
            C(m1);
            // at this point, m1 is pointed by spm1, spm3 and spm4
        }

        // beyond this point, spm4 is destructed and no longer point to m1
        // so, the reference of m1 is still 2, i,e pointed by spm1, spm3
        L(6)
        C(m1);

        // trigger sp& operator = (const sp<T>& other);
        L(7)
        // before the assigment, spm2 pointed to m2 and spm3 pointed to m1
        spm3 = spm2;
        // after the assigment, spm3 will no longer point to m1 but m2.
        // so the reference counter of m1 decrease by 1 and the reference counter
        // of m2 increase by 1.
        // now , m1 is pointed by spm1 m2 is pointed by spm2 and spm3
        C(m1);
        C(m2);

        // spm5 is a reference to spm1, no new strong object is created.
        // so reference count of m1 stays the same
        L(8)
        sp<Memory> &spm5 = spm1;
        C(m1);

        // we can also create a smart pointer pointing to nothing at first
        // and later assign it a value. We can also remove the reference explictly
        // by calling sp::clear()
        L(9)
        sp<Memory> spm6;
        assert(spm6.get() == NULL);
        spm6 = spm1;
        C(m1);
        L(10)
        spm6.clear();
        assert(spm6.get() == NULL);
        C(m1);

    }
    // beyond the close curly, all the smart pointer objects are out of
    // scope, so they will be desctructed and cause the reference counter
    // of its managed object decrease by 1.
    // For example, when spm1 destructed, the reference count of m1 became 0
    // and it will trigger the destructor of m1.
    L(-1)
    return 0;
}
