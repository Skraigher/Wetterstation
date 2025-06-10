/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-web-server
 */

#ifndef WEBPAGE_H
#define WEBPAGE_H

const char* webpage = R"=====(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 Wetterstation</title>
</head>
<body>
    <h1>WETTERSTATION</h1>
    <h2>ESP32 Temperature</h2>
    <p>Temperature: <span style="color: red;"><span id="temperature">Loading...</span> &#8451;</span></p>
    <p>Humidity: <span style="color: red;"><span id="humidity">Loading...</span>%</span></p>
    <br>
    <h2>ESP32 GAS-WERTE</h2>
    <p>Value: <span style="color: blue;"><span id="sensorValue">Loading...</span></span></p>
    
    <p>Volt: <span style="color: blue;"><span id="sensorVolt">Loading...</span>V</span></p>
    
    <p>Ratio: <span style="color: blue;"><span id="sensorRatio">Loading...</span> </span></p>

    <div style="
        position: absolute;
        top: 10px;
        right: 10px;
        background-color: white;
        padding: 5px 10px;
        border-radius: 5px;
        box-shadow: 0 0 5px rgba(0,0,0,0.1);
    ">
         <p style="margin: 0;">ZEIT: <span style="color: black;" id="time">Loading...</span></p>
    </div>

    <h2>ESP32 LED Steuerung</h2>
    <button onclick="toggleLED()">LED Umschalten</button>
    <p>Status: <span id="ledStatus">Unbekannt</span></p>
    <script>
        function fetchTemperature() {
            fetch("/temperature")
                .then(response => response.text())
                .then(data => {
                    document.getElementById("temperature").textContent = data;
                });
        }
        function fetchHumidity() {
            fetch("/humidity")
                .then(response => response.text())
                .then(data => {
                    if(!isNaN(data)){
                      document.getElementById("humidity").textContent = data; 
                    }
                });
        }
        function fetchSensorValue() {
            fetch("/sensorValue")
                .then(response => response.text())
                .then(data => {
                    if(!isNaN(data)){
                      document.getElementById("sensorValue").textContent = data;
                    }
                });
        }
        function fetchSensorVolt() {
            fetch("/sensorVolt")
                .then(response => response.text())
                .then(data => {
                    if(!isNaN(data)){
                      document.getElementById("sensorVolt").textContent = data;
                    }
                });
        }
        function fetchSensorRatio() {
            fetch("/sensorRatio")
                .then(response => response.text())
                .then(data => {
                    if(!isNaN(data)){
                      document.getElementById("sensorRatio").textContent = data;
                    }
                });
        }
        

        let ledOn = true; // Anfangszustand: LED ist AN

        function toggleLED() {
            ledOn = !ledOn; // Zustand umschalten
            const state = ledOn ? "on" : "off"; // richtigen Zustand setzen

            fetch("/led?state=" + state)
                .then(response => response.text())
                .then(data => {
                    document.getElementById("ledStatus").textContent = data;
                });
        }

        function fetchTime() {
            fetch("/time")
            .then(response => response.text())
            .then(data => {
            document.getElementById("time").textContent = data;
        });
}

        // Status beim Laden abfragen
        window.onload = () => {
             fetch("/ledStatus")
            .then(response => response.text())
            .then(data => {
                data = data.trim().toLowerCase(); // unnötige Leerzeichen entfernen, klein schreiben
                ledOn = (data === "on"); // Status setzen
                document.getElementById("ledStatus").textContent = ledOn ? "LED ist AN" : "LED ist AUS";
        });
};


        


        fetchTemperature();
        setInterval(fetchHumidity, 5000);
        setInterval(fetchSensorValue, 10000);
        setInterval(fetchSensorVolt, 10000);
        setInterval(fetchSensorRatio, 10000);
        setInterval(fetchTime, 5000); 
        setInterval(fetchTemperature, 5000); // Update temperature every 4 seconds
    </script>

</body>
</html>
)=====";

#endif