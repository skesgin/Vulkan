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

copy("../../VKTS_Binaries/shader/SPIR/V/phong_skinning_shadow.vert.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/shader/SPIR/V/phong_skinning_shadow.frag.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/shader/SPIR/V/phong_skinning_write_shadow.frag.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/shader/SPIR/V/phong_shadow.vert.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/shader/SPIR/V/phong_shadow.frag.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/shader/SPIR/V/phong_write_shadow.frag.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/transport_shuttle/*.vkts", "./assets/transport_shuttle/")
copy("../../VKTS_Binaries/transport_shuttle/*.tga", "./assets/transport_shuttle/")
copy("../../VKTS_Binaries/cache/transport_shuttle/*.tga", "./assets/cache/transport_shuttle/")

print("Building project")

os.chdir("jni")

subprocess.call("ndk-build -j", shell=True)

os.chdir("..")

for x in range(1, len(sys.argv)):
    if sys.argv[x] == "validate":
        validate()
        break
        
subprocess.call("ant debug", shell=True)  
