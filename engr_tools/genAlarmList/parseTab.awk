# Parse the alarmtb.cpp file to associate alarm ids with their types and language
# string ids (alarm body and status string). This file will work for both versions 
# 5 and 6, because the ALARM_TABLE definition differs only in fields beyond those
# of interest.

BEGIN		{ skip = 1; collect = 0 }

# Find the start of the table, and skip to the first member

/ALARM_TABLE_T[ \t]*alarms/ {
		  while(( 1 == skip  ) && ( getline > 0 ))
		  {
		    if( match( $0, "{" ))
		    {
		      skip = 0
		    }
		  }
		  next
		}

# Recognize the start of an entry
/\{/		{ collect = 1; next }

# Skip over the extraneous lines
skip == 1	{ next }

# Collect the fields of interest from the current entry
collect == 1	{ 
		  id = $1
		  sub( /,.*/, "", id )
		  if( "NULL_ALARM" == id )
		  {
		    exit
		  }
		  collect = 0
		  getline			# skip NAME_LITERAL
		  getline			# skip display info
		  getline aBody			# get the alarm body id
		  if( match( aBody, /},/ )) next
		  sub( /,.*/, "", aBody )	# ... drop the comma, any comments
		  sub( /[ \t]*/, "", aBody )	# ... drop leading space
		  getline sBody			# get the status line id
		  sub( /,.*/, "", sBody )	# ... drop the comma, any comments
		  sub( /[ \t]*/, "", sBody )	# ... drop leading space
		  getline bitmap		# get the optional bitmap line
		  sub( /,.*/, "", bitmap )	# ... drop the comma, any comments
		  sub( /[ \t]*/, "", bitmap )	# ... drop leading space
		  if( bitmap == "}" ) bitmap = "BITMAP_NULL"
		  printf "%s,%s,%s,%s\n", id, aBody, sBody, bitmap
		  ## printf "%s,%s,%s,%s\n", id, aBody, sBody, bitmap > "bitmaps.out"
		}

# Ignore anything until the next member
		{ next }
