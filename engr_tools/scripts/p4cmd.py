#! /usr/bin/env python
# encoding: utf-8
'''
Issues a Perforce command and prints the Python-marshalled output.
'''
import sys, pprint

################################################################################
def p4cmd(cmdString, creds=''):
    '''Run Perforce command and return output as a list of dictionaries.'''
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

if __name__ == "__main__":
    dd = p4cmd(' '.join(sys.argv[1:]))
    pprint.pprint(dd)
