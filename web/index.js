var http = require('http');
var https = require('https');
var fs = require('fs');

var contentTypes = {
  '.html': 'text/html',
  '.css': 'text/css',
  '.png': 'image/png',
  '.gif': 'image/gif',
  '.ico': 'image/x-icon'
};

function contentType(path) {
  var x = path.search(/\.[^\\/.]*$/);
  if (x >= 0) x = contentTypes[path.substr(x)];
  return x || 'text/plain';
}

http.createServer(function(req, res) {
  var match = /^([^?]*)(\?.*)?$/.exec(req.url);
  var path = match[1].replace(/^\/*/, '').replace(/\/*$/, '');
  var query = match[2];
  if (path == '') path = 'index.html';
  if (false) {
  }
  else {
    path = __dirname + '/static/' + path;
    if (!fs.existsSync(path)) {
      res.writeHead(404, {'Content-Type': 'text/plain'});
      res.end('404: not found!');
    }
    else {
      fs.readFile(path, function(err, data) {
        if (err) {
          res.writeHead(500, {'Content-Type': 'text/plain'});
          res.end('500: internal error!');
        }
        else {
          res.writeHead(200, {'Content-Type': contentType(path)});
          res.end(data);
        }
      });
    }
  }
}).listen(80);
console.log('Started at port 80');
