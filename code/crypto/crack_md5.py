# -*- coding: utf-8 -*-
from __future__ import print_function, division
import sys
import time
import binascii
import pyopencl as cl
import numpy as np

MAX_PW_LEN = 6  # Don't forget to adjust MAX_PW_LEN in the kernel accordingly
ALPHABET_SIZE = 26

# Read hash from arguments
if len(sys.argv) != 2:
    print('Usage: ./crack_md5.py <md5hash>')
    sys.exit(-1)
input_hash = bytearray(binascii.unhexlify(sys.argv[1]))

# Create context and queue
ctx = cl.create_some_context()
queue = cl.CommandQueue(ctx)

# Prepare result objects
result = bytearray(MAX_PW_LEN)
result_string = bytearray(MAX_PW_LEN)

# Prepare buffers
mf = cl.mem_flags
hash_buf = cl.Buffer(ctx, mf.READ_ONLY | mf.COPY_HOST_PTR, hostbuf=input_hash)
result_buf = cl.Buffer(ctx, mf.WRITE_ONLY | mf.COPY_HOST_PTR, hostbuf=result)

with open('md5.cl', 'r') as f:
    fstr = ''.join(f.readlines())
    prg = cl.Program(ctx, fstr).build()

# Get total start timestamp
t0 = time.time()
times = []

for i in range(1, MAX_PW_LEN + 1):
    # Debug output
    print('Starting round with length %d...' % i)

    # Define work sizes
    global_worksize = []
    for j in range(i):
        if j < 3:
            global_worksize.append(ALPHABET_SIZE)
        else:
            global_worksize[j % 3] *= ALPHABET_SIZE
    print('Work size: %s' % global_worksize)
    local_worksize = None  # Let OpenCL figure out the best value

    # Get round start timestamp
    t1 = time.time()

    # Run kernel!
    prg.crack(queue, global_worksize, local_worksize, hash_buf, result_buf, np.int8(i))

    # Copy result back to device
    cl.enqueue_read_buffer(queue, result_buf, result_string).wait()

    # Get elapsed round time
    t2 = time.time()
    times.append(t2 - t1)

# Get total elapsed time
t3 = time.time()

# Strip null bytes, convert to unicode
plaintext = result_string.strip(b'\x00').decode('ascii')
if plaintext:
    print('Result is "%s"!' % plaintext)
else:
    print('Did not find a result.')

# Print stats
print('\nStats\n-----\n')
print('- Elapsed total time: %fs' % (t3 - t0))
for i, t in enumerate(times):
    print('- Length %d: Finished in %fs' % (i + 1, t))
for i in range(3):
    print('- Length %d: Projected time would be %fs' % (len(times) + i, t * ALPHABET_SIZE ** i))
print('- Keyspace: %d' % (ALPHABET_SIZE ** MAX_PW_LEN))
