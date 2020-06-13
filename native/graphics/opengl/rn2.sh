#/bin/bash

# for filename in ./shaders/*.wgl1.*; do 
#     [ -f "$filename" ] || continue
#   	mv "$filename" "${filename//.wgl1/}"
# done

for filename in ./shaders/*.wgl1.*; do mv "./$filename" "./$(echo "$filename" | sed -e 's/.wgl1//g')";  done