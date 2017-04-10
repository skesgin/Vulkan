import glob
import os
import shutil
import subprocess  
import sys
  
from distutils.dir_util import copy_tree

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
        
def validate():
        
    os.chdir("libs")    
        
    allArchs = os.listdir()

    for arch in allArchs:
        copy_tree(os.environ['ANDROID_NDK_HOME'] + "/sources/third_party/vulkan/src/build-android/jniLibs/" + arch, arch)
    
    os.chdir("..")
        
####################
#
# Main
#
####################

print("Copying project assets")

copy("../../VKTS_Binaries/shader/SPIR/V/vertex_normal_texcoord.vert.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/shader/SPIR/V/texture_light.frag.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/textured_cube/*.vkts", "./assets/textured_cube/")
copy("../../VKTS_Binaries/textured_cube/*.tga", "./assets/textured_cube/")
copy("../../VKTS_Binaries/cache/textured_cube/*.tga", "./assets/cache/textured_cube/")

print("Building project")

os.chdir("jni")

subprocess.call("ndk-build -j", shell=True)

os.chdir("..")

for x in range(1, len(sys.argv)):
    if sys.argv[x] == "validate":
        validate()
        break
        
subprocess.call("ant debug", shell=True)  
