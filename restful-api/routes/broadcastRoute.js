const express = require('express');
const router = express.Router();
const socketController = require('../controllers/socketController');

router.post('/', (req,res,next) => {
    try {
        socketController.sendMessageToAllClients(JSON.stringify(req.body));
        res.sendStatus(204);
    } catch (ex) {
        console.log("Broadcast failed: ", ex);
        res.sendStatus(400);
    }
})

module.exports = router;
