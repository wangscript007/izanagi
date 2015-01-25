LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE     := libResource
LOCAL_CFLAGS     := -DANDROID -D__IZ_GLES2__ -D_DEBUG -D_LIB -D__IZ_DEBUG__ -D__IZ_OGL__ -D__IZANAGI_NO_USE_D3D__
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../include
LOCAL_SRC_FILES  := ../../source/resource/Resource.cpp \
../../source/resource/ResourceManager.cpp \
../../source/resource/archive/Archive.cpp \
../../source/resource/archive/ArchiveTarball.cpp

include $(BUILD_STATIC_LIBRARY)
