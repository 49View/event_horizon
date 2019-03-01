import axios from 'axios';
import { cacheAdapterEnhancer } from 'axios-extensions';

export const baseUrl = 'https://api.ateventhorizon.com';
//export const baseUrl = 'http://localhost:5000';

const instance=axios.create({
    baseURL: baseUrl,
    headers: {
        'Content-Type': 'application/json',
        'Cache-Control': 'no-cache',
        'x-eventhorizon-guest': 'guest'
    },
    withCredentials: true,
    adapter: cacheAdapterEnhancer(axios.defaults.adapter)
});

export default instance;