import time
import socket

HOST = "127.0.01"
PORT = 5000
BUFFER_SIZE = 16384

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s.bind((HOST, PORT))
s.listen(1)
print(f"Listening on {HOST}:{PORT}")

conn, addr = s.accept()
print(f"Connected by {addr}")

filename_lenght = int.from_bytes(conn.recv(4), "big")
filename = conn.recv(filename_lenght).decode("utf-8")
print(f"Receiving file: {filename}")

t0 = time.time()
fp = open(filename + "COPY", "wb")
nbytes = 0
while True:
    data = conn.recv(BUFFER_SIZE)
    nbytes += len(data)
    if not data:
        break
    fp.write(data)
    print(f"Received {nbytes / 1e9:.2f} GB [{nbytes / (time.time() - t0) / 1e6:.1f} MB/s]", end="\r")
print(f"Received {nbytes / 1e9:.2f} GB [{(nbytes / (time.time() - t0)) / 1e6:.1f} MB/s]")
fp.close()
t1 = time.time()
print(f"File {filename} received successfully")
print(f"File size: {nbytes / 1e9:.3f} GB")
print(f"Time taken: {t1 - t0:.2f} seconds")
print(f"Average speed: {(nbytes / (t1 - t0)) / 1e6:.3f} MB/s")

s.close()
