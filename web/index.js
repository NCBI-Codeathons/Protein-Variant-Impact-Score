var http = require('http');
//var https = require('https');

http.createServer(function(req, res) {
  var match = /^([^?]*)(\?.*)?$/.exec(req.url);
  var path = match[1].replace(/^\/*/, '').replace(/\/*$/, '');
  var query = match[2];
  res.writeHead(200, {'Content-Type': 'text/plain'});
  console.log('Serving:', req.url);
  res.end('Serving:' + req.url);
}).listen(8000);
console.log('Started at port 8000');
