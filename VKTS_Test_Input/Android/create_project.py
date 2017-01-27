import os
import subprocess  

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

print("Creating project files")
        
subprocess.call("android update project -n NativeActivity -p . -s -t android-24", shell=True)  
