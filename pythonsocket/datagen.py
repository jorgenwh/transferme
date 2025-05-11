import numpy as np

GB = 10
BYTES = GB * 1000**3

data = np.random.bytes(BYTES)
fp = open("data.bin", "wb")
fp.write(data)
fp.close()
print(f"Generated {GB} GB of random data in 'data.bin'")
