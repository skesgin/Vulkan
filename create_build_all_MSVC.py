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
# Pass '64bit' for 64bit project files.
# 

print("Creating and building all MSVC projects")

noVisual = ["VKTS_Example08"]

option = ""

for x in range(1, len(sys.argv)):
    if sys.argv[x] == '64bit':
        option = option + " " + '-G "Visual Studio 14 2015 Win64"'
        
for currentOption in ["", "-DVKTS_WSI=VKTS_NO_VISUAL"]:

    isVisual = True

    if currentOption == "-DVKTS_WSI=VKTS_NO_VISUAL":
        isVisual = False
        
    finalOption = option + " " + currentOption
    
    #

    os.chdir("VKTS/MSVC")

    print("Processing 'VKTS'")

    subprocess.call("cmake .." + finalOption, shell=True)
    subprocess.call("msbuild VKTS.sln /p:Configuration=Release", shell=True)

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

            subprocess.call("cmake .." + finalOption, shell=True)
            subprocess.call("msbuild %s.sln /p:Configuration=Release" % (example), shell=True)

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

            subprocess.call("cmake .." + finalOption, shell=True)
            subprocess.call("msbuild %s.sln /p:Configuration=Release" % (example), shell=True)

            os.chdir("../..")        