#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <array>
#include <chrono>
#include <random>
#define n 1000000 // number of points, time step is 0.01 thus resolution is 10000000. Increases how far the system moves in time
#define EIGHT_THIRDS 2.66666666666666666666667 
// Ring Buffer
const int buffer_size = n;
std::array<std::array<double, 4>, buffer_size> buffer;
int buffer_index = 0;
// lorenz equation - can be generalised 
double xBound_Coordinates(double x, double y, double z)
{
    return 10 * (y - x);
}

double yBound_Coordinates(double x, double y, double z)
{
    return x * (28 - z) - y;
}

double zBound_Coordinates(double x, double y, double z)
{
    return x * y - EIGHT_THIRDS*z;
}
int main()
{
    // start timing process - ignore
    auto start = std::chrono::high_resolution_clock::now();
    //define variables used in runge-kutta algorithm in the scope of main()
    double x, y, z, t, TimeStep, xSlope_RoundOne, xSlope_RoundTwo, xSlope_RoundThree, xSlope_RoundFour, ySlope_RoundOne, ySlope_RoundTwo, ySlope_RoundThree, ySlope_RoundFour, zSlope_RoundOne, zSlope_RoundTwo, zSlope_RoundThree, zSlope_RoundFour;
    //define and open data file to write data to
    std::ofstream outfile; 
    outfile.open("lorenz.dat");
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(-1, 1); // define the range
    // intialise lorenz system 
    x = distr(gen);
    y = distr(gen);
    z = distr(gen);
    t = distr(gen);
    // corresponding to time step in particular range n, increases resolution regardless of increase in n. 
    TimeStep = 0.01; 
    // check buffer index is in valid range 
    if (buffer_index >= 0 && buffer_index < buffer.size())
    {
        buffer[buffer_index][0] = t;
        buffer[buffer_index][1] = x;
        buffer[buffer_index][2] = y;
        buffer[buffer_index][3] = z;
    }
#pragma omp parallel
    {
#pragma omp for private(k1, l1, m1, k2, l2, m2, k3, l3, m3, k4, l4, m4) shared(x, y, z)
        for (int i = 0; i < n; i++)
        {
            xSlope_RoundOne = TimeStep * xBound_Coordinates(x, y, z);
            ySlope_RoundOne = TimeStep * yBound_Coordinates(x, y, z);
            zSlope_RoundOne = TimeStep * zBound_Coordinates(x, y, z);

            xSlope_RoundTwo = TimeStep * xBound_Coordinates(x + xSlope_RoundOne / 2, y + ySlope_RoundOne / 2, z + zSlope_RoundOne / 2);
            ySlope_RoundTwo = TimeStep * yBound_Coordinates(x + xSlope_RoundOne / 2, y + ySlope_RoundOne / 2, z + zSlope_RoundOne / 2);
            zSlope_RoundTwo = TimeStep * zBound_Coordinates(x + xSlope_RoundOne / 2, y + ySlope_RoundOne / 2, z + zSlope_RoundOne / 2);

            xSlope_RoundThree = TimeStep * xBound_Coordinates(x + xSlope_RoundTwo / 2, y + ySlope_RoundTwo / 2, z + zSlope_RoundTwo / 2);
            ySlope_RoundThree = TimeStep * yBound_Coordinates(x + xSlope_RoundTwo / 2, y + ySlope_RoundTwo / 2, z + zSlope_RoundTwo / 2);
            zSlope_RoundThree = TimeStep * zBound_Coordinates(x + xSlope_RoundTwo / 2, y + ySlope_RoundTwo / 2, z + zSlope_RoundTwo / 2);

            xSlope_RoundFour = TimeStep * xBound_Coordinates(x + xSlope_RoundThree, y + ySlope_RoundThree, z + zSlope_RoundThree);
            ySlope_RoundFour = TimeStep * yBound_Coordinates(x + xSlope_RoundThree, y + ySlope_RoundThree, z + zSlope_RoundThree);
            zSlope_RoundFour = TimeStep * zBound_Coordinates(x + xSlope_RoundThree, y + ySlope_RoundThree, z + zSlope_RoundThree);

            x = x + (xSlope_RoundOne + 2 * xSlope_RoundTwo + 2 * xSlope_RoundThree + xSlope_RoundFour) / 6;
            y = y + (ySlope_RoundOne + 2 * ySlope_RoundTwo + 2 * ySlope_RoundThree + ySlope_RoundFour) / 6;
            z = z + (zSlope_RoundOne + 2 * zSlope_RoundTwo + 2 * zSlope_RoundThree + zSlope_RoundFour) / 6;
            

            t = t + TimeStep;
            buffer[buffer_index][0] = t;
            buffer[buffer_index][1] = x;
            buffer[buffer_index][2] = y;
            buffer[buffer_index][3] = z;
            buffer_index = (buffer_index + 1) % buffer_size;
        }
        double xSolutions[n];
        for (int i = 0; i < buffer_size; i++)
        {
            int index = (buffer_index + i) % buffer_size;
            outfile << std::fixed << std::setprecision(64) << buffer[index][0] << " " << buffer[index][1] << " " << buffer[index][2] << " " << buffer[index][3] << '\n';
        }


        outfile.close();
        auto end = std::chrono::high_resolution_clock::now();
        double elapsed_time = std::chrono::duration<double>(end - start).count();
        std::cout << "Elapsed time: " << elapsed_time << " seconds" << '\n';
        return 0;
#pragma omp critical
    }
}
