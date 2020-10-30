#this script was made to update the CMakeLists.txt.
#It supports c and cpp files.

#Directory layout:

#Project dir
    #CMakeLists.txt
    #src
        #c and cpp files
    #scripts
        #generate.py

#Usage: python generate.py

import os;

src_files = ""
hdr_files = ""

for filename in sorted(os.listdir("../src")):
    if filename.endswith(".cpp") or filename.endswith(".c"):
	    src_files += "src/" + filename + "\n\t"

for filename in sorted(os.listdir("../src")):
    if filename.endswith(".hpp") or filename.endswith(".c"):
        hdr_files += "src/" + filename + "\n\t"

assert len(src_files) > 0, "the src dir don't contains .c or .cpp files"

cmake_template_text = open("cmake_template", "r").read()

src_start = cmake_template_text.find("#SRC HERE")

assert src_start != -1, "the template is missing the #SRC HERE line"

cmake_file = open("../CMakeLists.txt", "w")
cmake_template_text = cmake_template_text[0 : src_start] + src_files[0:-1] + cmake_template_text[src_start + 10:]

hdr_start = cmake_template_text.find("#HDR HERE")
assert hdr_start != -1, "the template is missing the #HDR HERE line"

cmake_file.write(cmake_template_text[0 : hdr_start] + hdr_files[0:-1] + cmake_template_text[hdr_start + 10:])
