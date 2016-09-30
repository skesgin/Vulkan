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

print("Building all MSVC projects")

noVisual = ["VKTS_Example08"]

option = ""
isVisual = True

for x in range(1, len(sys.argv)):
    option = option + " " + sys.argv[x]
    
    if sys.argv[x] == "-DVKTS_WSI=VKTS_NO_VISUAL":
        isVisual = False

os.chdir("VKTS/MSVC")

print("Processing 'VKTS'")

subprocess.call("msbuild VKTS.sln", shell=True)

os.chdir("../..")

allExamples = os.listdir()

for example in allExamples:

    if isVisual and example in noVisual:
        continue
    elif not isVisual and example not in noVisual:
        continue

    if example.startswith("VKTS_Example"):
    
        print("Processing '%s'" % (example))    
        
        os.chdir(example)
        os.chdir("MSVC")

        subprocess.call("msbuild %s.sln" % (example), shell=True)

        os.chdir("../..")

allExamples = os.listdir()

for example in allExamples:

    if isVisual and example in noVisual:
        continue
    elif not isVisual and example not in noVisual:
        continue

    if example.startswith("VKTS_Test"):
    
        print("Processing '%s'" % (example))    
    
        os.chdir(example)
        os.chdir("MSVC")

        subprocess.call("msbuild %s.sln" % (example), shell=True)

        os.chdir("../..")        