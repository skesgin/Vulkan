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

        subprocess.call("cmake -H%s -B%s %s" % (directory, directory + "/MSVC",self.buildOption), shell=True)
        subprocess.call("msbuild %s.sln /p:Configuration=Release" % (directory + "/MSVC/" + self.buildDirectory), shell=True)

        print("Finished '%s'." % (self.buildDirectory))
        
####################
#
# Main
#
####################

#
# Pass '64bit' for 64bit project files.
# 

print("Creating and building all MSVC projects ...")

#

currentArchitecture = ""

for x in range(1, len(sys.argv)):
    if sys.argv[x] == '64bit':
        currentArchitecture = " " + '-G "Visual Studio 14 2015 Win64"'
    
option = " -DCMAKE_BUILD_TYPE=Release" + currentArchitecture 

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