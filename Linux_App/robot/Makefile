include CommonDefs.mak

BIN_DIR = Bin

INC_DIRS = \
	../../Include \
	../Common

SRC_FILES = *.cpp ./cvblob_lib/*.cpp

USED_LIBS += OpenNI2
#USED_LIBS += `pkg-config --libs opencv`
USED_LIBS += opencv_core opencv_imgproc opencv_highgui opencv_objdetect
USED_LIBS += pthread rt


EXE_NAME = OpenCV

CFLAGS += -Wall
CFLAGS += `pkg-config --cflags opencv`


ifndef OPENNI2_INCLUDE
    $(error OPENNI2_INCLUDE is not defined. Please define it or 'source' the OpenNIDevEnvironment file from the installation)
else ifndef OPENNI2_REDIST
    $(error OPENNI2_REDIST is not defined. Please define it or 'source' the OpenNIDevEnvironment file from the installation)
endif

INC_DIRS += $(OPENNI2_INCLUDE)

include CommonCppMakefile

.PHONY: 
#copy-redist:
#	cp -R $(OPENNI2_REDIST)/* $(OUT_DIR)	

#$(OUTPUT_FILE): copy-redist

