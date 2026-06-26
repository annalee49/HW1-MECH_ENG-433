import numpy as np
import matplotlib.pyplot as plt

x = np.linspace(-1, 1, 1000)

# Stronger toggle effect
alpha = 2.0
F = x - alpha * x**3

# Normalize to +/-1
F = F / np.max(np.abs(F))

plt.figure(figsize=(8, 5))
plt.plot(x, F, linewidth=2)

plt.xlabel('Normalized Displacement')
plt.ylabel('Normalized Force')
plt.title('Haptic Toggle Switch')
plt.grid(True)

plt.show()

import numpy as np
import matplotlib.pyplot as plt

# Normalized displacement
x = np.linspace(-1, 1, 1000)

# Parameters
A = 0.8      # bump/dip strength
sigma = 0.25 # width

# Bump and dip force curves
F_bump = x + A * np.exp(-(x**2)/(sigma**2))
F_dip  = x - A * np.exp(-(x**2)/(sigma**2))

# Normalize to +/- 1
F_bump = F_bump / np.max(np.abs(F_bump))
F_dip  = F_dip  / np.max(np.abs(F_dip))

# Plot
plt.figure(figsize=(8,5))
#plt.plot(x, F_bump, linewidth=2, label='Bump')
plt.plot(x, F_dip, linewidth=2, label='Dip')

plt.axhline(0, color='black', linewidth=0.5)
plt.axvline(0, color='black', linewidth=0.5)

plt.xlabel('Normalized Displacement')
plt.ylabel('Normalized Force')
plt.title('Haptic Dip Force Curve')
plt.xlim([-1,1])
plt.ylim([-1.1,1.1])
plt.grid(True)
plt.legend()

plt.show()