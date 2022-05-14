var fs = require('fs') // this engine requires the fs module

function dTemplate(filePath, options, callback) { // define the template engine
  fs.readFile(filePath, function (err, content) {
    if (err) return callback(err)
    // this is an extremely simple template engine
    var rendered = content.toString()
      .replace('#title#', options.title)
      .replace('#message#', '<h1>' + options.message + '</h1>')
    return callback(null, rendered)
  })
}

exports.dTemplate = dTemplate;