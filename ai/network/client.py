import socket

class ZappyClient:

    def __init__(self, host, port, team):
        self.host = host
        self.port = port
        self.team = team
        self.sock = None
        self.file = None

    def connect(self):

        self.sock = socket.create_connection((self.host, self.port))
        self.file = self.sock.makefile("rwb", buffering=0)

        welcome = self.read_line()
        if welcome != "WELCOME":
            raise RuntimeError(f"Bad welcome: {welcome}")

        self.send_raw(self.team)
        client_num = self.read_line()
        world_size = self.read_line()

        print(f"[AI] Connected: slots={client_num}, map={world_size}")

    def send_raw(self, text):
        self.file.write((text + "\n").encode())
        print(f"[AI -> SERVER] {text}")

    def read_line(self):
        line = self.file.readline()
        if not line:
            raise RuntimeError("Server closed connection")
        print(f"[SERVER -> AI] {line.decode().strip()}")
        return line.decode().strip()

    def command(self, cmd):
        self.send_raw(cmd)

        while True:
            response = self.read_line()

            if response == "dead":
                raise RuntimeError("Player is dead")

            if response.startswith("message "):
                print(f"Broadcast ignored: {response}")
                continue

            if response.startswith("eject:"):
                print(f"Eject ignored: {response}")
                continue

            return response