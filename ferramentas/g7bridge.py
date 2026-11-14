"""Ponte Telnet para o G7.

O usuario digita login e senha neste console (a senha nao e ecoada nem gravada).
Depois do login, Claude envia comandos pela porta local 127.0.0.1:2323.
Protocolo (uma linha por pedido): PEEK | RUN <cmd> | RAW <texto>
"""
import msvcrt
import socket
import sys
import threading
import time

HOST, PORT = "192.168.1.1", 23
CTRL = ("127.0.0.1", 2323)

lock = threading.Lock()
buf = bytearray()          # tudo que veio do G7 (sem negociacao telnet)
tn = socket.create_connection((HOST, PORT), timeout=10)
tn.settimeout(None)


def negotiate_and_strip(data):
    out = bytearray()
    i = 0
    while i < len(data):
        b = data[i]
        if b == 255 and i + 1 < len(data):
            cmd = data[i + 1]
            if cmd in (251, 252, 253, 254) and i + 2 < len(data):
                opt = data[i + 2]
                if cmd == 251:  # WILL
                    tn.sendall(bytes([255, 253 if opt in (1, 3) else 254, opt]))
                elif cmd == 253:  # DO
                    tn.sendall(bytes([255, 251 if opt == 3 else 252, opt]))
                i += 3
                continue
            if cmd == 255:
                out.append(255)
            i += 2
            continue
        out.append(b)
        i += 1
    return bytes(out)


def reconnect():
    global tn
    while True:
        try:
            tn = socket.create_connection((HOST, PORT), timeout=10)
            tn.settimeout(None)
            return
        except OSError:
            time.sleep(2)


def reader():
    while True:
        try:
            d = tn.recv(65536)
        except OSError:
            d = b""
        if not d:
            print("\n[conexao encerrada pelo G7 - reconectando...]\n")
            reconnect()
            continue
        d = negotiate_and_strip(d)
        with lock:
            buf.extend(d)
            if len(buf) > 4_000_000:
                del buf[:2_000_000]
        sys.stdout.write(d.decode("utf-8", "replace"))
        sys.stdout.flush()


def keyboard():
    while True:
        ch = msvcrt.getwch()
        try:
            send_key(ch)
        except OSError:
            time.sleep(1)


def send_key(ch):
    if True:
        if ch == "\r":
            tn.sendall(b"\r\n")
        elif ch == "\x08":
            tn.sendall(b"\x7f")
        else:
            tn.sendall(ch.encode("utf-8", "replace"))


def run_cmd(cmd, timeout=600):
    marker = "__FIM_%d__" % int(time.time() * 1000)
    with lock:
        start = len(buf)
    tn.sendall((cmd + "; echo " + marker[:-2] + "$?__\r\n").encode())
    t0 = time.time()
    needle = marker[:-2].encode()
    while time.time() - t0 < timeout:
        with lock:
            seg = bytes(buf[start:])
        # o eco do proprio comando contem o marcador seguido de "$?"; queremos a saida real
        idx = 0
        while True:
            j = seg.find(needle, idx)
            if j < 0:
                break
            tail = seg[j + len(needle):j + len(needle) + 6]
            if tail[:1].isdigit():
                return seg[:j + len(needle) + tail.find(b"__") + 2]
            idx = j + 1
        time.sleep(0.05)
    return seg + b"\n[TIMEOUT]"


def control():
    srv = socket.socket()
    srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    srv.bind(CTRL)
    srv.listen(1)
    while True:
        c, _ = srv.accept()
        f = c.makefile("rb")
        line = f.readline().decode("utf-8").rstrip("\r\n")
        if line == "PEEK":
            with lock:
                resp = bytes(buf[-4000:])
        elif line.startswith("RUN "):
            resp = run_cmd(line[4:])
        elif line.startswith("RAW "):
            tn.sendall(line[4:].encode().decode("unicode_escape").encode("latin-1"))
            time.sleep(1)
            with lock:
                resp = bytes(buf[-2000:])
        else:
            resp = b"?"
        c.sendall(resp)
        c.close()


print("Ponte Telnet G7. Digite o login e a senha quando pedidos.")
print("Depois do login, deixe esta janela aberta: o Claude continua daqui.\n")
threading.Thread(target=reader, daemon=True).start()
threading.Thread(target=control, daemon=True).start()
keyboard()
