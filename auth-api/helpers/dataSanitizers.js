
function checkType(value, type) {

    let result = false;
    let typedValue = undefined;
    switch (type.toUpperCase()) {
        case "STRING": {
            if (typeof value === "string") {
                result=true;
                typedValue=value;
            }
            break;
        }
        case "EMAIL": {
            if (typeof value === "string") {
                if (/^\w+([\.-]?\w+)*@\w+([\.-]?\w+)*(\.\w{2,3})+$/.test(value)) {
                    result=true;
                    typedValue=value;
                }
            }
            break;
        }
        case "INT": {
            if (typeof value === "string") {
                const numbers = /^[0-9]+$/;
                if(value.match(numbers)) {
                    result=true;
                    typedValue=Number(value);
                }
            }
            break;
        }
        case "DECIMAL": {
            if (typeof value === "string") {
                const decimal=  /^[-+]?[0-9]+\.[0-9]+$/; 
                if(value.match(decimal)) {
                    result=true;
                    typedValue=Number(value);
                }
            }
            break;
        }
        case "BOOLEAN": {
            if (typeof value === "string") {
                if (value.toUpperCase()==="Y" || value.toUpperCase()==="S" || value.toUpperCase()==="1") {
                    result=true;
                    typedValue=true;
                } else if (value.toUpperCase()==="N" || value.toUpperCase()==="0") {
                    result=true;
                    typedValue=false;
                } else {
                    result=false;
                }
            }
            break;
        }
        default: {
            result=false;
            typedValue=undefined;
        }
    }
    return [result, typedValue];
}

function checkSize(value, type, min, max) {
    let result = false;
    if (min===false && max===false) {
        result = true;
    } else {
        switch (type.toUpperCase()) {
            case "STRING": {
                let minCheck=true;
                let maxCheck=true;
                if (min!==false) {
                    if (value.length<min) {
                        minCheck=false;
                    }
                }
                if (max!==false) {
                    if (value.length>max) {
                        maxCheck=false;                      
                    }
                }
                result=minCheck && maxCheck;
                break;
            }
            case "INT":
            case "DECIMAL": {
                let minCheck=true;
                let maxCheck=true;
                if (min!==false) {
                    if (value<min) {
                        minCheck=false;
                    }
                }
                if (max!==false) {
                    if (value>max) {
                        maxCheck=false;                      
                    }
                }
                result=minCheck && maxCheck;
                break;
            }
            case "EMAIL":
            case "BOOLEAN":
            default: {
                result=true;
            }
        }
    }
    return result;
}

module.exports = {

    checkBody: (req, paramDef) => {

        result={};
        error=null;
        try {
            for (let i=0;i<paramDef.length;i++) {            
                name=paramDef[i].name;
                type=paramDef[i].type || "string";
                required=paramDef[i].required || false;
                min=paramDef[i].min || false;
                max=paramDef[i].max || false;
                if (name===undefined || name===null) {
                    throw `invalid params definition`;
                }
                param = req.body[name];

                if (required && (param===undefined || param===null)) {
                    throw `param ${name} in required`;
                }
                if (param!==undefined && param!==null) {
                    const [valid, value] = checkType(param, type);
                    if (!valid) {
                        throw `invalid type for param ${name}`;
                    }
                    param=value;
                    if (!checkSize(param,type,min,max)) {
                        throw `invalid value for param ${name} (${min!==false?'min:'+min:''} ${max!==false?'max:'+max:''} )`;
                    }
                }
                result[name]=param;
            }
        } catch (ex) {
            result = null;
            error = `body params error, ${ex}`;
        }
        return [result,error];
    }
}