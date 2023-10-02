               // Import the functions you need from the SDKs you need
import { initializeApp } from "https://www.gstatic.com/firebasejs/9.19.1/firebase-app.js";
import { getDatabase, ref, set, get, child, onValue } from "https://www.gstatic.com/firebasejs/9.19.1/firebase-database.js";
     
     // Your web app's Firebase configuration
               
const firebaseConfig = {
    apiKey: "AIzaSyCY8LSptVjsMtrqMv-m4w2hGo-Ncy8zNmc",
    authDomain: "smartelectricityprovider.firebaseapp.com",
    databaseURL: "https://smartelectricityprovider-default-rtdb.asia-southeast1.firebasedatabase.app",
    projectId: "smartelectricityprovider",
    storageBucket: "smartelectricityprovider.appspot.com",
    messagingSenderId: "146805007700",
    appId: "1:146805007700:web:9f1653d16ce45f93b494c1",
    measurementId: "G-Y30N3KTHR8"
};
   
     // Initialize Firebase
const app = initializeApp(firebaseConfig);
var display = "";
// const db = getDatabase(app);
      //get ref to database services=
 
const db = getDatabase();
const ReadArduino = ref(db, 'Sensor1/WebRead');
   
onValue(ReadArduino, (snapshot) => {
const data = snapshot.val();
var  smth = false;
   
   
display = "<tr><th>Ngày-Giờ</th><th>Tổng lượng điện tiêu thụ trong ngày</th></tr>";
   
var keys = Object.keys(data);

for (var i = 0; i < keys.length; i++) {
    var val = data[keys[i]];
    if(keys[i] != "isOn") {
        display += "<tr><th>" + keys[i] + "</th><th>" + (val / 1000 / 3600).toFixed(5) + " kW/h</th></tr>";
    } else {
        console.log(val);
        smth = val
    }
}
   
var isOnQ = !smth;
   
if(smth == true) {
    document.getElementById("submit").value = "Turn Off";
    document.getElementById("status").innerText = "Tắt";
} else {
    document.getElementById("submit").value = "Turn On";
    document.getElementById("status").innerText = "Bật";
}
   
document.getElementById('myTable').innerHTML = display;
   
document.getElementById("submit").addEventListener('click', function(e){
    e.preventDefault();
    set(ref(db, 'Sensor1/ArduinoRead/isOn'), isOnQ);
})
   
});
   