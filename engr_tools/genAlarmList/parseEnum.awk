#! /usr/env gawk

#################################################################################
# Parse a file containing a C/C++ enum named enum_name.
#
# Note: if the enum in question has #if/#else/#endif directives in the body,
#       then you should use the output from compiler pre-processor!
#
BEGIN {
    errorOut = "/dev/stderr"
    enum_regex = "enum +\\<" enum_name "\\>"
    val = 0
    inEnum = 0
    kComments = 0

    # Check for required variable: enum_name
    if (! enum_name)
    {
        print "Need: -v enum_name=XXX" > errorOut
        exit 1
    }
}

#################################################################################
# Find the start of the enum
#
$0 ~ enum_regex {
    ## print "Begin: " $0
    inEnum = 1
    next
}

# Skip everything until the start is found
#
inEnum == 0 { next }

# Exit when we see the end of the enum
#
$0 ~ "};" {
    ## print "Done: ", enum_name
    exit
}

#################################################################################
# Skip empty lines, comments, comment blocks, and opening curly braces
#
/^[ \t]*$/    { next }
/^[ \t]*\/\// { next }

# This skips a pre-processor directive: #if, #else, #endif, #line, etc.
/^[ \t]*#/ { next }

# This marks the start of a C-style comment block: /* ....
#
/^[ \t]*\/\*/ { 
    kComments++
    ## printf("%d:%d##  %s\n", NR, kComments, $0)
    next
}

# This looks for the end of a C-style comment block: ... */
#
kComments > 0 { 
   kComments++
   ## printf("%d:%d##  %s\n", NR, kComments, $0)
   if ($0 ~ /.*\*\/[ \t]*$/) kComments=0
   next
}

# This looks for the enum's opening brace
#
/{/ {
    if (1 < NF) print "ERR! opening brace not alone on line " NR ": " $0 > errorOut
    next
}

#################################################################################
# Process the enum body
#
inEnum == 1 {
    sub( /,.*/, "" )                    # remove trailing comma, comment
    sub( /^[ \t]*/, "" )                # remove leading blanks
    nf = split($0, nv, /[ \t]*=[ \t]*/) # get assigned value, if provided
    if (nf == 1)
    {
        name = $1
    }
    else if (2 == nf)                   # Handle an explicit/relative assignment
    {
        name = nv[1]
        v = nv[2]

        # check for simple assignment case: NAME_2 = NAME_1
        #
        if (v in enum_map)
        {
            val = enum_map[v]
        }
        #
        # check for offset assignment using a decimal number:
        #  NAME_2 = (NAME_1 + 2)
        #  NAME_3 = NAME_2+5
        #
        else if ( match(v, /.*(\<.*\>)[ \t]*([+-])[ \t]*([[:digit:]]+)/, opers) )
        {
           lhs = opers[1]
           op  = opers[2]
           rhs = opers[3] + 0
           if (lhs in enum_map)
           {
              if (op == "+") val = enum_map[lhs] + rhs
              if (op == "-") val = enum_map[lhs] - rhs
              ## printf("FYI: v=%s -> val=%d\n", v, val) > errorOut
           }
        }
        # 
        # check for explicit decimal or hex value assignment:
        #  NAME_1 = 10
        #  NAME_2 = 0xFF
        #
        else if (v ~ /^[[:digit:]]+$/ || v ~ /^0x[[:xdigit:]]+$/)
        {
            val = strtonum(v)
        }
        else
        {
            print "ERR! cannot parse expression: " FILENAME ":" NR ": " $0 > errorOut
        }
    }
    print val "," name            # output enum name/value pair as: val,name 
    enum_map[name] = val          # save this entry
    val++                         # increment the numeric value for nominal case
}

#################################################################################
# Done. Exit with error code if the enum was not found
#
END { if (inEnum == 0) exit 1 }
