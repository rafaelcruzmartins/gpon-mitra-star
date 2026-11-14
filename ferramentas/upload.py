"""Envia um arquivo local para o G7 via ponte telnet, usando printf com escapes octais.
Uso: python upload.py ARQUIVO_LOCAL CAMINHO_REMOTO"""
import re
import socket
import sys
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
            return int(m.group(1)), data
    raise SystemExit("sem resposta")


src, dst = sys.argv[1], sys.argv[2]
blob = open(src, "rb").read()
rc, _ = run("rm -f %s" % dst)
CH = 160
for i in range(0, len(blob), CH):
    part = "".join("\\%03o" % b for b in blob[i:i + CH])
    rc, out = run("printf '%s' >> %s" % (part, dst))
    if rc != 0:
        raise SystemExit("falha no bloco %d: %r" % (i, out[-300:]))
    if (i // CH) % 20 == 0:
        print("%d/%d" % (i, len(blob)), flush=True)
rc, out = run("wc -c < %s" % dst)
print("remoto:", out.decode("latin-1").split("\n")[-2].strip(), "local:", len(blob),
      "crc32 local: %08x" % (zlib.crc32(blob) & 0xffffffff))
