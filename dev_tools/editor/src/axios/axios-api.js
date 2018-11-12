import axios from 'axios';
import { cacheAdapterEnhancer } from 'axios-extensions';

export const baseUrl = 'https://eventhorizon.pw/';
const instance=axios.create({
    baseURL: baseUrl,
    headers: {
        'Content-Type': 'application/json',
        'Cache-Control': 'no-cache'
    },
    adapter: cacheAdapterEnhancer(axios.defaults.adapter)
});

export default instance;