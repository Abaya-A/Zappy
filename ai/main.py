import argparse

from ai.network.client import ZappyClient
from ai.strategy.agent import Agent

def parse_args():
    parser = argparse.ArgumentParser(add_help=False)

    parser.add_argument("-p", type=int, required=True)
    parser.add_argument("-n", required=True)
    parser.add_argument("-h", dest="host", default="localhost")
    parser.add_argument("--help", action="help")

    return parser.parse_args()

def main():
    args = parse_args()

    client = ZappyClient(
        host=args.host,
        port=args.p,
        team=args.n
    )
    client.connect()

    agent = Agent(client)
    agent.run()

if __name__ == '__main__':
    main()