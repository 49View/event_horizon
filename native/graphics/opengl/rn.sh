#/bin/bash

for f in ./shaders/*; do 
python WGLConverter.py --input="$f" --type=2TG
done

for f in ./shaders/*.gen.*; do 
python WGLConverter.py --input="$f" --type=GT1
done

