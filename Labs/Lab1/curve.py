import numpy as np
import matplotlib.pyplot as plt

# Experimental data
n = np.array([5,10,15,20,25,30,35,40,45,50,55,60,70,80,90,100,110,120,200,500,1000,1500,2000])
ordinary = np.array([4.522,16.846,103.048,150.556,207.532,273.354,657.926,1454.63,
                     1811.388,2642.514,3028.104,3578.038,9621.615,13831.32,
                     16495.85,19044.37,22551.68,26728.385,69852.47,851557.4,
                     11452375.5,54580416.8,143505110])
strassen = np.array([1425.35,9959.524,9430.688,63808.25,64851.16,71175.02,453375.3,
                     451203.3,470522.8,469929.2,448538.6,476238.2,2950251,3028525,
                     3086257,2899294,2914468,3083276,14839065,98522252.3,
                     833197238.6,6241155812,6046228300])

# Fit log(time) = a + b*log(n)
def fit_powerlaw(n, t):
    b, a = np.polyfit(np.log(n), np.log(t), 1)
    return a, b  # log(k), exponent

a1, b1 = fit_powerlaw(n, ordinary)
a2, b2 = fit_powerlaw(n, strassen)

# Predicted curves from fitted exponents
n_fit = np.linspace(min(n), max(n), 200)
ordinary_fit = np.exp(a1) * n_fit**b1
strassen_fit = np.exp(a2) * n_fit**b2

# Theoretical reference curves (normalized for visibility)
n3_curve = (n_fit / n_fit[0])**3 * ordinary[0]
n28_curve = (n_fit / n_fit[0])**2.8 * strassen[0]

print(f"Ordinary fit exponent ≈ {b1:.2f}")
print(f"Strassen fit exponent ≈ {b2:.2f}")

# Plot experimental and theoretical curves (log–log)
plt.figure(figsize=(8,6))
plt.loglog(n, ordinary, 'o', label='Ordinary data')
plt.loglog(n, strassen, 's', label='Strassen data')
plt.loglog(n_fit, ordinary_fit, '-', label=f'Ordinary fit n^{b1:.2f}')
plt.loglog(n_fit, strassen_fit, '-', label=f'Strassen fit n^{b2:.2f}')
plt.loglog(n_fit, n3_curve, '--', color='gray', label=r'Reference $n^3$')
plt.loglog(n_fit, n28_curve, '--', color='black', label=r'Reference $n^{2.8}$')

plt.xlabel("Matrix size n")
plt.ylabel("Time (ms)")
plt.title("Empirical vs. Theoretical Growth (log–log scale)")
plt.legend()
plt.grid(True, which="both", ls="--", lw=0.5)
plt.show()