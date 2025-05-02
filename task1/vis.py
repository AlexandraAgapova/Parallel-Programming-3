import matplotlib.pyplot as plt
import numpy as np

threads = np.array([1, 2, 4, 7, 8, 16, 20, 40])

speedup_20000 = np.array([1.00, 1.99, 4.03, 5.71, 6.14, 8.16, 7.20, 8.36])
speedup_40000 = np.array([1.00, 2.06, 4.22, 6.16, 6.62, 10.05, 10.42, 10.73])

plt.figure(figsize=(8, 6))

plt.plot(threads, speedup_20000, marker='o', label="20000 x 20000")
plt.plot(threads, speedup_40000, marker='s', label="40000 x 40000")

# Идеальная прямая ускорения
plt.plot([0, 40], [0, 40], "--", color="gray", label="Идеальное ускорение")

plt.xlabel("Количество потоков")
plt.ylabel("Ускорение Sp")
plt.title("Масштабируемость умножения матрицы на вектор")
plt.xticks(np.arange(0, 45, 5))
plt.yticks(np.arange(0, 45, 5))
plt.grid(True)
plt.legend()

output_file = "res.png"
plt.savefig(output_file, dpi=300, bbox_inches="tight")
print(f"График сохранён в {output_file}")
