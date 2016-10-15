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

#
# Execute with: -DCMAKE_BUILD_TYPE=Release
# for release build.
# 

print("Creating all make projects")

noVisual = ["VKTS_Example08"]

option = ""
isVisual = True

for x in range(1, len(sys.argv)):
    if ' ' in sys.argv[x]:
        option = option + " \"" + sys.argv[x] + "\""
    else:
        option = option + " " + sys.argv[x]
    
    if sys.argv[x] == "-DVKTS_WSI=VKTS_NO_VISUAL":
        isVisual = False

os.chdir("VKTS")

print("Processing 'VKTS'")

subprocess.call("cmake ." + option, shell=True)

os.chdir("..")

allExamples = os.listdir()

for example in allExamples:

    if isVisual and example in noVisual:
        continue
    elif not isVisual and example not in noVisual:
        continue

    if example.startswith("VKTS_Example"):
    
        print("Processing '%s'" % (example))    
        
        os.chdir(example)

        subprocess.call("cmake ." + option, shell=True)

        os.chdir("..")

allExamples = os.listdir()

for example in allExamples:

    if isVisual and example in noVisual:
        continue
    elif not isVisual and example not in noVisual:
        continue

    if example.startswith("VKTS_Test"):
    
        print("Processing '%s'" % (example))    
    
        os.chdir(example)

        subprocess.call("cmake ." + option, shell=True)

        os.chdir("..")        