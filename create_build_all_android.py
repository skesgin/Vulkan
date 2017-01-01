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

        directory = os.getcwd() + "/" + self.buildDirectory + "/Android/jni"

        subprocess.call("ndk-build -C %s" % (directory), shell=True)

        print("Finished '%s'." % (self.buildDirectory))
        
####################
#
# Main
#
####################

print("Creating and building all Android projects")

#

currentValidate = ""

for x in range(1, len(sys.argv)):
    if sys.argv[x] == 'validate':
        currentValidate = " validate"
    
option = currentValidate 

#

allBuildThreads = []

allVKTS = os.listdir()

for package in allVKTS:
    if package.startswith("VKTS_PKG"):
        currentBuildThread = BuildThread(package, "")
        allBuildThreads.append(currentBuildThread)
        currentBuildThread.start()

for currentBuildThread in allBuildThreads:
    currentBuildThread.join()

#

allExamples = os.listdir()

for example in allExamples:
    if example.startswith("VKTS_Example") or example.startswith("VKTS_Test"):
    
        print("Processing '%s' ..." % (example))    
    
        os.chdir(example)
        os.chdir("Android")

        subprocess.call("create_project.py", shell=True)
        subprocess.call("build_project.py %s" % (option), shell=True)

        os.chdir("../..")

        print("Finished '%s'." % (example))

allExamples = os.listdir()

print("Done.")