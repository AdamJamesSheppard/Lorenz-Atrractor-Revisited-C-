// Deterministic, streaming classical RK4 for the Lorenz-63 ODE.
// Timesteps are dependent and deliberately sequential.
#include <array>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

using State = std::array<double, 3>;

double number(const char* text) {
    std::size_t used = 0;
    const std::string value(text);
    const double result = std::stod(value, &used);
    if (used != value.size() || !std::isfinite(result))
        throw std::invalid_argument("Expected a finite numeric argument");
    return result;
}

State rhs(const State& u, double sigma, double rho, double beta) {
    return {sigma * (u[1] - u[0]), u[0] * (rho - u[2]) - u[1],
            u[0] * u[1] - beta * u[2]};
}

State shifted(const State& u, const State& k, double scale) {
    State result{};
    for (int j = 0; j < 3; ++j) result[j] = u[j] + scale * k[j];
    return result;
}

State step(const State& u, double dt, double sigma, double rho, double beta) {
    const auto k1 = rhs(u, sigma, rho, beta);
    const auto k2 = rhs(shifted(u, k1, dt / 2), sigma, rho, beta);
    const auto k3 = rhs(shifted(u, k2, dt / 2), sigma, rho, beta);
    const auto k4 = rhs(shifted(u, k3, dt), sigma, rho, beta);
    State result{};
    for (int j = 0; j < 3; ++j) {
        result[j] = u[j] + dt * (k1[j] + 2*k2[j] + 2*k3[j] + k4[j]) / 6;
        if (!std::isfinite(result[j]))
            throw std::runtime_error("Non-finite state: partial output is invalid");
    }
    return result;
}

int main(int argc, char** argv) {
    try {
        if (argc != 7 && argc != 10)
            throw std::invalid_argument("Usage: lorenz_rk4 STEPS DT OUTPUT.csv X0 Y0 Z0 [SIGMA RHO BETA]");
        const std::string count(argv[1]);
        if (count.empty() || count.find_first_not_of("0123456789") != std::string::npos)
            throw std::invalid_argument("STEPS must be a nonnegative integer");
        const auto steps = std::stoull(count);
        if (steps == std::numeric_limits<unsigned long long>::max())
            throw std::invalid_argument("STEPS would overflow the loop counter");
        const double dt = number(argv[2]);
        if (dt <= 0 || !std::isfinite(steps * dt))
            throw std::invalid_argument("DT must be positive and final time finite");
        State u{number(argv[4]), number(argv[5]), number(argv[6])};
        const double sigma = argc == 10 ? number(argv[7]) : 10;
        const double rho = argc == 10 ? number(argv[8]) : 28;
        const double beta = argc == 10 ? number(argv[9]) : 8.0/3.0;
        if (std::filesystem::exists(argv[3]))
            throw std::invalid_argument("Output already exists; choose a fresh path");
        std::ofstream out(argv[3]);
        if (!out) throw std::runtime_error("Cannot open output");
        out << std::setprecision(std::numeric_limits<double>::max_digits10);
        out << "t,x,y,z\n";
        for (unsigned long long i = 0; i <= steps; ++i) {
            out << i * dt << ',' << u[0] << ',' << u[1] << ',' << u[2] << '\n';
            if (i != steps) u = step(u, dt, sigma, rho, beta);
        }
        out.close();
        if (!out) throw std::runtime_error("Output write failed; partial file is invalid");
        std::cout << "Completed " << steps << " steps; T=" << steps * dt << '\n';
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
