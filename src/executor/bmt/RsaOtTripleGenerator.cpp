//
// Created by 杜建璋 on 2024/8/30.
//

#include "executor/bmt/RsaOtTripleGenerator.h"
#include "executor/ot/RsaOtExecutor.h"
#include "utils/Math.h"
#include "utils/Mpi.h"

RsaOtTripleGenerator::RsaOtTripleGenerator(int l) {
    _l = l;
}

void RsaOtTripleGenerator::generateRandomAB() {
    _a0 = Math::rand64(0, (1LL << _l) - 1);
    _b0 = Math::rand64(0, (1LL << _l) - 1);
}

void RsaOtTripleGenerator::computeU() {
    computeMix(0, _u0);
}

void RsaOtTripleGenerator::computeV() {
    computeMix(1, _v0);
}

int64_t RsaOtTripleGenerator::corr(int i, int64_t x) const {
    return Math::ringMod((_a0 << i) - x, _l);
}

void RsaOtTripleGenerator::computeMix(int sender, int64_t &mix) {
    bool isSender = Mpi::rank() == sender;
    int64_t sum = 0;
    for (int i = 0; i < _l; i++) {
        int64_t s0 = 0, s1 = 0;
        int choice = 0;
        if (isSender) {
            s0 = Math::rand64(0, (1LL << _l) - 1);
            s1 = corr(i, s0);
        } else {
            choice = (int)((_b0 >> i) & 1);
        }
        RsaOtExecutor r(sender, s0, s1, choice);
        r.logBenchmark(false);
        if (_benchmarkLevel == BenchmarkLevel::DETAILED) {
            r.benchmark(BenchmarkLevel::DETAILED);
        }
        r.compute();
        if (_benchmarkLevel == BenchmarkLevel::DETAILED) {
            // add mpi time
            _mpiTime += r.mpiTime();
            _otMpiTime += r.mpiTime();
            _otRsaGenerationTime += r.getRsaGenerationTime();
            _otRsaEncryptionTime += r.getRsaEncryptionTime();
            _otRsaDecryptionTime += r.getRsaDecryptionTime();
            _otEntireComputationTime += r.entireComputationTime();
        }
        if (isSender) {
            sum += s0;
        } else {
            int64_t temp = r.result();
            if (choice == 0) {
                temp = Math::ringMod(-r.result(), _l);
            }
            sum += temp;
        }
    }
    mix = Math::ringMod(sum, _l);
}

void RsaOtTripleGenerator::computeC() {
    _c0 = Math::ringMod(_a0 * _b0 + _u0 + _v0, _l);
}

void RsaOtTripleGenerator::compute() {
    generateRandomAB();

    computeU();
    computeV();
    computeC();
}

int64_t RsaOtTripleGenerator::otRsaGenerationTime() const {
    return _otRsaGenerationTime;
}

int64_t RsaOtTripleGenerator::otRsaEncryptionTime() const {
    return _otRsaEncryptionTime;
}

int64_t RsaOtTripleGenerator::otRsaDecryptionTime() const {
    return _otRsaDecryptionTime;
}

int64_t RsaOtTripleGenerator::otMpiTime() const {
    return _otMpiTime;
}

int64_t RsaOtTripleGenerator::otEntireComputationTime() const {
    return _otEntireComputationTime;
}


std::string RsaOtTripleGenerator::tag() const {
    return "[BMT Generator]";
}