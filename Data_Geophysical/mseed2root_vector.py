# Author: Hsin-Yeh Wu
# MiniSEED format: https://ds.iris.edu/ds/nodes/dmc/data/formats/miniseed/
# Python module for reading mseed: https://docs.obspy.org/index.html
# Example command
# python mseed2root.py <mseedFile>
#
#########################################
# This version is setting the output branch with std:vectors.
# On one hand it is more intuitive to operate with vectors, because all the samples for the same event would be saved in one vector
# On the other hand the output root file would be much much larger.....
# So depending on your usage and input file size, one could decide whether or not using this version
#########################################

#/usr/bin/env python
#
from obspy import read, UTCDateTime
import ROOT
from ROOT import TTree
import yaml
import sys
import os
import errno
from array import array

import argparse
parser = argparse.ArgumentParser(description='convert mseed format file to root file')
parser.add_argument('in_filenames',nargs="+",help='input filenames')
parser.add_argument('--skim','-s',action="store_true",help='skim events. Only take the first sample of each run')
parser.add_argument('--report','-r',default=10,type=int,help='report every x events')
args = parser.parse_args()

rootDir = "output_root"
yamlDir = "yaml"
# Create output directory
try:
   os.mkdir(rootDir)
   os.mkdir(yamlDir)
except OSError as e:
    if e.errno == errno.EEXIST:
       print('Output directory exists.')
    else:
        raise

# Read input miniSeed file.
for infile in args.in_filenames:
   mseedFileName = infile
   mseeds = read(mseedFileName)

   # Read headers
   station       = mseeds[0].stats.station
   location      = mseeds[0].stats.location
   channel       = mseeds[0].stats.channel
   starttime     = mseeds[0].stats.starttime.datetime
   endtime       = mseeds[len(mseeds)-1].stats.endtime.datetime
   sampling_rate = float(mseeds[0].stats.sampling_rate)
   delta         = float(mseeds[0].stats.delta)

   a_dict = {}
   for variable in ["station","location","channel","starttime","endtime","sampling_rate","delta"]:
      a_dict[variable] = eval(variable)

   # Write headers into yaml file
   outputFileName = station + '_' + location + '_' + channel + '_' + starttime.strftime("%Y%m%d_%H%M%S")
   if(args.skim): outputFileName = station + '_' + location + '_' + channel + '_' + starttime.strftime("%Y%m%d_%H%M%S") + '_skim'
   with open(f'{yamlDir}/{outputFileName}.yaml' , 'w') as file:
      documents = yaml.dump(a_dict, file, default_flow_style=False)

   # Create output root file
   outroot = ROOT.TFile.Open(f'{rootDir}/{outputFileName}.root',"RECREATE")
   outroot.cd()

   print("====================")
   print(f"Input raw data: {infile}")
   print(f"Output root file: {rootDir}/{outputFileName}.root")
   print(f"Output yaml file: {yamlDir}/{outputFileName}.yaml")

   # Init Tree and Branch
   t = ROOT.TTree("GeoData", "GeoData")
   data = ROOT.std.vector('int')()
   timestamp = ROOT.std.vector('int')()
   timestamp_ns = ROOT.std.vector('int')()
   t.Branch('data', data)
   t.Branch('timestamp', timestamp)
   t.Branch('timestamp_ns', timestamp_ns)

   # Start filling root file
   print("====================")
   print(f"station:{station}, location:{location}, channel:{channel}")
   print(f"Start: {starttime}")
   print(f"End:   {endtime}")
   print(f"Total events: {len(mseeds)}")
   print("====================")

   for ev, mseed in enumerate(mseeds):
      if (ev%args.report==0): print("Processing event:{}, {}, {:.2f}".format(ev, mseed.stats.starttime.datetime, 100*ev/len(mseeds)))

      ts = int(mseed.stats.starttime.timestamp)
      # Get the numbers after decimal in ns.
      ns = int(mseed.stats.starttime.ns-int(mseed.stats.starttime.timestamp)*1e9)

      # Skimmed
      if (args.skim):
         data.push_back(int(mseed.data[0]))
         timestamp.push_back(int(ts))
         timestamp_ns.push_back(int(ns))
      # Get all data
      else:
         for value in mseed.data:
            data.push_back(int(value))
            timestamp.push_back(int(ts))
            timestamp_ns.push_back(int(ns))
            # update timestamp for next sample
            ns += 1e7
            if(ns>=1e9):
               ns = 0
               ts += 1# UTCDateTime + 1 second
      t.Fill()

   t.Write()
   outroot.Close()
