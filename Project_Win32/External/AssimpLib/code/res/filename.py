import fnmatch
import os

for file in os.listdir('../'):
    if fnmatch.fnmatch(file, '*.cpp'):
        print "\t\t\"$ExternalLibs_Path$/AsimpLib/code/" + file + "\""