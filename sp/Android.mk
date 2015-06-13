LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= strongp.cpp

LOCAL_SHARED_LIBRARIES := \
   libutils \
   libcutils
        
LOCAL_C_INCLUDES += frameworks/base/include system/core/include
                
LOCAL_MODULE:= spdemo 

#####################################

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= weakp.cpp

LOCAL_SHARED_LIBRARIES := \
   libutils \
   libcutils
        
LOCAL_C_INCLUDES += frameworks/base/include system/core/include
                
LOCAL_MODULE:= wpdemo 

#####################################

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= strongp2.cpp

LOCAL_SHARED_LIBRARIES := \
   libutils \
   libcutils
        
LOCAL_C_INCLUDES += frameworks/base/include system/core/include
                
LOCAL_MODULE:= strongp2 


include $(BUILD_EXECUTABLE)
