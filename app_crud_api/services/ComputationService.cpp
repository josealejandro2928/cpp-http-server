//
// Created by pepe on 3/14/24.
//

#include "ComputationService.h"

std::vector<long> ComputationService::computePrimesUpToN(long n) {
    std::vector<long> res;
    for (int i = 2; i <= n; i++) {
        bool isPrime = true;
        for (int j = 2; j * j <= i; j++) {
            if (i % j == 0) {
                isPrime = false;
                break;
            }
        }
        if (isPrime) {
            res.push_back(i);
        }
    }
    return res;
}

std::vector<std::pair<long, long>> ComputationService::computePrimeFactors(long n) {
    auto primeFactors = computePrimesUpToN(n);
    std::vector<std::pair<long, long>> res;
    int index = 0;
    while (n > 1 && index <= n) {
        int c = 0;
        while (n > 1 && n % primeFactors[index] == 0) {
            n /= primeFactors[index];
            c++;
        }
        if (c) {
            res.emplace_back(primeFactors[index], c);
        }
        index++;
    }
    return res;
}
