import os
import sys
import urllib.request
import zipfile

#

fileName = "VKTS_Binaries.zip"
binaryURL = "http://nopper.tv/Vulkan/" + fileName
BLOCK_SIZE = 1024*1024

#

print("Retrieving '%s'" % (binaryURL))    

response = urllib.request.urlopen(binaryURL)

if response is None:
    print("Could not open URL")
    sys.exit(-1)

with open(fileName, 'wb') as f:

    while True:
        blockRead = response.read(BLOCK_SIZE)
        print('.', sep='', end='', flush=True)
        if not blockRead:
            break
        f.write(blockRead)
    
    f.close()

print("")
print("Saved '%s'" % (fileName))
print("")

#

print("Unzipping '%s'" % (fileName))

binaryZip = zipfile.ZipFile(fileName, 'r')
binaryZip.extractall(".")
binaryZip.close()

print("Done")