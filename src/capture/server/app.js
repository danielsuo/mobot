// TODO: Refactor to handle multiple devices

var fs = require('fs');
var net = require('net');
var mkdirp = require('mkdirp');

var TCP_PORT = 8124;
var NUM_PATH_BYTES = 3;
var NUM_FILE_BYTES = 10;

var FILE_TYPE, // Directory (0) or file (1)
  FILE_INDEX, // Byte index in current file
  FILE_LENGTH, // Size of current file in bytes
  PATH, // Write path of current file
  BUFFER, // Byte buffer
  COUNTER;

var timeout;

var resetServer = function() {
  FILE_INDEX = FILE_LENGTH = FILE_TYPE = 0;
  PATH = '';
  BUFFER = [];
};

var keepAlive = function() {
  clearTimeout(timeout);
  timeout = setTimeout(function() {
    console.log('INFO: Resetting TCP server variables');
    resetServer();
  }, 5000);
};

var TCPServer = net.createServer(function(c) { //'connection' listener
  console.log('client connected');
  c.on('end', function() {
    console.log('client disconnected');
  });

  // TODO: should reset indices after some amount of time waiting
  c.on('data', function(data) {
    keepAlive();

    // If we have any data left over from the previous read, append it to the
    // beginning.
    if (BUFFER && BUFFER.length > 0) {
      data = Buffer.concat([BUFFER, data]);
      BUFFER = [];
    }

    var DATA_INDEX = 0;

    // If we are starting a new file, grab the new file's metadata
    if (FILE_INDEX === FILE_LENGTH) {
      resetServer();

      FILE_INDEX = 0;

      // Get file type
      FILE_TYPE = parseInt(data.slice(DATA_INDEX, DATA_INDEX + 1));
      DATA_INDEX += 1;

      // Get length of the file path
      var PATH_LENGTH = parseInt(data.slice(DATA_INDEX, DATA_INDEX + NUM_PATH_BYTES));
      DATA_INDEX += NUM_PATH_BYTES;

      // Get the file path
      PATH = data.slice(DATA_INDEX, DATA_INDEX + PATH_LENGTH).toString();
      DATA_INDEX += PATH_LENGTH;

      // Get the length of the file
      FILE_LENGTH = parseInt(data.slice(DATA_INDEX, DATA_INDEX + NUM_FILE_BYTES));
      DATA_INDEX += NUM_FILE_BYTES;

      if (FILE_TYPE === 0) {
        console.log('INFO: Writing new directory at ' + PATH);
      } else if (FILE_TYPE === 1) {
        console.log('INFO: Writing new file at ' + PATH);
      }
    }

    // If we're writing a new file, FILE_INDEX is 0, so we write the lesser of
    // the file's length and the amount of data left in the buffer. If we're
    // continuing a file, DATA_INDEX is 0, so we write the lesser of the
    // remaining file length or the amount of data in the buffer.
    var DATA_LENGTH = Math.min(FILE_LENGTH - FILE_INDEX, data.length - DATA_INDEX);

    if (FILE_TYPE === 0) {
      mkdirp.sync(PATH);
    } else if (FILE_TYPE === 1) {
      // Write the data to file
      fs.appendFileSync(PATH, data.slice(DATA_INDEX, DATA_INDEX + DATA_LENGTH));
    }

    DATA_INDEX += DATA_LENGTH;
    FILE_INDEX += DATA_LENGTH;

    // If we've finished a file and there is data left overe
    if (FILE_INDEX === FILE_LENGTH && DATA_INDEX < data.length) {
      BUFFER = data.slice(DATA_INDEX, data.length);
    }
  });

  c.on('error', function(e) {
    if (e.code == 'EADDRINUSE') {
      console.log('Address in use, retrying...');
      setTimeout(function() {
        TCPServer.close();
        var HTTP_PORT = 8080;
        TCPServer.listen(TCP_PORT, HOST);
      }, 1000);
    }
  });
});

// Listen on PORT
TCPServer.listen(TCP_PORT, function() {
  console.log('TCPServer bound');
});