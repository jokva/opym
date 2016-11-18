#! /usr/bin/env python

import opym
import sys, argparse

def parse_args():
    ap = argparse.ArgumentParser( description = "Print well information in eclipse deck" )
    ap.add_argument( 'deck', help = 'input deck' )
    ap.add_argument( '-t', '--timestep', dest = 'timestep', type = int,
                     default = 0, help = 'timestep' )

    ap.add_argument( '-d', '--defined', dest = 'defined', action = 'store_true',
                     default = False, help = 'Only defined wells' )

    prodgroup = ap.add_mutually_exclusive_group()
    prodgroup.add_argument( '-i', '--injector', dest = 'injector', action = 'store_true',
                            default = None, help = 'Only injectors' )
    prodgroup.add_argument( '-p', '--producer', dest = 'injector', action = 'store_false',
                            default = None, help = 'Only producers' )

    return ap.parse_args()

def list_wells(argv):
    args = parse_args()

    ecl = opym.parse(args.deck, [("PARSE_RANDOM_SLASH", opym.InputErrorAction.ignore)])
    wells = ecl.schedule.wells
    ts = args.timestep

    if args.injector is not None:
        wells = filter(lambda x: x.is_injector(ts) == args.injector, wells)

    if args.defined:
        wells = filter(lambda x: x.defined(ts), wells)

    for well in wells:
        print(well.name)

if __name__ == "__main__":
    list_wells(sys.argv)
