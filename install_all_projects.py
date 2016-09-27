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

print("Installing all examples")

allExamples = os.listdir()

for example in allExamples:
    if example.startswith("VKTS_Example"):
        os.chdir(example)
        os.chdir("Android")

        exec(open("install_project.py").read())

        os.chdir("../..")

print("Installing all tests")

allExamples = os.listdir()

for example in allExamples:
    if example.startswith("VKTS_Test"):
        os.chdir(example)
        os.chdir("Android")

        exec(open("install_project.py").read())

        os.chdir("../..")        