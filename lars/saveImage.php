<?php
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

require_once 'config.php';
require_once 'database.php';

try {
    $id = uniqid();
    $date = date('Y-m-d H:i:s');
    $dataUrl = json_decode(file_get_contents('php://input'))->dataUrl;
    $base64Data = substr($dataUrl, strpos($dataUrl, ',') + 1);
    $imageData = base64_decode($base64Data);
    $filename = 'image_' . $id . '.png';
    $filePath = __DIR__ . '/image/' . $filename;
    $imagelink = "https://34932.hosts1.ma-cloud.nl/jaar2/php/image/" . $filename;

    if (file_put_contents($filePath, $imageData) !== false) {
        $connection = database_connect();

        $sql = 'INSERT INTO imgdata (link, date, code) VALUES (?,?,?)';
        error_log("SQL Query: " . $sql);

        $stmt = $connection->prepare($sql);
        if (!$stmt) {
            throw new Exception("Error in preparing the SQL statement: " . $connection->error);
        }

        $stmt->bind_param('sss', $imagelink, $date, $id);
        if (!$stmt->execute()) {
            throw new Exception("Error in executing the SQL statement: " . $stmt->error);
        }

        echo json_encode(['success' => true, 'message' => 'Photo saved successfully', 'filename' => $filename, 'imagelink' => $imagelink]);
    } else {
        throw new Exception('Failed to save the image.');
    }
} catch (Exception $e) {
    echo json_encode(['success' => false, 'error' => $e->getMessage()]);
}
