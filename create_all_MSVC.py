import os
import subprocess
import sys

####################
#
# Functions
#
####################
        
        
####################
#
# Main
#
####################

noVisual = ["VKTS_Example08"]

option = ""
isVisual = True

if len(sys.argv) > 1:
    option = " " + sys.argv[1]
    
    if sys.argv[1] == "-DVKTS_WSI=VKTS_NO_VISUAL":
        isVisual = False

print("Building VKTS")

os.chdir("VKTS/MSVC")

subprocess.call("cmake .." + option, shell=True)

os.chdir("../..")

print("Building all examples")

allExamples = os.listdir()

for example in allExamples:

    if isVisual and example in noVisual:
        continue
    elif not isVisual and example not in noVisual:
        continue

    if example.startswith("VKTS_Example"):
        os.chdir(example)
        os.chdir("MSVC")

        subprocess.call("cmake .." + option, shell=True)

        os.chdir("../..")

print("Building all tests")

allExamples = os.listdir()

for example in allExamples:

    if isVisual and example in noVisual:
        continue
    elif not isVisual and example not in noVisual:
        continue

    if example.startswith("VKTS_Test"):
        os.chdir(example)
        os.chdir("MSVC")

        subprocess.call("cmake .." + option, shell=True)

        os.chdir("../..")        