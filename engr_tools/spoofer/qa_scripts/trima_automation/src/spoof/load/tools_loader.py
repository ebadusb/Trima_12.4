'''
Created on Jan 25, 2011

@author: jdavusb
'''

from ftplib import FTP
import telnetlib
import os
from spoof.log import logger

class loader():
    '''
    classdocs
    '''
    TRIMA_FTP_USER = "david"
    TRIMA_FTP_PASS = "david"
    TRIMA_TOOL_DIR = "/machine/tools/"
    TRIMA_TELNET_PROMPT = "-> "
    TRIMA_TELNET_TOOL_LD = "ld 1,0,\"" + TRIMA_TOOL_DIR + "spoof_main.out\"\n"
    TRIMA_TELNET_TOOL_UNLD = "unld \"spoof_main.out\"\n"
    TRIMA_TELNET_KILL = "\x03"

    def __init__(self, address, tools_dir, do_log=True):
        '''
        Constructor
        '''
        self.address = address
        self.tools_dir = tools_dir
        self.logger = None
        if do_log: self.logger = logger.log(True) # Add stdout logging only

    def log(self, msg):
        if (not self.logger == None) : self.logger.report(msg)

    def tool_transfer(self):
        ftp = FTP(self.address, loader.TRIMA_FTP_USER, loader.TRIMA_FTP_PASS)
        self.log("Connected to FTP server...")
        ftp.cwd(loader.TRIMA_TOOL_DIR)

        for filename in os.listdir(self.tools_dir):
            tool_file = open(self.tools_dir + filename,'rb')
            self.log("Sending " + filename + "...")
            ftp.storbinary('STOR ' + filename, tool_file)
            tool_file.close()

        self.log("Closing FTP connection...")
        ftp.close()
        
        self.log("Connecting to Telnet server...")
        vxloader = telnetlib.Telnet(self.address)
        vxloader.read_until(loader.TRIMA_TELNET_PROMPT)
        self.log("Unloading current spoof_main.out...")
        vxloader.write(loader.TRIMA_TELNET_TOOL_UNLD)
        vxloader.read_until(loader.TRIMA_TELNET_PROMPT)
        self.log("Loading new spoof_main.out")
        vxloader.write(loader.TRIMA_TELNET_TOOL_LD)
        vxloader.read_until(loader.TRIMA_TELNET_PROMPT)
        self.log("Exiting vxworks shell...")
        vxloader.write("exit")
        self.log("Closing Telnet connection...")
        vxloader.close()
        vxloader = None # Telnet client seems to have problems "letting go". Make sure it gets deleted.

    def reboot(self):
        rebooter = telnetlib.Telnet(self.address)
        rebooter.read_until(loader.TRIMA_TELNET_PROMPT)
        rebooter.write(loader.TRIMA_TELNET_KILL)
        rebooter.close()

    def reload_module(self, module):
        vxloader = telnetlib.Telnet(self.address)
        vxloader.read_until(loader.TRIMA_TELNET_PROMPT)
        self.log("Unloading current " + module + "...")
        vxloader.write("unld \"" + module + "\"\n")
        vxloader.read_until(loader.TRIMA_TELNET_PROMPT)
        self.log("Loading new " + module)
        vxloader.write("ld 1,0,\"" + loader.TRIMA_TOOL_DIR + module + "\"\n")
        vxloader.read_until(loader.TRIMA_TELNET_PROMPT)