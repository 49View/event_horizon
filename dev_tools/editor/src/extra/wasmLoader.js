/* global WebAssembly */
import axios from "../axios/backend";

const projectName = "editor";

export class wasmLoader {

    store = null;
    loaderModule = null;
    wasmBinary = null;
    wasmScript = null;
    wasmBinaryUrl = `/fs/artifacts%2F${projectName}.wasm`;
    wasmScriptUrl = `/fs/artifacts%2F${projectName}.js`;

    constructor(store)  {
        this.store = store;

        if (window.location.href.startsWith("https://localhost:5000")) {
            this.wasmBinaryUrl=`https://localhost:5000/${projectName}.wasm`;
            this.wasmScriptUrl=`https://localhost:5000/${projectName}.js`;
        } else if (window.location.href.startsWith("http://localhost:5000")) {
            this.wasmBinaryUrl=`http://localhost:5000/${projectName}.wasm`;
            this.wasmScriptUrl=`http://localhost:5000/${projectName}.js`;
        } 
        
        console.log(window.location.href);

        this.loaderModule = {
            print: (text) => {
                console.log(text);
            },
            printErr: (text) => {
                console.log("ERROR: "+text);
            },
            canvas: null,
            onRuntimeInitialized: () => {
                console.log("Runtime initialized");
                setTimeout(() => {
                    // window.Module.addScriptLine("DISABLE KEYBOARD");
                    // window.Module.pauseMainLoop();
                }, 3000);            },
            instantiateWasm: (imports, successCallback) => {
                WebAssembly.instantiate(window.wasmLoader.wasmBinary, imports).then(function(output) {
                    console.log('wasm instantiation succeeded');
                    successCallback(output.instance);
                }).catch(function(e) {
                    console.log('wasm instantiation failed! ' + e);
                });
                return {};
            }
        };
    }

    downloadWasm = async () => {
        try {
            let downloadConfig = {
                url: this.wasmBinaryUrl,
                method: "get",
                responseType: "arraybuffer"
            }

            const binaryContent = await axios(downloadConfig);
            this.wasmBinary=new Uint8Array(binaryContent.data);

            downloadConfig = {
                url: this.wasmScriptUrl,
                method: "get",
                responseType: "text"
            }

            const content = await axios(downloadConfig);
            this.wasmScript= content.data;
        } catch (ex) {
            console.log(ex);
            this.wasmBinary=null;
            this.wasmScript=null;
        }
    }

    instantiateWasm = async (canvas) => {

        await this.downloadWasm();

        this.loaderModule.canvas=canvas;
        window.Module=this.loaderModule;

        const s = document.createElement('script');                        
        s.text = this.wasmScript;
        document.body.appendChild(s);        
    }

    checkWasmSupport() {
        return (typeof WebAssembly === 'object');
    }

    checkWebGL2Support() {
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
}

export default wasmLoader;