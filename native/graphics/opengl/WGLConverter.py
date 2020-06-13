import sys, string, os, re

#based on https://webgl2fundamentals.org/webgl/lessons/webgl1-to-webgl2.html

CONVERSION_TYPES = ['2TG','GT1','GT2']
EXTENSIONS = {'2TG':'.gen','GT1':'.wgl1','GT2':'.wgl2'}

DICT_2TG = {'\\s*(layout\\s*\\(\\s*location\\s+=\\s*(\\d+)\\s*\\))':'%LOCATION#1#%','\\s*(layout\\s*\\(\\s*std140\\s+\\))':'%LOCATION#1#%','\\s*(in)\\s+':'%IN%','\\s*(out)\\s+':'%OUT%','\\s*(texture2d)\\s+':'%TEXTURE2D%'}
DICT_GT2 = {'\\s*(\\%LOCATION(\\d+)\\%)\\s*':'layout( location = #1# )','\\s*(\\%IN\\%)\\s*':'in','\\s*(\\%OUT\\%)\\s*':'out','\\s*(\\%TEXTURE2D\\%)\\s*':'texture2d'}
DICT_GT1 = {'\\s*(\\%LOCATION(\\d+)\\%)\\s*':'','\\s*(\\%IN\\%)\\s*':'attribute','\\s*(\\%OUT\\%)\\s*':'varying','\\s*(\\%TEXTURE2D\\%)\\s*':'texture'}

def getParams():
    inputFilename=None
    conversionType = None
    if (len(sys.argv) > 1):
        for arg in sys.argv:
            if (arg.startswith("--input=") or arg.startswith("--i=")):
                inputFilename=arg[arg.find("=")+1:]
            if (arg.startswith("--type=") or arg.startswith("--t=")):
                conversionType=arg[arg.find("=")+1:]
    return inputFilename, conversionType

def getOutputFilename(inputFilename, conversionType):
    
    fileName, fileExtension = os.path.splitext(inputFilename)

    for k in EXTENSIONS:
        v = EXTENSIONS[k]
        fileName = fileName.replace(v,"")

    outputExtension = EXTENSIONS.get(conversionType)
    return fileName + outputExtension + fileExtension, fileExtension

inputFilename, conversionType = getParams()

if (inputFilename is None or conversionType is None or CONVERSION_TYPES.count(conversionType)==0):
    print("Use: %s --input=inputfilename --type=conversiontype" % (os.path.basename(sys.argv[0])))
    exit()

outFilename, fileExtension = getOutputFilename(inputFilename,conversionType)

if (conversionType=="2TG"):
    dictionary = DICT_2TG
elif (conversionType=="GT2"):
    dictionary = DICT_GT2
elif (conversionType=="GT1"):
    dictionary = DICT_GT1

inFile = open(inputFilename,"r")
outFile = open(outFilename,"w")

for iR in inFile:
    oR = iR
    for k in dictionary:
        v=dictionary[k]
        match = re.search(k,oR)
        while match != None:
            localv = v
            if (len(match.groups())>1):
                for g in range(1,len(match.groups())):
                    localv = localv.replace("#%d#" % (g),match.groups()[g])
            oR = oR[:match.regs[1][0]]+localv+oR[match.regs[1][1]:]

            match = re.search(k,oR)
        #oR=re.sub(k,dictionary[k],oR)
    outFile.write(oR)
#print(inputFilename, conversionType, getOutputFilename(inputFilename,conversionType))

inFile.close()
outFile.close()

print("Created file %s" % (outFilename))
