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

print("Building VKTS")

os.chdir("VKTS/Android/jni")

os.system("ndk-build")

os.chdir("../../..")

print("Building all examples")

allExamples = os.listdir()

for example in allExamples:
    if example.startswith("VKTS_Example"):
        os.chdir(example)
        os.chdir("Android")

        exec(open("build_project.py").read())

        os.chdir("../..")

print("Building all tests")

allExamples = os.listdir()

for example in allExamples:
    if example.startswith("VKTS_Test"):
        os.chdir(example)
        os.chdir("Android")

        exec(open("build_project.py").read())

        os.chdir("../..")        