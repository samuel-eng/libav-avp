LIBAV_CONFIG_TYPE := ac3_no_neon

LOCAL_PATH := $(call my-dir)
LIBAV_DIR := $(LOCAL_PATH)/../../..

include $(LIBAV_DIR)/ndkbuild.mk
