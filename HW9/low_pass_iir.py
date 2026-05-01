import matplotlib.pyplot as plt
import numpy as np
import csv

dt = 1.0/10000.0 # 10kHz
t = np.arange(0.0, 1.0, dt) # 10s
# a constant plus 100Hz and 1000Hz
s = 4.0 * np.sin(2 * np.pi * 100 * t) + 0.25 * np.sin(2 * np.pi * 1000 * t) + 25

Fs = 10000 # sample rate
Ts = 1.0/Fs; # sampling interval

time= []
vals = []
with open('/Users/annalee/Documents/GitHub/HW1-MECH_ENG-433/HW9/sigD.csv', newline='') as f:
    # open the csv file
    reader = csv.reader(f)
    for row in reader:
        # read the rows 1 one by one
        time.append(float(row[0])) # leftmost column
        vals.append(float(row[1])) # second column

    sample_rate = len(vals) / time[-1]

data = vals  
time_sub = time
new_vals = []
A = 0.95
B = 1 - A
# initialize first value
new_vals.append(data[0])

for i in range(1, len(data)):
    y_i = A * new_vals[i-1] + B * data[i]
    new_vals.append(y_i)

sample_rate_sub = sample_rate
#ts = np.arange(0,t[-1],Ts) # time vector

ts = np.arange(0, time[-1], 1/sample_rate)
ts_sub = np.arange(0, time_sub[-1], 1/sample_rate_sub)

#y = s # the data to make the fft from
y = vals # the data to make the fft from
y_sub = new_vals # the data to make the fft from

n = len(y) # length of the signal
n_sub = len(y_sub) # length of the signal

k = np.arange(n)
k_sub = np.arange(n_sub)

T = n/sample_rate
frq = k/T # two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range
Y = np.fft.fft(y)/n # fft computing and normalization
Y = Y[range(int(n/2))]

T_sub = n_sub/sample_rate_sub
frq_sub = k_sub/T_sub # two sides frequency range
frq_sub = frq_sub[range(int(n_sub/2))] # one side frequency range
Y_sub = np.fft.fft(y_sub) / n_sub
Y_sub = Y_sub[range(int(n_sub/2))]


fig, (ax1, ax2) = plt.subplots(2, 1)
fig.suptitle(f'Comparison of IIR low pass and original data, A: {A}, B: {B}')

#ax1.plot(t,y,'b')
ax1.plot(time,vals,'k', label='Original')
ax1.plot(time_sub,new_vals,'r', label='Filtered')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax1.legend()


ax2.loglog(frq, abs(Y), 'k', label='Original')
ax2.loglog(frq_sub, abs(Y_sub), 'r', label='Filtered')
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
ax2.legend()

plt.show()