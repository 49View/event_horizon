import axios from 'axios';
import { cacheAdapterEnhancer } from 'axios-extensions';

export const baseUrl = (process.env.REACT_APP_FORCE_LOCAL_API==="true")?'https://localhost:5000':'https://api.ateventhorizon.com';

console.log("HOSTNAME: ",process.env.REACT_APP_FORCE_LOCAL_API);

const instance=axios.create({
    baseURL: baseUrl,
    headers: {
        'Content-Type': 'application/json',
        'Cache-Control': 'no-cache',
    },
    withCredentials: true,
    adapter: cacheAdapterEnhancer(axios.defaults.adapter)
});

export default instance;