import matplotlib.pyplot as plt

data = [
    (1, 500, 165.88), (1, 1000, 1643.41), (1, 1500, 5211.26), (1, 2000, 18857.20),
    (2, 500, 83.30), (2, 1000, 856.74), (2, 1500, 3002.77), (2, 2000, 12725.69),
    (4, 500, 45.62), (4, 1000, 533.78), (4, 1500, 1467.75), (4, 2000, 7743.14),
    (8, 500, 36.97), (8, 1000, 1079.31), (8, 1500, 2935.37), (8, 2000, 13693.31),
    (16, 500, 68.78), (16, 1000, 1108.70), (16, 1500, 2925.01), (16, 2000, 8731.95),
    (32, 500, 72.58), (32, 1000, 954.24), (32, 1500, 2581.36), (32, 2000, 10189.48),
    (64, 500, 66.08), (64, 1000, 994.82), (64, 1500, 2765.36), (64, 2000, 9414.73),
    (128, 500, 60.46), (128, 1000, 821.88), (128, 1500, 2586.23), (128, 2000, 9653.77)
]

threads_map = {}
for t, size, time in data:
    if t not in threads_map:
        threads_map[t] = {'size': [], 'time': []}
    threads_map[t]['size'].append(size)
    threads_map[t]['time'].append(time)

plt.figure(figsize=(10, 6))
for t in sorted(threads_map.keys()):
    plt.plot(threads_map[t]['size'], threads_map[t]['time'], marker='o', label=f'{t} Потоки')

plt.title('Зависимость времени выполнения от размера матрицы и числа потоков')
plt.xlabel('Размер матрицы (N x N)')
plt.ylabel('Время выполнения (мс)')
plt.grid(True, linestyle='--', alpha=0.7)
plt.legend()

plt.tight_layout()
plt.savefig('matrix_performance.png')
plt.show()