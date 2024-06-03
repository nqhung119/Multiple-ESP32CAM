const socket = io();

let desiredWidth = 488.4;
let desiredHeight = 200;

socket.on("webcam1_io", function(data_received) {
    let base64Data = data_received;
    const img = document.getElementById("esp1");
    const image = new Image();
    image.src = "data:image/jpeg;base64," + base64Data;
    image.onload = function() {
        img.width = desiredWidth;
        img.height = desiredHeight;
        img.src = image.src;
    };
});
socket.on("webcam2_io", function(data_received) {
    let base64Data = data_received;
    const img = document.getElementById("esp2");
    const image = new Image();
    image.src = "data:image/jpeg;base64," + base64Data;
    image.onload = function() {
        img.width = desiredWidth;
        img.height = desiredHeight;
        img.src = image.src;
    };
});
socket.on("webcam3_io", function(data_received) {
    let base64Data = data_received;
    const img = document.getElementById("esp3");
    const image = new Image();
    image.src = "data:image/jpeg;base64," + base64Data;
    image.onload = function() {
        img.width = desiredWidth;
        img.height = desiredHeight;
        img.src = image.src;
    };
});
socket.on("webcam4_io", function(data_received) {
    let base64Data = data_received;
    const img = document.getElementById("esp4");
    const image = new Image();
    image.src = "data:image/jpeg;base64," + base64Data;
    image.onload = function() {
        img.width = desiredWidth;
        img.height = desiredHeight;
        img.src = image.src;
    };
});

document.getElementById('esp1').addEventListener('click', function() {
    this.width = 1022.4;
    this.height = 520;
    desiredWidth = 1022.4;
    desiredHeight = 520;

    document.getElementById('camera-2').style.display = 'none';
    document.getElementById('camera-3').style.display = 'none';
    document.getElementById('camera-4').style.display = 'none';

    var camera1 = document.getElementById('camera-1');
    camera1.classList.remove('col-6');
    camera1.classList.add('col-12');

    document.getElementById('camera3-4').style.display = 'none';
});

document.getElementById('esp2').addEventListener('click', function() {
    this.width = 1022.4;
    this.height = 520;
    desiredWidth = 1022.4;
    desiredHeight = 520;

    document.getElementById('camera-1').style.display = 'none';
    document.getElementById('camera-3').style.display = 'none';
    document.getElementById('camera-4').style.display = 'none';

    var camera2 = document.getElementById('camera-2');
    camera2.classList.remove('col-6');
    camera2.classList.add('col-12');

    document.getElementById('camera3-4').style.display = 'none';
});

document.getElementById('esp3').addEventListener('click', function() {
    this.width = 1022.4;
    this.height = 520;
    desiredWidth = 1022.4;
    desiredHeight = 520;

    document.getElementById('camera-1').style.display = 'none';
    document.getElementById('camera-2').style.display = 'none';
    document.getElementById('camera-4').style.display = 'none';

    var camera3 = document.getElementById('camera-3');
    camera3.classList.remove('col-6');
    camera3.classList.add('col-12');

    document.getElementById('camera1-2').style.display = 'none';
});

document.getElementById('esp4').addEventListener('click', function() {
    this.width = 1022.4;
    this.height = 520;
    desiredWidth = 1022.4;
    desiredHeight = 520;

    document.getElementById('camera-1').style.display = 'none';
    document.getElementById('camera-2').style.display = 'none';
    document.getElementById('camera-3').style.display = 'none';

    var camera4 = document.getElementById('camera-4');
    camera4.classList.remove('col-6');
    camera4.classList.add('col-12');

    document.getElementById('camera1-2').style.display = 'none';
});

document.querySelectorAll('[id^="esp"]').forEach(item => {
    item.addEventListener('click', function() {
        document.getElementById('back').style.display = 'block';
    });
});

document.addEventListener('DOMContentLoaded', function() {
    document.getElementById('back').style.display = 'none';
});


$("#tangtoc").click(function () {
    socket.emit("tangtoc-clicked");
});
$("#sangtrai").click(function () {
    socket.emit("sangtrai-clicked");
});
$("#sangphai").click(function () {
    socket.emit("sangphai-clicked");
});
$("#giamtoc").click(function () {
    socket.emit("giamtoc-clicked");
});
$("#phanh").click(function () {
    socket.emit("phanh-clicked");
});