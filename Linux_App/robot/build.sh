#!/bin/bash
export OPENNI2_INCLUDE=~/xtion/OpenNI2/Include/
export OPENNI2_REDIST=.

PLATFORM=Arm make ALLOW_WARNINGS=1
