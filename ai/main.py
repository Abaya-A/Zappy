import argparse

from network.client import ZappyClient

parser = argparse.ArgumentParser(add_help=False)
parser.add_argument("-p", type=int, required=True)
parser.add_argument("-n", required=True)
parser.add_argument("-h", dest="host", default="localhost")
parser.add_argument("--help", action="help")
args = parser.parse_args()
