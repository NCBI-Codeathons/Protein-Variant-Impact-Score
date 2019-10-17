var http = require('http');
var https = require('https');

function parseQuery(query) {
  var ret = {};
  if (!query) return ret;
  if (query[0] == '?') query = query.substr(1);
  var args = query.split('&');
  for (var i = 0; i < args.length; i++) {
    var pair = args[i].split('=');
    ret[pair[0]] = pair[1];
  }
  return ret;
}

http.createServer(function(req, res) {
  var match = /^([^?]*)(\?.*)?$/.exec(req.url);
  var path = match[1].replace(/^\/*/, '').replace(/\/*$/, '');
  var query = parseQuery(match[2]);
  if (path == 'fetch') {
    if (query.id) {
      var url = 'https://eutils.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi?db=protein&rettype=fasta&retmode=text&id=' + query.id;
      https.get(url, function(resp) {
        var data = '';
        resp.on('data', (chunk) => {
          data += chunk;
        });
        resp.on('end', () => {
          res.writeHead(200, {'Content-Type': 'text/plain'});
          res.end(data);
        });
      }).on('error', function(err) {
        console.log('Error: ' + err.message);
        res.writeHead(404, {'Content-Type': 'text/plain'});
        res.end('404: not found!');
      });
    }
    else {
      console.log('Error: ID not defined');
      res.writeHead(404, {'Content-Type': 'text/plain'});
      res.end('404: not found!');
    }
  }
  else {
    console.log('Serving:', req.url);
    res.writeHead(200, {'Content-Type': 'text/plain'});
    res.end('Serving:' + req.url);
  }
}).listen(8000);
console.log('Started at port 8000');
