# Web server configuration

Static content is served from `public_html` by Apache2 (that is preinstalled on GCP Debian instance)

AJAX api is served by Node.js script at port 8000.

To enable both Node.js and Apache on the same server, add the following lines in `/etc/apache2/apache2.conf`:

    LoadModule proxy_module /usr/lib/apache2/modules/mod_proxy.so
    LoadModule proxy_http_module /usr/lib/apache2/modules/mod_proxy_http.so
    ProxyPass /ajax http://localhost:8000

Then run `sudo systemctl restart apache2` and start the Node script from this directory: `node index.js &`.