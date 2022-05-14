const template = require('./template-engine.js');
const request = require('request');

const { Client } = require('@elastic/elasticsearch');
const client = new Client({node: 'http://192.168.1.7:9200'});
const path = require('path');
const express = require('express');
const app = express();
const port = 3000;

app.engine('ntl', template.dTemplate);
app.set('views', './views'); // specify the views directory
app.set('view engine', 'ntl'); // register the template engine

app.use('/data', express.static(path.join(__dirname, 'data')));

app.get('/', function (req, res) {
  res.render('index', { title: 'Irrigation System 2.0', message: 'Hello there!' })
});

app.get(/^\/zone\/([0-3])\/(start|stop)$/, function (req, res) {
  
res.send("You want to " + req.params[1] + " zone " + req.params[0]);

console.log('http://192.168.1.20/zone/' + req.params[1] + '?id=' + req.params[0]);

request.post('http://192.168.1.20/zone/' + req.params[1] + '?id=' + req.params[0], 
  function optionalCallback(err, httpResponse, body) {
    if (err) {
      return console.error('Operation Failed:', err);
    }
    console.log('Nice!', body);
  });
});

app.listen(port, () => {
  console.log(`Example app listening at http://localhost:${port}`)
})

var aa;

async function main() {
  console.log("aa");

  // promise API
  const result = await client.search({
    index: 'irrigation*',
    body: {
      query: {
        "bool": {
          "must": [],
          "filter": [
            {
              "match_all": {}
            },
            {
              "range": {
                "@timestamp": {
                  "gte": "now-1m",
                  "lte": "now",
                  "format": "strict_date_optional_time"
                }
              }
            }
          ],
          "should": [],
          "must_not": []
        }
      }
    }
  });

  aa = result.body;

}

//main();