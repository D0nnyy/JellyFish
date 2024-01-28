const width = 640, height = 480,
    canvas = document.getElementById("canvas"),
    ctx = canvas.getContext('2d');

let video = null, isSetup = false;
let firstPhoto = 0;

Setup();
setInterval(Setup, 1000);

function Setup() {
    if (!isSetup) {
        canvas.width = width;
        canvas.height = height;

        canvas.style.width = `${width}px`;
        canvas.style.height = `${height}px`;
        canvas.style.backgroundColor = 'red';

        video = document.getElementById("video");

        fetch('sensor_data.json')
            .then(response => response.json())
            .then(sensorData => {
                console.log("sensor data:", sensorData);
                if (sensorData.SoundDetected === 1) {
                    takePhoto();
                }
            })
            .catch((err) => {
                console.error(`Error:\n ${err}`);
            });

        navigator.mediaDevices
            .getUserMedia({ video: true, audio: false })
            .then((stream) => {
                video.srcObject = stream;
                video.play();
            }).catch((err) => {
                console.error(`Error obtaining video stream:\n ${err}`);
            });

        isSetup = true;
        setTimeout(() => {
            isSetup = false;
        }, 4000);
    }
}

function takePhoto() {
    requestAnimationFrame(() => {
        ctx.drawImage(video, 0, 0, width, height);
        const dataUrl = canvas.toDataURL('image');
        savePhoto(dataUrl);
    });
}

function savePhoto(dataUrl) {
    fetch('/jaar2/php/saveImage.php', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json', },
        body: JSON.stringify({ dataUrl }),
    })
        .then(response => response.json())
        .then(result => {
            let jsonResponse = result;
            console.log(jsonResponse);
            if (firstPhoto == 0) {
                firstPhoto += 1;
                qrcode = new QRCode(document.getElementById("qrcode"), jsonResponse.imagelink);
            } else {
                qrcode.clear();
                qrcode.makeCode(jsonResponse.imagelink);
            }
        })
        .catch(error => { console.error('Error saving photo:', error); });
}