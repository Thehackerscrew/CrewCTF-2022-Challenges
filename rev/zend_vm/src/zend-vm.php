<?php
function ds($x) {
    for($i = 0; $i < strlen($x); $i++) {
        $x[$i] = chr((ord($x[$i]) ^ 13) - 37);
    }
    return $x;
}

if(ds("\x85\x99\x97\x9e\x94")(ds("\x81\x87\x94\x89\x83\x9e\x85\x9c\x97\x84\x87\x84\x89\x86\x83\x9c\x87\x95")()) > 1) {
    die(ds("\x7e\x99\x94\x48\x94\x80\x8b\x94\x48\x87\x8b\x95\x93\x4b"));
}

$h = ["\xcb\xb1\x84\xdd\x8e\x05\xc9\x70\x9e\x5d\xca\xed\xaa\x04\x95\xcf",
"\x60\x24\x65\xf4\xa2\xf9\x0d\xc9\xbd\x72\xba\x5e\x5a\x87\x17\xf9",
"\xd7\xe5\x4b\x0e\x44\x6d\x49\x16\xe4\xe2\x0c\xc1\x82\x83\xfb\x1e",
"\x50\x86\x28\x26\xab\x95\xfd\xe1\xf1\x43\x0f\x99\xe4\xfa\xfa\x0c",
"\x64\x71\x68\xc7\xed\x2e\x8b\x85\x39\xe9\xb3\xc7\x0a\x4b\x74\xa9",
"\x61\xd5\x58\xd4\x22\xf6\xa0\xf8\xd0\xbd\x2d\x38\x86\x02\xed\xc3",
"\x20\x88\xa7\xce\x16\x29\xd2\x17\x0b\x6d\x80\xa2\x48\xe8\x9c\x93",
"\x14\x82\x68\xd2\x09\xc0\x9b\x1b\x80\xb9\xaf\x0c\x72\x58\xd2\xed",
"\xbf\x10\x0a\xac\x2f\x2c\xca\x19\xf0\x96\x2e\x99\xb6\xaf\x00\x25",
"\xc1\x9a\xa6\xd0\x16\x0d\x40\x34\x67\x6d\xa8\x3f\x0e\xa1\xbd\xaa",
"\x6e\xc3\xca\xf6\x41\x27\xb1\x14\x27\x3d\xf2\x7b\x42\x10\xc1\x10",
"\x05\xbd\xd1\xcb\x07\x74\x24\x82\x81\x40\x68\x99\x79\x5a\x5d\x59",
"\xc7\x2b\x2e\x56\x7b\xa7\xc4\xf2\x70\xa1\x0b\x9c\x26\xff\xdb\x41",
"\x7a\xef\x5c\xb0\xdc\xcb\xff\x69\x69\xd1\x67\x7b\xc3\x1c\x10\xf4",
"\x1a\x97\xc7\xb6\x18\x4d\x7f\xf0\xa3\x37\x69\x63\x3c\x9f\xb3\xa2",
"\x4e\xa5\x9e\x3a\x1e\xfa\xd8\x72\x5a\x9a\x87\x14\xf2\x75\x0b\x80",
"\x16\xbc\x57\x25\x32\x4c\x93\x90\xdb\xb0\xa5\x30\x69\x4b\xa4\xaa",
"\xca\x52\xd5\xc8\x79\xf8\x9c\xd9\xe7\x9c\x1c\x00\x68\x63\x58\xb3",
"\xdc\x04\x08\xce\x2d\x55\x50\xea\xaf\x33\x4f\xb3\x2f\x78\x78\xd8",
"\x40\x0e\xee\xd8\xe0\xdc\x7b\xab\x89\x4c\xd6\x5f\xca\x7c\xfd\xca",
"\xad\x7e\xd1\xdb\xc6\xac\x28\x00\x9c\xe1\x67\xc8\x66\x9f\x51\x9c",
"\xd9\xab\x79\x31\xd0\x73\x7f\x3e\x44\xd7\x36\xc7\xd3\x5d\x32\xf0",
"\xe8\xf1\x82\x43\x60\x8b\x2b\x4a\xd2\xf4\x3b\x44\x70\x22\xa6\xae",
"\xab\x3e\x82\x8a\xec\xcb\xbe\x80\x66\xa0\xc4\x00\x8a\x04\xcb\x92",
"\x77\x30\xa2\x73\x83\x40\xa1\xf0\x9f\xa7\xf6\x1e\x23\xe6\x5a\x50",
"\x4b\x13\xc5\x1c\x6e\x37\x8e\x50\xe9\xb1\xd4\x34\x10\x14\xc9\x03",
"\xe3\xbf\xd8\xa8\x37\x68\x81\x66\x06\x82\x18\xfc\x94\xf0\xd2\x23",
"\xb5\x87\x5d\x56\x41\xc6\xa4\x6c\xd9\xc2\x9a\x4e\xf5\x19\x47\x0b",
"\xec\xed\x1f\xb0\x5a\xa5\x59\xf1\x8f\xc6\x4a\x3f\x83\x91\x82\xe5",
"\x9b\x25\xb1\x03\x8c\x8e\x15\x4d\x17\xb6\xef\x5c\xda\x74\xad\xa6",
"\x74\x1c\x09\x01\xcf\xb7\xb3\x12\x2d\xb4\x24\xde\xcf\xb0\xab\x7a",
"\xde\x61\x84\x14\xc8\xfe\xe6\x43\x0f\x8e\xaf\xa7\x32\x07\xb8\x73",
"\xa6\xd3\xc3\x91\xe9\xb6\xab\xb8\x72\xf9\xcf\x63\x94\xe1\xfa\xca",
"\x08\xde\x32\xa3\x86\x25\x40\x38\xcf\xc2\x6e\xe7\x1a\xa8\x67\x1a",
"\x7b\x7a\x7c\xc9\x9c\xfc\x3d\x6f\x71\xbf\x45\xbb\xc3\x65\xc1\x8e",
"\x21\x3c\x0c\x75\x02\x46\x2b\x0b\xe7\x14\x6d\xb5\x66\xc6\xce\xbf",
"\x5d\x86\x52\xd1\x13\xa3\x1f\xa9\x9b\x4a\xcd\xc6\x51\xc4\xf6\x65",
"\x10\xfd\xb2\x13\x98\xce\x5e\x3b\xf1\x20\x60\xe7\x0f\x2e\x11\x5c",
"\xb9\x6d\xfb\xb0\x27\x4f\x0f\xef\x4a\x3a\xd8\x00\xf4\xf6\xfc\xb6",
"\x0f\x28\x57\x27\x40\x1a\x74\x38\x94\xb0\xd1\x43\xcb\xa2\xeb\x1b",
"\x5a\x01\x93\x8b\xa5\xde\x9b\xad\x08\x8c\xd8\x7d\xcd\xad\x9b\xd1",
"\xf6\x55\x92\x5d\x1d\x3e\x5c\x7a\xa1\x40\x16\xa8\xbf\xc3\x86\x2e",
"\xeb\xa7\x04\x18\xdd\xe4\x07\xc0\x80\xe9\xd4\x52\xb3\x39\x9f\x49",
"\x64\xb4\x35\x6d\x1f\xeb\xf4\x0c\xad\xa8\xe6\xf9\xf9\x8d\x4f\x81",
"\xe8\xa9\x3c\xaf\xa6\xd6\x30\xa5\x80\x55\xba\x68\x7e\x59\x60\xa2",
"\x97\x1c\x68\xb2\x9d\x44\x89\xe5\x75\x33\x2a\x2d\xe3\x79\xf4\xf6",
"\x4b\xf1\x89\x29\x6b\x8e\xf6\x6d\x42\x1b\x73\x70\x31\xfb\xc5\x58",
"\xe2\x4a\x64\xc7\x8f\xb8\xbe\x18\x37\xd4\x5b\x38\x11\x6e\xf6\xf6"];

$i = ds("\x94\x9a\x83\x9f")(ds("\x9a\x87\x8b\x84\x9c\x83\x9e\x87")(ds("\x66\x9c\x8b\x81\x52\x48")));
if(ds("\x95\x94\x9a\x9c\x87\x9e")($i) != ds("\x85\x99\x97\x9e\x94")($h)) {
    die(ds("\x71\x9a\x99\x9e\x81\x4b\x22"));
}

$r = true;
for($j = ds("\x95\x94\x9a\x9c\x87\x9e")($i) - 1; $j >= 0; $j--) {
    $r &= (ds("\x9f\x84\x57")(ds("\x95\x97\x8a\x95\x94\x9a")($i, $j), true) == $h[strlen($i) - $j - 1]);
}

die($r ? ds("\x65\x99\x9a\x9a\x87\x85\x94\x4b\x22") : ds("\x71\x9a\x99\x9e\x81\x4b\x22"));
?>