<?php
$data = file_get_contents("php://input");

$decodedData = json_decode($data, true);
if ($decodedData !== null) {
    $filePath = 'sensor_data.json';
    $jsonContent = json_encode($decodedData, JSON_PRETTY_PRINT);
    if ($jsonContent !== false) {
        if (file_put_contents($filePath, $jsonContent) !== false) {
            echo "Data successfully saved to $filePath.";
        } else {
            echo "Error saving data to $filePath.";
        }
    } else {
        echo "Error encoding JSON data for saving.";
    }
} else {
    echo "Error decoding JSON data: " . json_last_error_msg();
}