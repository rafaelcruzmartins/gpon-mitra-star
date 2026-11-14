"""Baixa um arquivo do G7 usando g7dump (somente leitura).
Uso: python fetch.py ARQUIVO_REMOTO SAIDA_LOCAL [PORTA]"""
import hashlib
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


remote, local = sys.argv[1], sys.argv[2]
port = int(sys.argv[3]) if len(sys.argv) > 3 else 2400
run("rm -f /var/tmp/g7dump.log")
rc, out = run("chmod +x /var/tmp/g7dump && (/var/tmp/g7dump %d %s > /var/tmp/g7dump.log 2>&1 &)" % (port, remote))
if rc:
    raise SystemExit(out)
time.sleep(1)
s = socket.create_connection(("192.168.1.1", port), timeout=30)
data = bytearray()
while True:
    d = s.recv(65536)
    if not d:
        break
    data += d
s.close()
time.sleep(1)
rc, log = run("cat /var/tmp/g7dump.log")
m = re.search(r"bytes=(\d+) crc32=([0-9a-f]{8})", log)
open(local, "wb").write(data)
crc = "%08x" % (zlib.crc32(bytes(data)) & 0xffffffff)
ok = bool(m) and int(m.group(1)) == len(data) and m.group(2) == crc
print("%s -> %s: %d bytes, crc32 local %s, remoto %s, sha256 %s, %s" % (
    remote, local, len(data), crc, m.group(2) if m else "?",
    hashlib.sha256(data).hexdigest(), "OK" if ok else "FALHOU"))
sys.exit(0 if ok else 1)
