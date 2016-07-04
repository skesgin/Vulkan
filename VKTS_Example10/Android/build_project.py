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
copy("../../VKTS_Binaries/shader/SPIR/V/bsdf.vert.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/shader/SPIR/V/bsdf_no_texcoord.vert.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/shader/SPIR/V/bsdf_tangents.vert.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/shader/SPIR/V/environment.vert.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/shader/SPIR/V/environment.frag.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/font/*.fnt", "./assets/font/")
copy("../../VKTS_Binaries/font/*.tga", "./assets/font/")
copy("../../VKTS_Binaries/material_probes/*.spv", "./assets/material_probes/")
copy("../../VKTS_Binaries/material_probes/*.vkts", "./assets/material_probes/")
copy("../../VKTS_Binaries/material_probes/*.tga", "./assets/material_probes/")
copy("../../VKTS_Binaries/primitives/*.vkts", "./assets/primitives/")

print("Building project")

os.chdir("jni")

subprocess.call("ndk-build", shell=True)

os.chdir("..")
        
subprocess.call("ant debug", shell=True)  
