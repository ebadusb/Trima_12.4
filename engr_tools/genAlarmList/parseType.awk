# Parse the loadAlarms.cpp file to associate each alarm name with its type,
# display flag, and dlog description (untranslated string). The structure
# was changed between versions 5 and 6 to add a new field relating to sounds
# before the (final) dlog description string.

BEGIN {}

# Get info from the add commands
/AlarmTable::add/	{
		  gsub( /[ \t]*,[ \t\"]*/, "," )
		  sub( /.*add\([ \t]*/, "" )
		  sub( /"[ \t]*);.*/, "" )
		  numFields = split( $0, sect, /,/ )
		  key   = sect[ 1 ]
		  type  = sect[ 2 ]
		  flag  = sect[ 4 ]
		  desc  = sect[ numFields ]
		  # Modify characters unacceptable to xml
   		  gsub( /[<]/, "\\&lt;", desc )
		  gsub( /[>]/, "\\&gt;", desc )
		  print key "," type "," flag "," desc
		}

# Nothing else is relevent
		{ next }
