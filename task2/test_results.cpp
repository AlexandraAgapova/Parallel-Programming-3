#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

bool compare(double a, double b) {
    return std::abs(a - b) < 1e-5;
}

void test_file(const std::string& filename) {
    std::ifstream fin(filename);
    std::string line;
    int lineNum = 1;
    while (std::getline(fin, line)) {
        size_t eq = line.find(" = ");
        if (eq == std::string::npos) continue;

        std::string expr = line.substr(0, eq);
        double saved = std::stod(line.substr(eq + 3));

        double computed = 0.0;

        if (expr.find("sin(") == 0) {
            double arg = std::stod(expr.substr(4, expr.size() - 5));
            computed = std::sin(arg);
        } else if (expr.find("sqrt(") == 0) {
            double arg = std::stod(expr.substr(5, expr.size() - 6));
            computed = std::sqrt(arg);
        } else if (expr.find("^") != std::string::npos) {
            size_t pos = expr.find("^");
            double base = std::stod(expr.substr(0, pos));
            double exp = std::stod(expr.substr(pos + 1));
            computed = std::pow(base, exp);
        }

        if (!compare(computed, saved)) {
            std::cerr << "Mismatch in " << filename << " on line " << lineNum
                      << ": got " << saved << ", expected " << computed << "\n";
        }

        lineNum++;
    }
}

int main() {
    test_file("sin_results.txt");
    test_file("sqrt_results.txt");
    test_file("pow_results.txt");
    std::cout << "Тестирование завершено.\n";
    return 0;
}
