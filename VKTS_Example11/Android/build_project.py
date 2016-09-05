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

copy("../../VKTS_Binaries/shader/SPIR/V/phong_shadow.vert.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/shader/SPIR/V/phong_shadow.frag.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/shader/SPIR/V/phong_write_shadow.frag.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/shader/SPIR/V/voxelize.vert.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/shader/SPIR/V/voxelize.geom.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/shader/SPIR/V/voxelize.frag.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/sponza/*.vkts", "./assets/sponza/")
copy("../../VKTS_Binaries/sponza/*.tga", "./assets/sponza/")

print("Building project")

os.chdir("jni")

subprocess.call("ndk-build", shell=True)

os.chdir("..")
        
subprocess.call("ant debug", shell=True)  
