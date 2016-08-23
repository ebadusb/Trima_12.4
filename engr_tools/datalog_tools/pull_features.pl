#!/usr/bin/python

#import os
import sys
import re
#import math

def help() :

    print '''Usage: pull_features [INPUT] [OUTPUT]
where INPUT is a DLog file, translated into CSV (comma-separated-value)
format, and OUTPUT is the name of the output file (defaults to
features_pulled.txt).
'''

    sys.exit(1)

# Given a line from the dlog, parse to see if it's of interest.  There
# are two cases: it's a section name, it's a field definition, or it's
# not of interest.  We only want definitions from the FEATURES and GUI
# sections, so we keep track of what section we're in.  Definitions
# are gathered into the following global lists so they can be sorted
# and output alphabetically.
current_section = ''
defns = { '[GUI]' : [], '[FEATURES]' : [] }

def parseline(line) :
    global current_section
    global defns

    # regex for definitions.  This matches a line like:
    #  ..config data: name=value <some number of hex digits>
    defn_re     = re.compile(r"(.*)config data: (.*=\w*\s{1}[0-9a-f]{6,8})\s*");
    # regex for section name
    section_re  = re.compile(r".*config data: (\[\w+\])")
    
    # Look for section headers
    sec = section_re.match(line)
    if sec :
        current_section = sec.group(1)
        if current_section == "[GUI]" or current_section == "[FEATURES]" :
            print "found section", sec.group(1)

    elif current_section == "[GUI]" or current_section == "[FEATURES]" :
        # if not a section header, but we're in a section of interest,
        # look for definitions

        d = defn_re.match(line)
        if (d) :
            # if it's a definition, add to the dictionary ... however,
            # for reasons unknown, the CRC value is separated by a
            # space, not a comma.  There should be only one space in the matched group.
            def_w_comma = re.sub(' ', ',', d.group(2))
            defns[current_section].append(def_w_comma)


initial_text = '''# features.bin - software options for Trima
#
# 1.) This file must have lines that are less then 255 characters long in order
#     for the name/value to be read properly.
#
# 2.) NO SPACES ARE ALLOWED BETWEEN name=value!
#
# 3.) Each entry (or VALUE) is completely specified by SECTION_NAME and NAME.
#     The format for data which is to be seen by the computer is as follows:
#
# [SECTION_NAME]
# name0=value0
# name1=value1
# name2=value2
# ...
# [SECTION_NAME1]
# name0=value0
# ...
# NO SPACES ARE ALLOWED BETWEEN name=value

##################   VERSION NUMBER  #################################
[OPTIONS]
#
# The following version number must be changed if the format of this
# file changes, allowing install or downgrade scripts to determine
# when file conversion is necessary.
#
#
# Japan Default:
#
file_version=0.0.0.0
#######################################################################

[FileInfo]
ReadOnly=true
FileName="/config/features.bin"
ServiceChangeable=false
'''

def write_section(out, sec) :
    out.write('\n' + sec + '\n')
    for x in sorted(defns[sec]):
        out.write(x + '\n')

def write_features(fname) :
    outf = open(fname , 'w')
    # First, write a bunch of "boilerplate".  We don't attempt to
    # recreate all the comments, but we can recreate some of them.
    outf.write(initial_text)
    write_section(outf, '[FEATURES]')
    write_section(outf, '[GUI]')

  

    



######################################################################


###  Command line args and get started
try :
    filename = sys.argv[1]
except:
    "Please specify at least an input file\n";
    help()

print "Reading from file", filename

outname = 'features_pulled.txt'
if len(sys.argv) >= 3 :
    outname = sys.argv[2]

print "Writing to file", outname

file = open(filename, 'r')

## Special DLog data fields
log_file = ""
trima_build = ""
log_data = ""
headings1 = []
headings2 = []
#dlog = []


######################################################################
#
#  Read the DLOG
line_no = 0
for line in file :

    # First 8 lines are special
    if (line_no < 8) :

        fields = line.split(',');
        if line_no == 0 :
            pass  # confidentiality line, disregard
        elif line_no == 1 :
            log_file = fields[0] # log_file
            print log_file
        elif line_no == 2 :
            trima_build = fields[0]
        elif (line_no == 3) or (line_no == 4 ) :
            pass # empty line
        elif line_no == 5 :
            log_data = fields[0]
        elif line_no == 6 :
            headings1 = fields
        elif line_no == 7 :
            headings2 = fields

    # Process the bulk of the data file
    else :
        fields =  line.split(',', len(headings1) )
        #dlog.append(fields)
        parseline(fields[-1])

    line_no+= 1

print "Scanned", line_no, "lines"

for x in defns.keys() :
    print "Section", x
    for y in defns[x]:
        print "\t", y

write_features(outname)
