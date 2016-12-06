Assets used by the examples:
----------------------------

This folder and sub folders contains all executables, shaders and assets. The examples are copied  
into this folder after the build. Generated mip maps, cube maps and BSDF look up textures are stored in the `cache` folder.
By default, this content is already generated.

VKTS is trying to locate `vk_layer_settings.txt` and uses this folder as the working directory.     
  
__This folder has to be the working directory, otherwise shaders and assets are not found__.  
  
Using Android, the needed asset files will be copied in the asset folder after executing `build_project.py`.

Inside `shader/GLSL/` there is a python build script for the host system, which do compile all available GLSL source files.  
The resulting SPIR-V binaries are deployed into `shader/SPIR/V`.  
