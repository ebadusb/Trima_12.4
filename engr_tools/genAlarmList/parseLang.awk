# set ts=4

# Parse a language file (string_xx-xx.info) to associate alarm
# body strings and status strings with their external string ids.

BEGIN { OFS = "@!@" } 

# Skip empty lines and comments
#
/^#/       { next }
/^[ \t]*$/ { next }

# Modify characters unacceptable to xml (and continue on)
#
           { gsub( /[<]/, "\\&lt;" )
	         gsub( /[>]/, "\\&gt;" )
           }

# String IDs are the first field and contain the word "text" or "TEXT_"
# 
$1 ~ /(text|TEXT_)/	 { external = $1
		   # Delete anything outside the double-quoted string
		   sub( /^[^\"]*\"/, "\"" )
		   sub( /\"[^"]*$/, "\"" )
		   # Remove the BeginSeg/EndSeg markers
		   gsub( /\#\!\[[BeginEd]*Seg [0-9]\]/, "" )
		   # Print the result line when we find it
		   print external, $0
		 }
