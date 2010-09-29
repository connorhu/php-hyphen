<?php

$string = 'login';
$dic = hyphen_open('/path/to/hyph.dic');
echo hyphen_word($dic, $string);
hyphen_close($dic);

?>