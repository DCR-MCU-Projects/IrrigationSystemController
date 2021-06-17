const { Client } = require('@elastic/elasticsearch')
const client = new Client({
  node: 'http://192.168.1.7:9200'
})

const express = require('express')
const app = express()
const port = 3000

app.get('/', (req, res) => {

  res.send(aa);
})

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
                  "gte": "2021-06-12T03:52:19.692Z",
                  "lte": "2021-06-12T03:53:19.692Z",
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

main();