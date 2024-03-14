//
// Created by pepe on 3/14/24.
//

#ifndef HTTP_SERVER_COMPUTATIONSERVICE_H
#define HTTP_SERVER_COMPUTATIONSERVICE_H


#include <vector>

class ComputationService {
public:
    static std::vector<long> computePrimesUpToN(long n);
    static std::vector<std::pair<long,long>> computePrimeFactors(long n);
};


#endif //HTTP_SERVER_COMPUTATIONSERVICE_H
