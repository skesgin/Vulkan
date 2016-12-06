import os
import subprocess
import sys
import threading

####################
#
# Functions
#
####################
        
class BuildThread (threading.Thread):

    def __init__(self, buildDirectory, buildOption):
        threading.Thread.__init__(self)
        self.buildDirectory = buildDirectory
        self.buildOption = buildOption

    def run(self):
        print("Processing '%s' ..." % (self.buildDirectory))

        directory = os.getcwd() + "/" + self.buildDirectory

        subprocess.call("cmake -H%s -B%s %s" % (directory, directory + "/build",self.buildOption), shell=True)
        subprocess.call("make -C %s" % (directory + "/build"), shell=True)

        print("Finished '%s'." % (self.buildDirectory))
        
####################
#
# Main
#
####################

#
# Pass 'VKTS_DISPLAY_VISUAL' for VK_KHR_display WSI project files.
# Pass 'VKTS_WAYLAND_VISUAL' for VK_KHR_wayland_surface WSI project files.
# 

print("Creating and building all make projects ...")

#

currentVisual = ""

for x in range(1, len(sys.argv)):
    if sys.argv[x] == 'VKTS_DISPLAY_VISUAL':
        currentVisual = " -DVKTS_WSI=VKTS_DISPLAY_VISUAL"
    if sys.argv[x] == 'VKTS_WAYLAND_VISUAL':
        currentVisual = " -DVKTS_WSI=VKTS_WAYLAND_VISUAL"
    
option = " -DCMAKE_BUILD_TYPE=Release" + currentVisual 

#

allBuildThreads = []

allVKTS = os.listdir()

for package in allVKTS:
    if package.startswith("VKTS_PKG"):
        currentBuildThread = BuildThread(package, option)
        allBuildThreads.append(currentBuildThread)
        currentBuildThread.start()

for currentBuildThread in allBuildThreads:
    currentBuildThread.join()

#

allBuildThreads = []

allVKTS = os.listdir()

for package in allVKTS:
    if package.startswith("VKTS_Example") or package.startswith("VKTS_Test"):
        currentBuildThread = BuildThread(package, option)
        allBuildThreads.append(currentBuildThread)
        currentBuildThread.start()

for currentBuildThread in allBuildThreads:
    currentBuildThread.join()

#

print("Done.")