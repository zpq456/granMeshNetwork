<?php

$servername = "localhost";

// REPLACE with your Database name
$dbname = "kimchi_db";
// REPLACE with Database user
$username = "kimchi_user";
// REPLACE with Database user password
$password = "rlaclWkd@1122";

// Keep this API Key value to be compatible with the ESP32 code provided in the project page. 
// If you change this value, the ESP32 sketch needs to match
$api_key_value = "tPmAT5Ab3j7F9";

$api_key= $serial = $temp1 = $temp2 = $temp3 = $temp4 = "";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $api_key = test_input($_POST["api_key"]);
    if($api_key == $api_key_value) {
        $serial = test_input($_POST['serial']);
        $temp1 = test_input($_POST['temp1']);
        $temp2 = test_input($_POST['temp2']);
        $temp3 = test_input($_POST['temp3']);
        $temp4 = test_input($_POST['temp4']);
        
        // Create connection
        $conn = new mysqli($servername, $username, $password, $dbname);
        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } 
        
        $sql = "INSERT INTO MasterDI4DO4 (serial, temp1, temp2, temp3, temp4) VALUES ('" . $serial . "', '" . $temp1 . "', '" . $temp2 ."', '" . $temp3 ."', '" . $temp4 . "')";
        
        if ($conn->query($sql) === TRUE) {
            echo "New record created successfully";
        } 
        else {
            echo "Error: " . $sql . "<br>" . $conn->error;
        }
    
        $conn->close();
    }
    else {
        echo "Wrong API Key provided.";
    }

}
else {
    echo "No data posted with HTTP POST.";
}

function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}