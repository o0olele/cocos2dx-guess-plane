LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/cocos)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/cocos/audio/include)

LOCAL_MODULE := MyGame_shared

LOCAL_MODULE_FILENAME := libMyGame

LOCAL_SRC_FILES := $(LOCAL_PATH)/hellocpp/main.cpp \
                   $(LOCAL_PATH)/../../../Classes/AppDelegate.cpp \
                   $(LOCAL_PATH)/../../../Classes/HelloWorldScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/LoginScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/RegScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/HomeScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/SetPlaneScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/DuelScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/MyPlaneScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/WarnScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/SettingScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/PlaneSprite.cpp \
                   $(LOCAL_PATH)/../../../Classes/SocketBase.cpp \
                   $(LOCAL_PATH)/../../../Classes/SocketClient.cpp \
                   $(LOCAL_PATH)/../../../Classes/Global.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../Classes

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END


LOCAL_STATIC_LIBRARIES := cocos2dx_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-add-path, $(LOCAL_PATH)/../../../cocos2d)
$(call import-module, cocos)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
