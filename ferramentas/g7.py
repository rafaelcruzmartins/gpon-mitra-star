"""Cliente da ponte: python g7.py PEEK | python g7.py RUN "<cmd>" """
import re
import socket
import sys

END = re.compile(rb"__FIM_\d{13}\d+__$")
c = socket.create_connection(("127.0.0.1", 2323), timeout=900)
req = " ".join(sys.argv[1:])
c.sendall((req + "\n").encode())
if not req.startswith("RUN "):
    c.settimeout(3)
data = b""
while True:
    try:
        d = c.recv(65536)
    except socket.timeout:
        break
    if not d:
        break
    data += d
    if req.startswith("RUN ") and END.search(data):
        break
c.close()
sys.stdout.buffer.write(data.replace(b"\r", b""))
