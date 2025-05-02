#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <numeric>

constexpr int NUM_RUNS = 10;
const std::vector<int> THREAD_COUNTS = {1, 2, 4, 7, 8, 16, 20, 40};
const std::vector<int> MATRIX_SIZES = {20000, 40000};

void initializeMatrix(std::vector<std::vector<double>> &matrix, int start, int end) {
    int size = matrix.size();
    for (int i = start; i < end; i++)
        for (int j = 0; j < size; j++)
            matrix[i][j] = static_cast<double>(i + j);
}

void initializeVector(std::vector<double> &vec, int start, int end) {
    for (int i = start; i < end; i++)
        vec[i] = static_cast<double>(i);
}

void multiplyMatrixVector(const std::vector<std::vector<double>> &matrix,
                          const std::vector<double> &vec,
                          std::vector<double> &result,
                          int start, int end) {
    int size = matrix.size();
    for (int i = start; i < end; i++) {
        double sum = 0.0;
        for (int j = 0; j < size; j++)
            sum += matrix[i][j] * vec[j];
        result[i] = sum;
    }
}

double runOnce(int matrixSize, int numThreads) {
    std::vector<std::vector<double>> matrix(matrixSize, std::vector<double>(matrixSize));
    std::vector<double> vec(matrixSize), result(matrixSize);
    std::vector<std::thread> threads;
    int chunk = matrixSize / numThreads;

    auto start = std::chrono::high_resolution_clock::now();

    // Инициализация матрицы
    for (int i = 0; i < numThreads; ++i)
        threads.emplace_back(initializeMatrix, std::ref(matrix), i * chunk, (i + 1) * chunk);
    for (auto &t : threads) t.join();
    threads.clear();

    // Инициализация вектора
    for (int i = 0; i < numThreads; ++i)
        threads.emplace_back(initializeVector, std::ref(vec), i * chunk, (i + 1) * chunk);
    for (auto &t : threads) t.join();
    threads.clear();

    // Умножение
    for (int i = 0; i < numThreads; ++i)
        threads.emplace_back(multiplyMatrixVector, std::cref(matrix), std::cref(vec), std::ref(result), i * chunk, (i + 1) * chunk);
    for (auto &t : threads) t.join();

    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(end - start).count();
}

int main() {
    for (int size : MATRIX_SIZES) {
        std::cout << "\n=== Размер матрицы: " << size << " x " << size << " ===\n";
        std::vector<double> averageTimes;

        for (int threads : THREAD_COUNTS) {
            std::vector<double> times;
            for (int run = 0; run < NUM_RUNS; ++run) {
                double t = runOnce(size, threads);
                times.push_back(t);
                std::cout << "Размер: " << size << " | Потоки: " << std::setw(2) << threads
                          << " | Запуск №" << std::setw(2) << run + 1
                          << " | Время: " << std::fixed << std::setprecision(4) << t << " с\n";
            }

            std::sort(times.begin(), times.end());
            double sum = std::accumulate(times.begin() + 1, times.end() - 1, 0.0);
            double avg = sum / (NUM_RUNS - 2);
            averageTimes.push_back(avg);
            std::cout << "Среднее время (без крайних): " << std::fixed << std::setprecision(4) << avg << " с\n\n";
        }

        std::cout << "== Таблица ускорения для " << size << "x" << size << " ==\n";
        std::cout << "Потоки\tСреднее время\tУскорение Sp\n";
        double baseTime = averageTimes[0];
        for (size_t i = 0; i < THREAD_COUNTS.size(); ++i) {
            double sp = baseTime / averageTimes[i];
            std::cout << THREAD_COUNTS[i] << "\t"
                      << std::fixed << std::setprecision(4) << averageTimes[i] << "\t\t"
                      << std::fixed << std::setprecision(2) << sp << '\n';
        }
    }
    return 0;
}
