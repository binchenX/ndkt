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
// and be accepted by both sp<T> and wp<T>, where sp stands for
// strong pointer and wp stands for weak pointer. 
// A weak pointer won't prevent the object it points to from being 
// deleted - as long as there is no strong pointer pointing to the 
// raw object it will be deleted. Hence, to make sure the raw object
// is still valid, you will first need to promote it to be strong 
// pointer first. If the promote success, use it; otherwise, you know
// underlying object is no longer valid and you won't want to use it.  

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
#define C(obj) ALOGD("        Count of %p : %d", (void*)obj, obj->getStrongCount());

int main()
{
    wp<Memory> wpm1;
    Memory *m = new Memory(1);
    {//scope for spm1
        // create a Memory instance and let it be managed by a sp
        L(1)
        sp<Memory> spm1(m); 
        // assign the sp to the weak pointer, wpm1
        wpm1 = spm1;

        { // scope for spm2
            // You can't access wp's underlying raw pointer directly, since
            // there is no accessor apis defined for it.
            // Recall that for sp, there are three accessor apis/operators
            // you can use: get(),*,->
            // To access the raw pointer the wp pointing to, you need first
            // promote it to an sp, and use the returning sp if it is not NULL.
            sp<Memory> spm2 = wpm1.promote();
            if (spm2 != NULL) {
                L(2)
                ALOGD("        Promotion to sp successfully");
                spm2->size();
                // ref count is 2, since both spm1 and spm2 pointed to it
                C(m)
            }
        }
        // at this point, spm2 is out of scope, ref count of the m becomes 1 
    }
    // Beyond previouse close curly, spm1 was also out of scope.
    // Since there was no strong pointer to m, m was destroyed.
    // Hence, following promotion will fail and we should not try to
    // use it any more.
    L(3)
    sp<Memory> spm3 = wpm1.promote();
    if (spm3 == NULL) {
        ALOGD("        Promotion Fail. Object had already been destructed");
    }

    L(-1)
    return 0;
}
