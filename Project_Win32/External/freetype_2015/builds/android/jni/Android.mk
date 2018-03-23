# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
FREETYPE_PROJECT_PATH:=$(call my-dir)
LOCAL_PATH:= $(call my-dir)/../../../src

include $(CLEAR_VARS)
LOCAL_MODULE    := libfreetype
LOCAL_CFLAGS    := -mandroid -DFT2_BUILD_LIBRARY -D_LIB
LOCAL_CPPFLAGS  := -mandroid -DFT2_BUILD_LIBRARY -D_LIB -std=gnu++11
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../include
LOCAL_SRC_FILES  := base/ftbbox.c \
					base/ftmm.c \
					base/ftpfr.c \
					base/ftsynth.c \
					base/fttype1.c \
					base/ftwinfnt.c \
					base/ftbase.c \
					base/ftbitmap.c \
					base/ftgasp.c \
					base/ftglyph.c \
					base/ftinit.c \
					base/ftstroke.c \
					base/ftsystem.c \
					cache/ftcache.c \
					pcf/pcf.c \
					pfr/pfr.c \
					psaux/psaux.c \
					pshinter/pshinter.c \
					psnames/psnames.c \
					raster/raster.c \
					sfnt/sfnt.c \
					truetype/truetype.c \
					type1/type1.c \
					cid/type1cid.c \
					type42/type42.c \
					winfonts/winfnt.c \
					autofit/autofit.c \
					bdf/bdf.c \
					cff/cff.c \
					lzw/ftlzw.c \
					smooth/smooth.c \
					gzip/ftgzip.c

include $(BUILD_STATIC_LIBRARY)
