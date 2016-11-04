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
# Pass 'VKTS_DISPLAY_VISUAL' for VK_KHR_display WSI project files.
# Pass 'VKTS_WAYLAND_VISUAL' for VK_KHR_wayland_surface WSI project files.
# 

print("Creating and building all make projects")

noVisual = ["VKTS_Example08"]

option = ""

currentVisual = ""

for x in range(1, len(sys.argv)):
    if sys.argv[x] == 'VKTS_DISPLAY_VISUAL':
        currentVisual = "-DVKTS_WSI=VKTS_DISPLAY_VISUAL"
    if sys.argv[x] == 'VKTS_WAYLAND_VISUAL':
        currentVisual = "-DVKTS_WSI=VKTS_WAYLAND_VISUAL"
        
for currentOption in [currentVisual, "-DVKTS_WSI=VKTS_NO_VISUAL"]:

    isVisual = True

    if currentOption == "-DVKTS_WSI=VKTS_NO_VISUAL":
        isVisual = False
        
    finalOption = option + " " + currentOption + " -DCMAKE_BUILD_TYPE=Release"
    
    #
        
    os.chdir("VKTS")

    print("Processing 'VKTS'")

    subprocess.call("cmake ." + finalOption, shell=True)
    subprocess.call("make", shell=True)

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

            subprocess.call("cmake ." + finalOption, shell=True)
            subprocess.call("make", shell=True)

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

            subprocess.call("cmake ." + finalOption, shell=True)
            subprocess.call("make", shell=True)

            os.chdir("..")        