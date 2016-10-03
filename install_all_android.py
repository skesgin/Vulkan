import os

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

print("Installing all Android projects")

allExamples = os.listdir()

for example in allExamples:
    if example.startswith("VKTS_Example"):
    
        print("Processing '%s'" % (example))    
    
        os.chdir(example)
        os.chdir("Android")

        exec(open("install_project.py").read())

        os.chdir("../..")

allExamples = os.listdir()

for example in allExamples:
    if example.startswith("VKTS_Test"):
    
        print("Processing '%s'" % (example))    
    
        os.chdir(example)
        os.chdir("Android")

        exec(open("install_project.py").read())

        os.chdir("../..")        