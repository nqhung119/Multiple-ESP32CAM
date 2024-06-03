const express = require('express');
const app = express();
const ejs = require('ejs');
const mqtt = require('mqtt');
const WebSocket = require('ws');

var port = 8500;
const wss = new WebSocket.Server({ port: 8501 });


app.set("view engine", "ejs");
app.get("/", function(req, res) {
    res.render("index");    
});
app.use('/public', express.static('public'));

var server = require("http").Server(app);
var io = require("socket.io")(server);
server.listen(port);

const options = {
    username: '',
    password: '',
    rejectUnauthorized: false
};

const client = mqtt.connect('mqtt://127.0.0.1:1883', options); 

client.on('connect', function(){ 
    console.log("Da ket noi MQTT Broker")
    client.subscribe("esp32cam1")
    client.subscribe("esp32cam2")
    client.subscribe("esp32cam3")
    client.subscribe("esp32cam4")
})

client.on('message', async function (topic, message) {
  if (topic === "esp32cam1") {
    const webcam1_data = message.toString();
    io.emit("webcam1_io", webcam1_data);
  }

  else if (topic === "esp32cam2") {
    const webcam2_data = message.toString();
    io.emit("webcam2_io", webcam2_data);
  }

  else if (topic === "esp32cam3") {
    const webcam3_data = message.toString();
    io.emit("webcam3_io", webcam3_data);
  }

  else if (topic === "esp32cam4") {
    const webcam4_data = message.toString();
    io.emit("webcam4_io", webcam4_data);
  }
});

io.on("connection", function (socket) {
  socket.on("tangtoc-clicked", function () {
    client.publish("control", "tangtoc");
    console.log("Da gui lenh tang toc do.");
  });
  socket.on("sangtrai-clicked", function () {
    client.publish("control", "sangtrai");
    console.log("Da gui lenh sang trai.");
  });
  socket.on("sangphai-clicked", function () {
    client.publish("control", "sangphai");
    console.log("Da gui lenh sang phai.");
  });
  socket.on("giamtoc-clicked", function () {
    client.publish("control", "giamtoc");
    console.log("Da gui lenh giam toc.");
  });
  socket.on("phanh-clicked", function () {
    client.publish("control", "phanh");
    console.log("Da gui lenh phanh.");
  });
});

io.on("connection", function (socket) {
  socket.on("direction-io", async function (switchValue) {
    const mqttMessage = switchValue;
    if (mqttMessage == "tien") {
      console.log("Trang thai di tien.");
    }
    else if (mqttMessage == "lui") {
      console.log("Trang thai di lui.");
    }
    client.publish("directioncontrol", mqttMessage);
  });
});

io.on("connection", function (socket) {
  socket.on("flash-io", async function (switchValue) {
    const mqttMessage = switchValue;
    if (mqttMessage == "bat") {
      console.log("Da bat den flash.");
    }
    else if (mqttMessage == "tat") {
      console.log("Da tat den flash.");
    }
    client.publish("flashcontrol", mqttMessage);
  });
});

console.log("Website co dia chi:");
console.log(`http://localhost:${port}`);
