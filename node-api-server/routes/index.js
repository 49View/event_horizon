var express = require('express');
var router = express.Router();

/* GET home page. */
router.get('/', function(req, res, next) {
  // res.send('Node api status: <img src="https://circleci.com/gh/49View/node-api-server.svg?style=svg&circle-token=0312dab6c17d25ac4cde35689efc24eb1616d3aa">');
  res.end('<!doctype html> \
  <html lang="en-us"> \
    <head> \
      <meta charset="utf-8"> \
      <meta http-equiv="Content-Type" content="text/html; charset=utf-8"> \
      <title>Emscripten-Generated Code</title> \
      <style> \
        .canvas { \
          height: 100%; \
          width: 100%; \
            } \
      </style> \
    </head> \
    <body style="margin-top: 0px;margin-left: 0px;margin-right: 0px;margin-bottom: 0px;"> \
  <div class="emscripten_border"> \
    <canvas class="display" id="canvas" oncontextmenu="event.preventDefault()" width="1106" height="852"></canvas> \
    <script> \
    Module = { \
      canvas: document.getElementById("canvas") \
    } \
  </script> \
    <script async="text/javascript" src="editor.js"></script> \
  </div> \
    </body> \
  </html>');
});

module.exports = router;
