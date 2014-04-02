#! /usr/bin/python

import os
import re
import sys

#############################################################
# GetPlatinumKitVersion
#############################################################
def GetPlatinumKitVersion():
    version = 0
    cwd = os.path.dirname(os.path.abspath(__file__))
    input = open(cwd+'/../Source/Platinum/PltVersion.h', 'r');
    lines = input.readlines()
    for line in lines:
        if line.startswith('#define PLT_PLATINUM_SDK_VERSION_STRING'):
            version = re.sub('.*?(\d+)\.(\d+)\.(\d+).(\d+).*', '\\1.\\2.\\3.\\4', line)
            break
    if version == 0:
        raise "unable to obtain Platinum Kit version"
    input.close()
    return version.strip()

print GetPlatinumKitVersion()