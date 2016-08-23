#!/usr/bin/env python
'''
Generates an XML file-to-author mapping for use with Parasoft. The files to
map are set by: 1) auto-generate an input file for all C/C++ files,
2) a specified input file list, or 3) a Perforce query spec.

A Perforce query also generates a file (changed_files.lst) that can be used with
Parasoft's --include option. 

If available, this script uses Perforce's P4 Python27 API (import P4);
otherwise, it resorts to queries via sub-process commands. 
'''

import sys
import os
import re

__all__     = []
__version__ = 0.4
__date__    = '2013-09-17'
__updated__ = '2014-03-13'

DEBUG = False

################################################################################
# Constants for Eclipse and Perforce paths. Tailor them for your project:
#
MY_PROJECT = '/TrimaMain'             # name of Eclipse project
MY_SRC_DIR = '/Trima/Main'            # path that maps to Eclipse project 
DEPOT_ROOT = '//depot/main/embedded'  # root of the Perforce depot 
MY_DEPOT   = DEPOT_ROOT + MY_SRC_DIR  # depot path to the source tree

################################################################################
# Lists and patterns for folders and files to exclude:
# 
exclDirs = [
    'bootrom', 'engr_tools', 'guitest', 'unit_test', 'vxworks_sim',
    'control_versalogic', 'safety_versalogic', 'safety_versalogic_pxe'
    ]

exclFiles = [ 'gei82543End.c', 'miiLib.c', 'ftpdLib.c', 'esmcEnd.c']

# Combined RegEx pattern to ignore directory and file paths
exclPaths = re.compile( '.*/(' + '|'.join(exclDirs + exclFiles) + ')' )

# This RegEx pattern matches a C/C++ source file
pattCppSrc = re.compile('.*\.(c|h|cpp|hpp)$')

################################################################################
# Defaults for Perforce look to the environment variables
#
p4Client = os.environ.get('P4CLIENT', '')
p4User   = os.environ.get('P4USER', os.environ.get('USERNAME', ''))
p4Pass   = os.environ.get('P4PASSWD', '')
p4Creds  = ''
theP4    = None

###############################################################################
def emit(txt, fout=sys.stdout):
    '''Print line to output file (adds newline)'''
    # print(txt, file=fout)       # Use from __future__ import print_function
    print >>fout, txt             # Use 2.7 >> operator

################################################################################
def p4cmd(cmdString, creds=''):
    '''Run Perforce command and return output as a list of dictionaries.
    Uses a subprocess to spawn the command and get results.
    '''
    import marshal, subprocess
    results = []

    # Prepend any P4 credentials to the command string
    cmdLine = "p4 -G %s %s" % (creds, cmdString)

    # Run the Perforce command using -G option to get results as
    # marshaled dictionary objects.
    process = subprocess.Popen( cmdLine, stdout=subprocess.PIPE, shell=True )

    # Harvest stdout output until end of file exception is thrown.
    try:
        while 1:
            output = marshal.load( process.stdout )
            results.append( output )
    except EOFError:
        pass
    finally:
        process.stdout.close()
    
    return results
    
################################################################################
def p4run(cmdStr):
    '''Runs a Perforce command. It first tries using the P4Python API'''

    global theP4, p4Client, p4User, p4Pass

    # Check if we've turned off the P4Python API
    if theP4 is False:
        return p4cmd(cmdStr)

    results = []
    
    # Try the P4Python API
    try:
        from P4 import P4, P4Exception

        # Singleton initialization
        if theP4 is None:
            # print 'Connecting: %s@%s' % (p4User, p4Client)
            theP4 = P4()
            theP4.client = p4Client
            theP4.user = p4User
            x = theP4.connect()
            print x

        args = cmdStr.split()
        cmd  = args[0]
        del args[0]
         
        if 'filelog' in cmd:
            # This returns a list of P4.DepotFile objects
            results = theP4.run_filelog(args)
        else:
            # This returns a list of marshaled dictionaries
            results = theP4.run(cmd, args)

    except ImportError:
        # Turn off attempts to use P4 API
        emit('Using command-line P4 API')
        theP4 = False
        results = p4cmd(cmdStr) 

    except P4Exception:
        for e in theP4.errors:            # Display errors
            print e
            
    return results

