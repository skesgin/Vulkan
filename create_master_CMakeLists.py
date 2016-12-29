import os

####################
#
# Credits
#
# Based on code from Prabhu Sundararaj <prabhu.sundararaj@nxp.com>
#
####################


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

print("Creating master CMakeLists.txt for all projects")

#

outputFile = open("CMakeLists.txt", "w")

#

outputFile.write("cmake_minimum_required(VERSION 3.2)\n")
outputFile.write("\n")
outputFile.write('project("VKTS")\n')
outputFile.write("\n")

allVKTS = os.listdir()

#

for package in allVKTS:
    if package.startswith("VKTS_PKG"):
        print("Processing '%s'" % (package))
        outputFile.write("add_subdirectory(%s)\n" % (package))

outputFile.write("\n")

for package in allVKTS:
    if package.startswith("VKTS_Example"):
        print("Processing '%s'" % (package))
        outputFile.write("add_subdirectory(%s)\n" % (package))

outputFile.write("\n")

for package in allVKTS:
    if package.startswith("VKTS_Test"):
        print("Processing '%s'" % (package))
        outputFile.write("add_subdirectory(%s)\n" % (package))

#

outputFile.close()

#

print("Done.")