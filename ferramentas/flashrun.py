"""Envia a imagem ao g7flash no G7.
Uso: python flashrun.py IMAGEM PORTA [--write]"""
import re
import socket
import sys
import time
import zlib

END = re.compile(rb"__FIM_\d{13}(\d+)__$")


def run(cmd):
    c = socket.create_connection(("127.0.0.1", 2323), timeout=120)
    c.sendall(("RUN " + cmd + "\n").encode())
    data = b""
    while True:
        d = c.recv(65536)
        if not d:
            break
        data += d
        m = END.search(data)
        if m:
            c.close()
            return int(m.group(1)), data.decode("latin-1")
    raise SystemExit("sem resposta")


img = open(sys.argv[1], "rb").read()
port = int(sys.argv[2])
mode = sys.argv[3] if len(sys.argv) > 3 else ""
crc = "%08x" % (zlib.crc32(img) & 0xffffffff)
print("local: %d bytes crc32 %s" % (len(img), crc))
run("rm -f /var/tmp/g7flash.out")
run("chmod +x /var/tmp/g7flash && (/var/tmp/g7flash %d %d %s %s > /var/tmp/g7flash.out 2>&1 &)"
    % (port, len(img), crc, mode))
time.sleep(1.5)
s = socket.create_connection(("192.168.1.1", port), timeout=60)
s.sendall(img)
s.shutdown(socket.SHUT_WR)
reply = b""
while True:
    d = s.recv(4096)
    if not d:
        break
    reply += d
s.close()
print("G7 respondeu:", reply.decode().strip())
time.sleep(3)
rc, out = run("cat /var/tmp/g7flash.out")
print(out.split("\n", 2)[-1] if "\n" in out else out)
