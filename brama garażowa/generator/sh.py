import matplotlib.pyplot as plt
import numpy as np

# Wczytywanie danych z pliku
data = np.fromfile('o.raw', dtype=np.float32)

# Tworzenie wykresu
plt.plot(data)
plt.title('Wykres danych')
plt.xlabel('Numer próbki')
plt.ylabel('Wartość')
plt.show()
