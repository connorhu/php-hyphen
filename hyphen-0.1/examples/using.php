<?php

$start = memory_get_usage(true);
$time_start = microtime(true);

$string = 'bejelentkezés';
$dic = hyphen_open('/usr/share/hyphen/hyph_hu_HU.dic');

for ($i=0; $i < 10000000; $i++) { 
    hyphen_word($dic, $string);
}

hyphen_close($dic);

$end = memory_get_usage(true);
$time_end = microtime(true);
var_dump($end - $start);
var_dump($time_end - $time_start);

