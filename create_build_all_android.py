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

print("Creating and building all Android projects")

os.chdir("VKTS/Android/jni")

print("Processing 'VKTS'")

os.system("ndk-build")

os.chdir("../../..")

allExamples = os.listdir()

for example in allExamples:
    if example.startswith("VKTS_Example"):
    
        print("Processing '%s'" % (example))    
    
        os.chdir(example)
        os.chdir("Android")

        exec(open("create_project.py").read())
        exec(open("build_project.py").read())

        os.chdir("../..")

allExamples = os.listdir()

for example in allExamples:
    if example.startswith("VKTS_Test"):
    
        print("Processing '%s'" % (example))    
    
        os.chdir(example)
        os.chdir("Android")

        exec(open("create_project.py").read())
        exec(open("build_project.py").read())

        os.chdir("../..")        