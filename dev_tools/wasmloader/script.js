const options = {
    wasmBinary: "editor.wasm",
    wasmScript: "editor.js",
    canvasId: "canvas"
}

let wasmBinaryContent = null;
let wasmScriptContent = null;
let canvasElement = null;


document.onreadystatechange = async () => {
    if (document.readyState == "complete") {
        await onDocumentReady();
    }
}

const onDocumentReady = async () => {
    try {
        console.log("Start");
        if (!checkWasmSupport() || !checkWebGL2Support()) {
            throw new Error("WASM o WEBGL2 UNSUPPORTED");
        }
        canvasElement = document.getElementById(options.canvasId);
        await loadWasm();
        console.log("File downloaded");
        if (wasmBinaryContent!==null && wasmScriptContent!==null) {
            await runWasm();
            console.log("Wasm started");
        }
    } catch (ex) {
        console.log("Error",ex);
    }
}

const runWasm = async () => {

    return new Promise( (resolve, reject) => {
        window.Module = {
            print: (text) => {
                console.log("W: "+text);
            },
            printErr: (text) => {
                console.log("W ERROR: "+text);
            },
            canvas: canvasElement,
            onRuntimeInitialized: () => {
                console.log("Runtime initialized");
                resolve();
            },
            instantiateWasm: (imports, successCallback) => {
                WebAssembly.instantiate(wasmBinaryContent, imports).then(function(output) {
                    console.log('wasm instantiation succeeded');
                    successCallback(output.instance);
                }).catch(function(e) {
                    console.log('wasm instantiation failed! ' + e);
                    reject();
                });
                return {};
            }
        };
    
        const s = document.createElement('script');                        
        s.text = wasmScriptContent;
        document.body.appendChild(s);     
    });
}

const loadWasm = async () => {
    let fileContent = null;
    fileContent=await downloadFile(options.wasmBinary);
    if (fileContent!==null) {
        wasmBinaryContent=await fileContent.arrayBuffer();
    }
    fileContent=await downloadFile(options.wasmScript);
    if (fileContent!==null) {
        wasmScriptContent=await fileContent.text();
    }
}

const downloadFile = async (url) => {

    let downloadResponse = null;
    try {
        const currentDate = new Date();
        const downloadRequest = new Request(url+"?t="+currentDate.getTime());
        downloadResponse = await fetch(downloadRequest);
    } catch (ex) {
        console.log("Error downloading "+url, ex);
        downloadResponse=null;
    }

    return downloadResponse;
}

const checkWasmSupport = () => {
    return (typeof WebAssembly === 'object');
}

const checkWebGL2Support = () => {
    let canvas=document.createElement('canvas');
    let result=false;
    try {
        if (canvas.getContext("webgl2")!==null) {
            result=true;
        }
    } catch (ex) {

    }
    return result;
}