################################################################################
def p4get(p4obj, key, irev=0):
    '''Extracts the key value from a P4 object.
    The object is either from P4Python API or a marshaled dictionary.'''

    val = None
    
    if theP4 is False:
        # We are using the marshaled dictionary API
        if key == 'nrevs':
            val = 0
            while p4obj.has_key('rev'+str(val)): val += 1
            return val
        keyi = key + str(irev)
        if   p4obj.has_key(key):     val = p4obj[key]
        elif p4obj.has_key(keyi):    val = p4obj[keyi] 
        if val:
            if key in 'rev|change':  val = int(val)
            if key == 'user':        val = val.lower()

    elif theP4 != None:
        # We are using the P4 Python API
        if key == 'nrevs':           return len(p4obj.revisions)
        if key == 'code':            return 'okay' 
        if isinstance(p4obj, dict):  val = p4obj[key]
        elif key == 'depotFile':     val = p4obj.depotFile
        elif key == 'rev':           val = p4obj.revisions[irev].rev
        elif key == 'change':        val = p4obj.revisions[irev].change
        elif key == 'action':        val = p4obj.revisions[irev].action
        elif key == 'user':          val = p4obj.revisions[irev].user.lower()
        else:                        pass

    return val

################################################################################
def makeSrcListAll(srcFile, top=os.getcwd()):
    '''Generate file listing of all valid C/C++ source files to map'''

    emit('Autogen source files to: %s' % srcFile)

    # RegEx patterns to match against
    global exclDirs, exclFiles, exclPaths, pattCppSrc
    pattDirs = re.compile( '(' + '|'.join(exclDirs) + ')' )
    pattFile = re.compile( '(' + '|'.join(exclFiles) + ')' )

    top = os.path.abspath(top)
    fout = open(srcFile, 'w')
    cnt = 0

    if DEBUG: emit('DEBUG is ON')

    for (path, dirs, files) in os.walk(top):

        tail = os.path.basename(path)

        # Apply directory exclusion filter (do in 2 steps for debug purposes)
        if DEBUG: # Test the path filter
            xpath = path.replace('\\', '/') + '/'
            excl = [d for d in dirs if exclPaths.match(xpath + d)]
        else:
            excl = [d for d in dirs if pattDirs.match(d)]

        if excl:
            emit('Excluding dirs: %s/{%s}' % (tail, ','.join(excl)))
            dirs[:] = [d for d in dirs if d not in excl]

        for fname in files:
            # Apply file exclusion filter:
            if DEBUG: skip = exclPaths.match(xpath + fname)
            else:     skip = pattFile.match(fname) 
            if skip:
                emit('Excluding file: %s/%s ' % (tail, fname))
            elif pattCppSrc.match(fname):
                emit(os.path.join(path, fname), fout)
                cnt += 1

    fout.close()
    emit('Valid C/C++ files found: %d' % cnt)
    
    return (cnt > 0)

################################################################################
def makeSrcListNdays(srcFile, nDays):
    '''Runs Perforce query for files in MY_DEPOT that changed in last nDays'''
    
    from datetime import date, timedelta
    
    if nDays < 0 or nDays > 30:
        emit("Invalid! NDAYS = [0,30]")
        okay = False
    else:
        now = date.today()
        delta = timedelta(days=nDays)
        begin = now - delta
        # begYMD = begin.strftime('%Y/%m/%d:00:00')
        begYMD = begin.strftime('%Y/%m/%d')
        okay = makeSrcListByDate(srcFile, begYMD)

    return okay

################################################################################
def makeSrcListByDate(srcFile, begDate, endDate=None):
    '''Runs Perforce begDate query for MY_DEPOT/...@begDate,@endDate
    where: begDate is either yyyy/mm/dd or yyyy/mm/dd:hh:mm:ss;
    ditto for endDate or 'now' to specify the current time (the default). 
    Also generates file for Parasoft --include option.
    '''
    global exclPaths, pattCppSrc

    if endDate is None: endDate = 'now'

    emit('Mapping files changed from %s to %s' % (begDate,endDate))

    # Validate the begDate to be in yyyy/mm/dd format
    #
    okay = re.match('201[3-9]\/[01]?[0-9]\/[0123]?[0-9]', begDate) 
    if not okay:
        emit('Bad begDate: ' + begDate)
        return False

    # Validate the endDate to be in yyyy/mm/dd format or else 'now'
    #
    if endDate is not 'now':
        okay = re.match('201[3-9]\/[01]?[0-9]\/[0123]?[0-9]', endDate) 
        if not okay:
            emit('Bad endDate: ' + endDate)
            return False

    cmd = 'files %s...@%s,@%s' % (MY_DEPOT, begDate, endDate)
    depotDicts = p4run(cmd)

    from pprint import pprint
    if DEBUG: pprint(depotDicts)
    
    okay = ( len(depotDicts) > 0 and p4get(depotDicts[0], 'code') != 'error' )
    
    if okay:
        fout = open(srcFile, 'w')
        for dd in depotDicts:
            if DEBUG: pprint(dd)
            fileSpec = p4get(dd, 'depotFile') ## dd['depotFile']
            if exclPaths.match(fileSpec):
                continue
            if pattCppSrc.match(fileSpec):
                fileSpec += '#' + str(p4get(dd, 'rev')) ## dd['rev']
                emit(fileSpec, fout)
        fout.close()
    else:
        emit('No changes between %s and %s' % (begDate,endDate) )
        
    return okay

