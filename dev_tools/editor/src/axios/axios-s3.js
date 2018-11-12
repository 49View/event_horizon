import axios from 'axios';

const instance=axios.create({
    baseURL: 'https://s3.amazonaws.com/sixthviewfs/'
});

export default instance;