BEGIN { cnt_act = cnt_exp = 0 }

# Sample record from $LANGDIR/string.info:
#
# languageCrc "0xd3e783d8" 0 0 0 0x0001 0 0 0 0 18 0 approved
#
/languageCrc/ {
  crc = gensub(/"/, "", "g", $2)
  sub(/0x/, "", crc)
  # Convert filename to Unix style: '\' to '/'
  filename = gensub("\\\\", "/", "g", FILENAME)
  # print "languageCrc: filename is: " filename
  actual[ filename ] = crc
  cnt_act += 1

  # Done with this language file
  nextfile
}

# Sample output from stringCrcGen:
#
# Crc for C:\Dev\tip63\Trima\Trima_Languages\11.0/string.info:d3e783d8
# Crc for C:\Dev\tip63\Trima\Trima_Languages\11.0/string_ja-jp.info:142c58fb
#
/Crc for/ {
  fc = $3
  sub(/.info:/, ".info ", fc)
  n = split(fc, a)
  file = a[1]
  crc  = a[2]
  # Convert filename to Unix style: '\' to '/'
  filename = gensub("\\\\", "/", "g", file)
  # print "CrcGenerator filename is: " filename
  expected[ filename ] = crc
  cnt_exp += 1
}

END {
  err = 0
  for ( f in actual ) {
    if ( actual[f] != expected[f] ) {
      printf("CRC Mis-match in %s : actual='%s' expected='%s'\n",
             f, actual[f], expected[f]);
      err += 1
    }
  }

  if ( err == 0 ) {
    if ( cnt_act == 0 || cnt_act != cnt_exp ) {
      printf("Did not find the expected CRCs! counts: actual=%d expected=%d\n",
             cnt_act, cnt_exp);
      err = -1
    }
  }

  exit err
}
    
  
    
  
  
   
  
  
