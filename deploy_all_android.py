import glob
import os
import shutil
import subprocess  

####################
#
# Functions
#
####################

def copy(src, dst):
    # Copy exact file or given by wildcard 
    for filename in glob.glob(src):
        print("Copying asset '%s' to '%s'" % (os.path.basename(filename), dst))
        shutil.copyfile(filename, dst)        
        
####################
#
# Main
#
####################

print("Deploying all Android projects")

allExamples = os.listdir()

for example in allExamples:
    if example.startswith("VKTS_Example"):
    
        print("Processing '%s'" % (example))    
    
        copy(example + "/Android/bin/NativeActivity-debug.apk", example + "-debug.apk")
