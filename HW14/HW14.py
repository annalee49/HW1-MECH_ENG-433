#!/usr/bin/env python3
import sys
import serial
import time
import numpy as np
import matplotlib.pyplot as plt

if len(sys.argv) < 2:
    print("Usage: plot.py <serial_port> [baudrate]")
    sys.exit(1)

port = sys.argv[1]
baud = int(sys.argv[2]) if len(sys.argv) > 2 else 115200
NUM_LINES = 5 * 80  # 5 seconds at 80Hz

def read_lines(ser, n):
    lines = []
    start = time.time()
    while len(lines) < n and (time.time() - start) < 50.0:
        line = ser.readline()
        if not line:
            continue
        lines.append(line.decode('utf-8').strip())
    return lines

with serial.Serial(port, baud, timeout=10) as ser:
    ser.reset_input_buffer()
    ser.reset_output_buffer()

    print("Collection requested...")
    ser.write(f"{NUM_LINES}\n".encode('utf-8'))
    ser.flush()

    raw_lines = read_lines(ser, NUM_LINES)
    print("Collected " + str(len(raw_lines)) + " lines")
    if len(raw_lines) < NUM_LINES:
        print(f"Warning: expected {NUM_LINES} lines, got {len(raw_lines)}")

    # parse lines: index time raw filtered
    indices = []
    times = []
    raw_vals = []
    filt_vals = []

    for ln in raw_lines:
        parts = ln.split()

        print(parts)
        if len(parts) != 4:
            continue

        try:
            idx = int(parts[0])
            t = float(parts[1])
            raw = float(parts[2])
            filt = float(parts[3])
        except ValueError:
            continue

        indices.append(idx)
        times.append(t)
        raw_vals.append(raw)
        filt_vals.append(filt)

    times = np.array(times)
    raw_vals = np.array(raw_vals)
    filt_vals = np.array(filt_vals)

    # normalize time to start at 0
    times = (times - times[0]) / 1000.0  # ms to seconds

    # plot raw and filtered vs time
    plt.figure()
    plt.plot(times, raw_vals, label='Raw')
    plt.plot(times, filt_vals, label='Filtered')
    plt.xlabel('Time (s)')
    plt.ylabel('Value')
    plt.title('HX711 Raw vs Filtered')
    plt.legend()
    plt.savefig('data.png')
    plt.show()

    # FFT of both
    N = len(raw_vals)
    dt = np.mean(np.diff(times))
    freqs = np.fft.rfftfreq(N, d=dt)
    raw_vals = raw_vals - np.mean(raw_vals)
    filt_vals = filt_vals - np.mean(filt_vals)


    fft_raw = np.abs(np.fft.rfft(raw_vals))
    fft_filt = np.abs(np.fft.rfft(filt_vals))

    plt.figure()
    plt.plot(freqs, fft_raw, label='FFT Raw')
    plt.plot(freqs, fft_filt, label='FFT Filtered')
    plt.xlabel('Frequency (Hz)')
    plt.ylabel('Magnitude')
    plt.title('FFT of Raw vs Filtered')
    plt.legend()
    plt.xscale('log')
    plt.yscale('log')
    plt.xlim([0.1, 40])
    plt.ylim([1e2, 1e6])
    plt.savefig('fft.png')
    plt.show()