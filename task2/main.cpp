#include "task_server.h"
#include <iostream>
#include <fstream>
#include <random>
#include <cmath>
#include <thread>

const int N = 50;
std::mt19937 gen(std::random_device{}());
std::uniform_real_distribution<> dist(0.1, 100.0);

template<typename Func>
void run_client(TaskServer<double>& server, const std::string& filename, Func func, const std::string& format) {
    std::ofstream fout(filename);
    std::vector<size_t> ids;
    std::vector<std::string> inputs;

    for (int i = 0; i < N; ++i) {
        double a = dist(gen), b = dist(gen)/10.0;
        ids.push_back(server.add_task([=]() { return func(a, b); }));
        char buf[64];
        snprintf(buf, sizeof(buf), format.c_str(), a, b);
        inputs.push_back(buf);
    }

    for (int i = 0; i < N; ++i) {
        double result = server.request_result(ids[i]);
        fout << inputs[i] << " = " << result << '\n';
    }
}

int main() {
    TaskServer<double> server;
    server.start();

    std::thread t1([&]() {
        run_client(server, "sin_results.txt",
                [](double x, double) { return std::sin(x); }, "sin(%.3f)");
    });

    std::thread t2([&]() {
        run_client(server, "sqrt_results.txt",
                [](double x, double) { return std::sqrt(x); }, "sqrt(%.3f)");
    });

    std::thread t3([&]() {
        run_client(server, "pow_results.txt",
                [](double x, double y) { return std::pow(x, y); }, "%.3f^%.3f");
    });


    t1.join();
    t2.join();
    t3.join();

    server.stop();
    return 0;
}
