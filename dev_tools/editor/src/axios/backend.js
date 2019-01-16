import axios from 'axios';
import { cacheAdapterEnhancer } from 'axios-extensions';

export const baseUrl = (window.location.href.startsWith("https://localhost")) ? 'https://localhost:3000' : 'https://api.ateventhorizon.com';
//export const baseUrl = 'https://api.ateventhorizon.com';

const instance=axios.create({
    baseURL: baseUrl,
    headers: {
        'Content-Type': 'application/json',
        'Cache-Control': 'no-cache',
        'x-eventhorizon-guest': 'eh_sandbox'
    },
    withCredentials: true,
    adapter: cacheAdapterEnhancer(axios.defaults.adapter)
});

export default instance;