################################################################################
# Returns the author (username) for the input file by consulting Perforce. 
# The filename can be either a Perforce depot path, or an absolute filesystem
# path. With depot format, the optional revision spec can be used. For example:
#
# //depot/embedded/Trima/Main/projectrevision#2  --> author for rev 2
# //depot/embedded/Trima/Main/projectrevision    --> author for latest/head rev
# C:\Dev\trima\Trima\Main\projectrevision        --> author for latest/head rev
#
# If the file has only single rev, it's possibly was branched, in which case,
# it will look further back to get the last author with a rev > 1
#
def getAuthor(fileSpec):
    '''Returns the author for given file specification.'''

    # List of explicit change numbers that should be ignored for authorship:
    # 55074 : Reformat Trima Code using uncrustify
    # 55112 : Remove RCS header lines
    # 55320,55324,55326,55309 : Change of file-type attribute
    ignoreChanges = [ 55074, 55112, 55320, 55324, 55326, 55309 ]

    ## print '==================== ', fileSpec
    
    # execute the P4 command
    cmd = 'filelog -i ' + fileSpec
    depotFiles = p4run(cmd)

    # If the command failed or returned nil, author is unknown
    if len(depotFiles) is 0: return 'unknown'
        
    # this is the most recent (newest/latest) author
    author = p4get(depotFiles[0], 'user')

    # Exit now if author is unknown or a specific rev number was requested
    if author is None:  return 'unknown'
    if '#' in fileSpec: return author

    # Exit now if no need to look deeper
    revnum = p4get(depotFiles[0], 'rev')
    change = p4get(depotFiles[0], 'change')
    if revnum > 1 and change not in ignoreChanges: return author
    
    # Here, we look back into the history for last author with an edit action
    # (revnum > 1) or else take the initial revision (action=='add')
    #
    for df in depotFiles:
        nrevs = p4get(df, 'nrevs')
        for irev in range(nrevs):
            change = p4get(df, 'change', irev)
            revnum = p4get(df, 'rev',    irev)
            action = p4get(df, 'action', irev)
            user   = p4get(df, 'user',   irev)
            ## print 'change=%d rev=%d action=%s user=%s' % (change,revnum,action,user)
            if change in ignoreChanges:
                ## print 'ignore: change=%d' % change
                continue
            if revnum > 1:
                return user 
            if action == 'add':
                ## print 'initial rev: action=%s change=%d' % (action, change)
                return user 

    return author

################################################################################
def doAuthorMap(srcFile, doIncFile=False):
    '''Loop through the files in srcFile and generate XML author mapping.
    Optionally writes file for Parasoft --include option
    '''
    
    global MY_PROJECT, MY_SRC_DIR, DEPOT_ROOT, MY_DEPOT
    global exclPaths, pattCppSrc

    # This RegEx matches a file in Perforce depot path syntax
    depot_patt = re.compile(DEPOT_ROOT)
    # This RegEx matches a file in the sandbox view
    sandbox_patt = re.compile('.*'+MY_SRC_DIR)
    
    # Begin the map
    emit("<authorship>")

    # Open the input file with Universal (U) line endings
    fin = open(srcFile, 'rU')

    if doIncFile:
        # Parasoft's --include option requires a file with .lst as extension
        incFile = os.path.splitext(srcFile)[0]
        incFile += '.lst'
        fincl = open(incFile, 'w')
    else:
        fincl = None
        
    for rawline in fin:
        # Convert everything to forward slashes
        line = rawline.strip().replace('\\', '/')

        # Skip this file if its under one of the ignored directories        
        if exclPaths.match(line):
            # emit('<!-- Skipping ignored: %s -->' % line)
            continue

        # Is this a depot path?                
        if depot_patt.match(line):
            # Extract the depot path. Ex: //depot/main/embedded/Trima/Main/cds/software_cds.cpp
            # Here, we retain the optional rev spec
            fileSpec = line.split()[0]
            
            # Here, we omit the optional rev spec and convert to an Eclipse path
            # Ex: /TrimaMain/cds/software_cds.cpp
            path = fileSpec.split('#')[0]
        else:
            # Assume it's a file system path
            path = fileSpec = line

        # Skip this file if it's not a C/C++ source file        
        if not pattCppSrc.match(path):
            # emit('<!-- Skipping non-C/C++: %s -->' % path)
            continue

        # Convert to Eclipse project path  
        projPath = sandbox_patt.sub(MY_PROJECT, path)

        # Get the author information
        author = getAuthor(fileSpec)
        emit('   <file author="%s" path="%s" />' % (author, projPath) )

        # Convert to Parasoft include path: **/cds/software_cds.cpp
        if fincl:
            inclPath = projPath.replace(MY_PROJECT, '**')
            emit(inclPath, fincl)

    # End of map
    emit("</authorship>")

    # Close the file(s) we opened
    fin.close()    
    if fincl: fincl.close()

    return True
    
