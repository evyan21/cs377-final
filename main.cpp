#include <iostream>
#include <fstream>
#include <chrono>

using namespace std;

struct OpStats {
    double avgAccessTime;
    double throughput;
};

OpStats measureRead(string filename, int blockSize) {
    ifstream input(filename, ios::ate);

    if (!input.is_open()) {
        cout << "error reading: could not open file" << endl;
        return {0.0, 0.0};
    }

    int fileSize = input.tellg();

    // move pointer to the beginning
    input.seekg(0, ios::beg);

    char* buffer = new char[blockSize];

    // start timer
    auto start = chrono::steady_clock::now();
    int bytesRead = 0;

    // continue reading until hit end of file
    while (bytesRead < fileSize) {
        input.read(buffer, blockSize);
        bytesRead += input.gcount();
    }

    // end timer
    auto end = chrono::steady_clock::now();
    // calculate duration
    auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start).count();

    delete[] buffer;

    double avgAccessTime = duration / static_cast<double>(fileSize);
    double throughput = (fileSize / static_cast<double>(1024 * 1024)) / (duration / 1e9);

    return {avgAccessTime, throughput};
}

OpStats measureWrite(string filename, int blockSize) {
    ofstream output(filename);

    if (!output.is_open()) {
        cout << "error writing: could not open file" << endl;
        return {0.0, 0.0};
    }

    char* buffer = new char[blockSize];

    // start timer
    auto start = chrono::steady_clock::now();

    // continue writing for 10 MB
    for (int i = 0; i < 1024 * 1024 * 10 / blockSize; i++) {
        output.write(buffer, blockSize);
    }

    // end timer
    auto end = chrono::steady_clock::now();
    // calculate duration
    auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start).count();

    delete[] buffer;

    double avgAccessTime = duration / static_cast<double>(1024 * 1024 * 10);
    double throughput = (10) / (duration / 1e9);

    return {avgAccessTime, throughput};
}

OpStats measureSeek(string filename, int numSeeks) {
    ifstream input(filename, ios::ate);

    if (!input.is_open()) {
        cout << "error reading: could not open file" << endl;
        return {0.0, 0.0};
    }

    int fileSize = input.tellg();

    // move pointer to the beginning
    input.seekg(0, ios::beg);

    // start timer
    auto start = chrono::steady_clock::now();

    // continue reading until hit end of file
    for (int i = 0; i < numSeeks; i++) {
        int pos = rand() % fileSize;
        input.seekg(pos, ios::beg);
    }

    // end timer
    auto end = chrono::steady_clock::now();
    // calculate duration
    auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start).count();

    double avgAccessTime = duration / static_cast<double>(numSeeks);
    double throughput = (numSeeks / 1e6) / (duration / 1e9);

    return {avgAccessTime, throughput};
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        fprintf(stderr, "usage: %s <FileName> \n", argv[0]);
        exit(0);
    } 

    string filename = argv[1];
    cout << "File name: " << filename << endl;
    const int blockSize = 1028;
    const int numSeeks = 100000;

    // Measure performance of write operation
    const auto writeStats = measureWrite(filename, blockSize);
    cout << "Write operation stats:\n";
    cout << "Average access time: " << writeStats.avgAccessTime << " ns\n";
    cout << "Throughput: " << writeStats.throughput << " MB/s\n";

    // Measure performance of read operation
    const auto readStats = measureRead(filename, blockSize);
    cout << "Read operation stats:\n";
    cout << "Average access time: " << readStats.avgAccessTime << " ns\n";
    cout << "Throughput: " << readStats.throughput << " MB/s\n";

    // Measure performance of seek operation
    const auto seekStats = measureSeek(filename, numSeeks);
    cout << "Seek operation stats:\n";
    cout << "Average access time: " << seekStats.avgAccessTime << " ns\n";
    cout << "Throughput: " << seekStats.throughput << "*10^6 ops/s\n";

    return 0;
}