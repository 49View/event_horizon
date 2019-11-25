const express = require('express');
const router = express.Router();

router.get('/', (req,res,next) => {

    result = { "event_horizon": "2.0"}

    console.log("INDEX", result);
    res.send(result);
})

module.exports = router;
