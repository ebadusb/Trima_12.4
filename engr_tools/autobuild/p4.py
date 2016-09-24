# This script provides a class that runs Perforce command line and returns
# output formatted into marshalled dictionaries via the P4 -G option.
# Example usage: P4.run("changes", ['-t', '-l', "@Rev01.01.001"]) returns
# a list of dictionaries representing all changelists that correspond to
# the label Rev01.01.001.

import os, marshal, sys, re, operator

class p4Cmd:
   @staticmethod
   # @param string   Perforce command
   # @param string[] P4 command args, i.e. ['-t', '-l', "@Rev01.01.001"]
   # @param stream   Stream (open file) to which to send the output.
   def run( cmd, args=[], input=0 ):
      commandLine = "p4 -G " + cmd + " " + " ".join( args )
      if sys.version_info[1] < 6:
         (pipeIn, pipeOut) = os.popen2( commandLine , "b" )
      else:
         from subprocess import Popen, PIPE
         p = Popen( commandLine, stdin=PIPE, stdout=PIPE, shell=True )
         (pipeIn, pipeOut) = (p.stdin, p.stdout)
      if input:
         marshal.dump( input, pipeIn, 0 )
         pipeIn.close()
      r = []
      try:
         while 1:
            x = marshal.load( pipeOut )
            r = r + [ x ]
      except EOFError: pass
      return r


   @staticmethod
   # getNextRevision examines existing Perforce labels starting with the prefix, followed by 
   # rev. Rev is a string with the form "#.X" where # is an integer. It will return the next iteration
   # of the label based on existing Perforce labels.
   #
   # For example: if label common_build_17.0085 is the highest revision within v17, then 
   # getNextRevision("common_build_", "17.X", "%04d") will return "common_build_17.0086"
   #
   def getNextRevision(prefix, rev, xfmt="%05d"):
      formatRev = rev.replace(".X", "")
      # Build label search expression
      labelExp = prefix + formatRev + "*"
      # Get labels.
      labels = p4Cmd.run("labels", ["-e "+labelExp])

      # For default, use X=1
      nextRevNum = 1
      revString = formatRev + "." + xfmt % nextRevNum
      
      # Are there any labels for this version?
      if len(labels) > 0:
         # Sort by update ID to get the latest.
         labels = sorted(labels, key=operator.itemgetter('Update'), reverse=True)

         # Get the latest label (filter out test label DevTest)
         latestLabel = labels[0]['label']
         if 'DevTest' in latestLabel:
            if len(labels) == 1:
                latestLabel = ""
            else:
                latestLabel = labels[1]['label']

         # Break off the build number. Note that this strips out all instances of '.'
         newRev = latestLabel.replace(prefix, "").split('.')
         
         # Accommodate bug in P4 where it ignores the ".*" combination. Need this if were 
         # doing a new minor version (i.e. X.X.1)
         if len(newRev) < len(rev.split('.')):
            print "Using default/new revString", revString
         else:
            # Increment only the final number
            nextRevNum = int(newRev[-1]) + 1
            newRev[-1] = xfmt % nextRevNum
            # Convert to string, adding '.' as a delimiter
            revString = '.'.join(newRev)

      label = prefix + revString
      # return (label, str(nextRevNum))
      return label
