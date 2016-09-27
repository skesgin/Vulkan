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

print("Creating all examples")

allExamples = os.listdir()

for example in allExamples:
    if example.startswith("VKTS_Example"):
        os.chdir(example)
        os.chdir("Android")

        exec(open("create_project.py").read())

        os.chdir("../..")

print("Creating all tests")

allExamples = os.listdir()

for example in allExamples:
    if example.startswith("VKTS_Test"):
        os.chdir(example)
        os.chdir("Android")

        exec(open("create_project.py").read())

        os.chdir("../..")        