var http = require('http');
var https = require('https');
var fs = require('fs');

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

var scripts = process.argv[2];
console.log('scripts at', scripts);

http.createServer(function(req, res) {
  var match = /^([^?]*)(\?.*)?$/.exec(req.url);
  var path = match[1].replace(/^\/*/, '').replace(/\/*$/, '');
  var query = parseQuery(match[2]);
  ////////////////////////////////////////////////
  if (path == 'fetch') {
    var fetch = {};
    if (query.id) {
      var url = 'https://eutils.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi?db=protein&rettype=fasta&retmode=text&id=' + query.id;
      https.get(url, function(resp) {
        var data = '';
        resp.on('data', (chunk) => {
          data += chunk;
        });
        resp.on('end', () => {
          res.writeHead(200, {'Content-Type': 'application/json'});
          var arr = data.split(/[\n\r]+/);
          fetch.title = arr[0];
          fetch.sequence = arr.slice(1).join('');
          res.end(JSON.stringify(fetch));
        });
      }).on('error', function(err) {
        console.log('Error: ' + err.message);
        res.writeHead(404, {'Content-Type': 'application/json'});
        fetch.title = '404: not found!';
        res.end(JSON.stringify(fetch));
      });
    }
    else {
      console.log('Error: ID not defined');
      res.writeHead(404, {'Content-Type': 'application/json'});
      fetch.title('404: not found!');
      res.end(JSON.stringify(fetch));
    }
  }
  ////////////////////////////////////////////////
  else if (path == 'score') {
    var score = {};
    path = scripts + '/fake-data.json';
    fs.readFile(path, function(err, data) {
      if (err) {
        console.log(err);
        res.writeHead(500, {'Content-Type': 'text/plain'});
        res.end('500: internal error!');
      }
      else {
        res.writeHead(200, {'Content-Type': 'application/json'});
        res.end(data);
      }
    });
  }
  ////////////////////////////////////////////////
  else {
    console.log('Serving:', req.url);
    res.writeHead(200, {'Content-Type': 'text/plain'});
    res.end('Serving:' + req.url);
  }
}).listen(8000);
console.log('Started at port 8000');
