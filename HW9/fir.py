import numpy as np
import matplotlib.pyplot as plt
import csv
time = []
vals = []

with open('/Users/annalee/Documents/GitHub/HW1-MECH_ENG-433/HW9/sigD.csv', newline='') as f:
    reader = csv.reader(f)
    for row in reader:
        time.append(float(row[0]))
        vals.append(float(row[1]))

time = np.array(time)
vals = np.array(vals)

Fs = len(vals) / time[-1]   # sample rate

# FIR FILTER 
fc = 10#cutoff frequency (Hz) ← CHANGE THIS
N = 401         # number of taps (must be odd)

# normalized cutoff
fc_norm = fc / (Fs / 2)

n = np.arange(N)
center = (N - 1) / 2

# sinc filter
h = np.sinc(fc_norm * (n - center))

# window (Hamming)
window = np.blackman(N)
#window = np.kaiser(N, beta=0)

# apply window
h = h * window

# normalize gain
h = h / np.sum(h)

# APPLY FILTER (convolution)
filtered = np.convolve(vals, h, mode='same')

# FFT FUNCTION
def compute_fft(signal, Fs):
    n = len(signal)
    Y = np.fft.fft(signal) / n
    frq = np.fft.fftfreq(n, d=1/Fs)

    # take only positive half
    mask = frq >= 0
    return frq[mask], np.abs(Y[mask])

frq, Y = compute_fft(vals, Fs)
frq_f, Y_f = compute_fft(filtered, Fs)

# PLOTTING
fig, (ax1, ax2) = plt.subplots(2, 1)

fig.suptitle(f'FIR Low-pass | N={N}, fc={fc} Hz (Hamming Window)')

# TIME DOMAIN
ax1.plot(time, vals, 'k', label='Original')
ax1.plot(time, filtered, 'r', label='Filtered')
ax1.set_xlabel('Time (s)')
ax1.set_ylabel('Amplitude')
ax1.legend()

# FREQUENCY DOMAIN
ax2.loglog(frq, Y, 'k', label='Original')
ax2.loglog(frq_f, Y_f, 'r', label='Filtered')
ax2.set_xlabel('Frequency (Hz)')
ax2.set_ylabel('|Y(f)|')
ax2.legend()

plt.tight_layout()
plt.show()