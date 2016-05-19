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
    # Create directory if needed
    if not os.path.exists(dst):
        os.makedirs(dst)

    # Copy exact file or given by wildcard 
    for filename in glob.glob(src):
        print("Copying asset '%s'" % (os.path.basename(filename)))
        shutil.copy(filename, dst)        
        
####################
#
# Main
#
####################

print("Copying project assets")

copy("../../VKTS_Binaries/shader/SPIR/V/font.vert.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/shader/SPIR/V/font.frag.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/font/*.fnt", "./assets/font/")
copy("../../VKTS_Binaries/font/*.tga", "./assets/font/")
copy("../../VKTS_Binaries/test/cycles/*.spv", "./assets/test/cycles/")
copy("../../VKTS_Binaries/test/cycles/*.vkts", "./assets/test/cycles/")
copy("../../VKTS_Binaries/test/cycles/*.tga", "./assets/test/cycles/")

print("Building project")

os.chdir("jni")

subprocess.call("ndk-build", shell=True)

os.chdir("..")
        
subprocess.call("ant debug", shell=True)  
