import socket

class ZappyClient:

    def __init__(self, host, port, team):
        self.host = host
        self.port = port
        self.team = team
        self.sock = None
        self.file = None

        self.messages = []
        self.current_level_line = None

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

        command_name = cmd.split()[0]

        while True:
            line = self.read_line()

            if line == "dead":
                raise RuntimeError("Player is dead")

            if line.startswith("message "):
                self.messages.append(line)
                print(f"Broadcast received: {line}")
                continue

            if line.startswith("eject:"):
                print(f"Eject ignored: {line}")
                continue

            if command_name == "Incantation":
                return self.handle_incantation_response(line)

            if line == "Elevation underway":
                self.current_level_line = self.read_incantation_result()
                continue

            if line.startswith("Current level:"):
                self.current_level_line = line
                continue

            return line

    def handle_incantation_response(self, line):
        if line == "ko":
            return "ko"

        if line.startswith("Current level:"):
            return line

        if line != "Elevation underway":
            return line

        return self.read_incantation_result()

    def read_incantation_result(self):
        while True:
            line = self.read_line()

            if line == "dead":
                raise RuntimeError("Player is dead")

            if line.startswith("message "):
                self.messages.append(line)
                print(f"Broadcast received: {line}")
                continue

            if line.startswith("eject:"):
                print(f"Eject ignored: {line}")
                continue

            if line.startswith("Current level:"):
                return line

            if line == "ko":
                return "ko"