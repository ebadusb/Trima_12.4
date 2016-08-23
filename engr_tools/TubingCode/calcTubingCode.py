#!/usr/bin/env python
# encoding: utf-8
'''
Calculator for Trima Tubing Set codes (a.k.a. Admin codes).

It computes one of the following:
1) for a Catalog REF number and a set of function bits, compute the Admin Code
2) for a Catalog REF number and an Admin code, display the function bits. 

@copyright:  2013 Terumo BCT. All rights reserved.
@author:     mra1usb
'''
import sys

from argparse import ArgumentParser
from argparse import RawDescriptionHelpFormatter
# from pprint import pprint

__all__     = []
__version__ = 0.1
__date__    = '2013-10-04'
__updated__ = '2013-10-11'

DEBUG = 1

FUNCTION_BITS = {
0:  'DRBC_BAG',
1:  'RBC_BAG',
2:  'PLASMA_BAG',
3:  'PLATELET_BAG',
4:  'MSS_PLT',
5:  'NO_SALINE_BOLUS',
6:  'MSS_RBC',
7:  'WHITE_STAMP',
8:  'spare_1',
9:  'spare_2',
10: 'AC_LUER_CONNECT',
11: 'spare_3',
12: 'spare_4',
13: 'spare_5',
14: 'spare_6',
15: 'spare_7',
16: 'MAX_NUM_BITS',
}

ORIGINAL_NUM_BITS = 10
ORIGINAL_MASK = (1 << ORIGINAL_NUM_BITS) - 1
EXTENDED_MASK = ~ORIGINAL_MASK
MASK16 = 0xFFFF
MASK32 = 0xFFFFFFFF

################################################################################
class MyErr(Exception):
    '''Generic exception to raise and log different fatal errors.'''
    def __init__(self, msg):
        super(MyErr).__init__(type(self))
        self.msg = "E: %s" % msg
    def __str__(self):        return self.msg
    def __unicode__(self):    return self.msg

################################################################################
def crc16(byteArray):
    '''Compute 16-bit CRC used by Trima admin code. 
    The input is a bytearray in little-endian order'''

    MASK16          = 0xFFFF           
    CRC_RESET_VALUE = 0x0000
    CRC_POLYNOMIAL  = 0x8005
    CRC_XOR_OUTPUT  = 0x0000

    crc = CRC_RESET_VALUE

    for b in byteArray:
        crc ^= (b << 8) & MASK16
        for i in range(8):
            if crc & 0x8000:
                crc = (crc << 1) ^ CRC_POLYNOMIAL
            else:
                crc <<= 1
            crc &= MASK16

    return CRC_XOR_OUTPUT ^ crc
        
################################################################################
def showFunctionBits(funcBits=MASK16):
    '''Display the function bits'''

    global FUNCTION_BITS
    print 'FunctionBits for: 0x%04x:%d' % (funcBits, funcBits) 
    for (bitIndx,val) in FUNCTION_BITS.iteritems():
        mask = (1 << bitIndx)
        if funcBits & mask:
            print '0x%04x : bit[%02d] = %s' % (mask, bitIndx, val)

################################################################################
def getFunctionBits(catalogNum, adminCode):
    '''Return the function bits from an admin code'''

    global MASK16
    functionBits = (adminCode >> 16) & MASK16

    if adminCode != calcAdminCode(catalogNum, functionBits):
        print "AdminCode(0x%08x:%d) is invalid!" % (adminCode, adminCode)
        
    return functionBits

################################################################################
def calcAdminCode(catalogNum, functionBits):
    '''Calculate and return the admin code'''

    global MASK16, MASK32

    if functionBits & EXTENDED_MASK:
        # new: 5-6 digits for catalog + 16 function bits (leave room for 32)
        uint64 = (catalogNum << 32) | (functionBits & MASK16)
        hexStr = '%016x' % uint64
    else:
        # orig: 5-6 digits for catalog + 10 function bits
        uint32 = ( (catalogNum << 10) | (functionBits & MASK16) ) & MASK32
        hexStr = '%08x' % uint32
        
    bites = bytearray.fromhex(hexStr)
    
    # Reverse the byte array to be little-endian (Intel)
    bites.reverse()

    crcBCT = crc16(bites)
    
    adminCode = (functionBits << 16) | crcBCT
    return adminCode

################################################################################
def main(argv=None): # IGNORE:C0111
    '''Parse command line options and execute the program.'''
    
    if argv is None:
        argv = sys.argv
    else:
        sys.argv.extend(argv)

    # prog_name = os.path.basename(sys.argv[0])
    prog_version = "v%s" % __version__
    prog_build_date = str(__updated__)
    prog_version_message = '%%(prog)s %s (%s)' % (prog_version, prog_build_date)
    prog_shortdesc = __import__('__main__').__doc__.split("\n")[1]

    try:
        # Setup argument parser
        parser = ArgumentParser(description=prog_shortdesc, formatter_class=RawDescriptionHelpFormatter)
        parser.add_argument("-a", "--admin",    dest="admin",  help="For admin code, show function bits")
        parser.add_argument("-f", "--fbits",    dest="fbits",  help="For function bits, compute admin code")
        parser.add_argument("-b", "--showbits", dest="showbits",  help="Show function bits and exit", action="store_true")
        # parser.add_argument("-v", "--verbose", dest="verbose", action="count", help="set verbosity level [default: %(default)s]")
        parser.add_argument('-V', '--version', action='version', version=prog_version_message)
        parser.add_argument(dest="refnum", default="80400", help="Cassette reference number [default: %(default)s]", nargs='?')
        
        # Process arguments
        args = parser.parse_args()
        
        if args.showbits:
            showFunctionBits()
            return 0 

        refnum = int(args.refnum, 0)
        
        if args.admin:
            admin = int(args.admin, 0) 
            myFunctionBits = getFunctionBits(refnum, admin)
            print "CatalogNum( %d ) + AdminCode( 0x%08x:%d ) => FunctionBits(0x%04x:%d )" % (refnum, admin, admin, myFunctionBits, myFunctionBits)
            showFunctionBits(myFunctionBits)
        
        if args.fbits:
            funcbits = int(args.fbits, 0)
            myAdminCode = calcAdminCode(refnum, funcbits)
            showFunctionBits(funcbits)
            print "CatalogNum( %d ) + FunctionBits( 0x%04x:%d ) => AdminCode( 0x%08x:%d )" % (refnum, funcbits, funcbits, myAdminCode, myAdminCode)

        return 0

    except KeyboardInterrupt:
        ### handle keyboard interrupt ###
        return 0

#    except Exception, e:
#        if DEBUG:
#            raise(e)
#        indent = len(prog_name) * " "
#        sys.stderr.write(prog_name + ": " + repr(e) + "\n")
#        sys.stderr.write(indent + "  for help use --help")
#        return 2

################################################################################

if __name__ == "__main__":
    if DEBUG == 2:
        sys.argv.append("-h")
        sys.argv.append("-v")
        sys.argv.append("-r")
    sys.exit(main())