################################################################################

def main(argv=None):
    '''Handle command line options and execute.'''

    from optparse import OptionParser

    retval = 0

    prog_name     = os.path.basename(sys.argv[0])
    prog_usage    = "usage: %s [--help] [filespec | p4datespec] [p4creds]" % prog_name
    prog_longdesc = __doc__
    prog_version  = "v"+str(__version__)
    prog_bld_date = "%s" % __updated__
    prog_details  = '%%prog %s (%s)' % (prog_version, prog_bld_date)
    prog_license  = "Copyright 2013 TerumoBCT (mra1usb)"
 
    if argv is None:
        argv = sys.argv[1:]
    try:
        # setup option parser
        parser = OptionParser(version=prog_details, description=prog_longdesc, epilog=prog_license, )
        parser.set_usage(prog_usage)
        parser.add_option("-a", dest="autogen", action="store_true",help="auto-generate from all C/C++ source")
        parser.add_option("-f", dest="infile",  metavar="INFILE",   help="use files listed in INFILE")
        parser.add_option("-n", dest="ndays",   metavar="NDAYS",    help="p4 query for files changed since midnight - NDAYS", type="int")
        parser.add_option("-d", dest="date",    metavar="DATE",     help="p4 query for files changed in range begDate[,endDate] where date is yyyy/mm/dd[:hh[:mm[:ss]]]")
        parser.add_option("-c", dest="client",  metavar="P4CLIENT", help="p4 client (workspace) [default: environment var]")
        parser.add_option("-u", dest="user",    metavar="P4USER",   help="p4 user (login) [default: environment var]")
        parser.add_option("-P", dest="passwd",  metavar="P4PASSWD", help="p4 password [default: environment var]")
        
        # set defaults
        # parser.set_defaults(ndays=7, infile="allsrc.txt")
        
        # parse the command-line options
        (opts, args) = parser.parse_args(argv)

        # Should not be any args left over
        if len(argv) == 0 or len(args) > 0:
            parser.print_usage()
            return 1

        # Write the XML header so that debug will go inside comment block
        emit('<?xml version="1.0" encoding="utf-8"?>')
        emit('<!--')

        # Check for Perforce credentials
        global p4Client, p4User, p4Pass
        if opts.client: p4Client = opts.client #; print 'P4User  : ', p4Client
        if opts.user:   p4User   = opts.user   #; print 'P4Client: ', p4User
        if opts.passwd: p4Pass   = opts.passwd #; print 'P4Pass  : ', p4Pass 
        
        # Parse options to set the input file to process
        okay = True
        doIncludeFile = False
        if opts.autogen:
            inputFile = 'allsrc.txt'
            okay = makeSrcListAll(inputFile)
        elif opts.infile:
            inputFile = opts.infile
            emit('Mapping files from: %s' % inputFile)
        elif opts.ndays >= 0:
            emit('Mapping files changed in last NDAYS: %d' % opts.ndays)
            inputFile = 'changed_files.p4'
            doIncludeFile = True
            okay = makeSrcListNdays(inputFile, opts.ndays)
        elif opts.date:
            inputFile = 'changed_files.p4'
            doIncludeFile = True
            dates = opts.date.split(',')
            begDate = dates[0]
            endDate = None
            if len(dates) is 2:
                endDate = dates[1]
            okay = makeSrcListByDate(inputFile, begDate, endDate)
        else:
            emit('No valid options specified!')
            okay = False

        if okay:
            p4run('info')
            emit('-->')
            doAuthorMap(inputFile, doIncludeFile)
        else:
            emit('-->')
            retval = 1
        
#    except Exception, e:
#        indent = len(prog_name) * " "
#        sys.stderr.write(prog_name + ": " + repr(e) + "\n")
#        sys.stderr.write(indent + "  for help use --help\n")
#        retval = 2

    finally:
        if theP4 and theP4.connected(): theP4.disconnect()

    return retval

################################################################################

if __name__ == "__main__":
    sys.exit(main())

