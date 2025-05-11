import os
import time
import socket

FILENAME = "data.bin"
HOST = "127.0.0.1"
PORT = 5000
BUFFER_SIZE = 16384

if not os.path.exists(FILENAME):
    print(f"File {FILENAME} does not exist")
    exit(1)

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))
print(f"Connected to {HOST}:{PORT}")

filename_encoded = FILENAME.encode("utf-8")
filename_length = len(filename_encoded)
s.send(filename_length.to_bytes(4, "big"))
s.send(filename_encoded)

print(f"Sending file: {FILENAME}...")
t0 = time.time()
fp = open(FILENAME, "rb")
nbytes = 0
while True:
    data = fp.read(BUFFER_SIZE)
    nbytes += len(data)
    if not data:
        break
    s.send(data)
fp.close()
t1 = time.time()
print(f"File {FILENAME} sent successfully")
print(f"File size: {nbytes / 1e9:.3f} GB")
print(f"Time taken: {t1 - t0:.2f} seconds")
print(f"Average speed: {(nbytes / (t1 - t0)) / 1e9:.3f} GB/s")

s.close()
