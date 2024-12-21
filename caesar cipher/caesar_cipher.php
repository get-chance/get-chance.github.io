<?php
// Caesar Cipher 암호화 함수
function caesar_cipher($text, $shift) {
    $ciphertext = "";
    $shift = $shift % 26; // 알파벳 순환

    foreach (str_split($text) as $char) {
        // 알파벳 대문자 처리
        if (ctype_upper($char)) {
            $ciphertext .= chr(((ord($char) - 65 + $shift) % 26) + 65);
        }
        // 알파벳 소문자 처리
        elseif (ctype_lower($char)) {
            $ciphertext .= chr(((ord($char) - 97 + $shift) % 26) + 97);
        }
        // 공백 및 기타 문자 처리
        else {
            $ciphertext .= $char;
        }
    }

    return $ciphertext;
}

// 사용자 입력 처리
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $shift = intval($_POST["shift"]);
    $plaintext = $_POST["plaintext"];

    $ciphertext = caesar_cipher($plaintext, $shift);

    echo "<h1>암호화 결과</h1>";
    echo "<p><strong>평문 :</strong> $plaintext</p>";
    echo "<p><strong>암호문 :</strong> $ciphertext</p>";
}
?>