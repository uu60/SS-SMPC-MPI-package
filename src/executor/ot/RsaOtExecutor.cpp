//
// Created by 杜建璋 on 2024/7/15.
//

#include "executor/ot/RsaOtExecutor.h"
#include "utils/MpiUtils.h"
#include "utils/MathUtils.h"
#include "utils/CryptUtils.h"

RsaOtExecutor::RsaOtExecutor(int sender, int64_t m0, int64_t m1, int i)
        : RsaOtExecutor(2048, sender, m0, m1, i) {
}

RsaOtExecutor::RsaOtExecutor(int bits, int sender, int64_t m0, int64_t m1, int i) {
    _bits = bits;
    _isSender = sender == MpiUtils::rank();
    if (_isSender) {
        _m0 = m0;
        _m1 = m1;
    } else {
        _i = i;
    }
}

void RsaOtExecutor::compute() {
    int64_t start, end;
    if (_benchmarkLevel >= BenchmarkLevel::GENERAL) {
        start = System::currentTimeMillis();
    }
    // preparation
    generateAndShareRandoms();
    generateAndShareRsaKeys();

    // process
    process();
    if (_benchmarkLevel >= BenchmarkLevel::GENERAL) {
        end = System::currentTimeMillis();
        if (_isLogBenchmark) {
            Log::i(tag() + " Entire computation time: " + std::to_string(end - start) + "ms.");
            Log::i(tag() + " MPI transmission and synchronization time: " + std::to_string(_mpiTime) + " ms.");
        }
        _entireComputationTime = end - start;
    }
}

void RsaOtExecutor::generateAndShareRsaKeys() {
    if (_isSender) {
        int64_t start, end;
        if (_benchmarkLevel == BenchmarkLevel::DETAILED) {
            start = System::currentTimeMillis();
        }
        CryptUtils::generateRsaKeys(_bits, _pub, _pri);
        if (_benchmarkLevel == BenchmarkLevel::DETAILED) {
            end = System::currentTimeMillis();
            if (_isLogBenchmark) {
                Log::i(tag() + " RSA keys generation time: " + std::to_string(end - start) + " ms.");
            }
            _rsaGenerationTime = end - start;
            MpiUtils::send(&_pub, _mpiTime);
        } else {
            MpiUtils::send(&_pub);
        }
        return;
    }
    // receiver
    if (_benchmarkLevel == BenchmarkLevel::DETAILED) {
        MpiUtils::recv(&_pub, _mpiTime);
    } else {
        MpiUtils::recv(&_pub);
    }
}

void RsaOtExecutor::generateAndShareRandoms() {
    // 11 for PKCS#1 v1.5 padding
    int maxLen = (_bits >> 3) - 11;
    if (_isSender) {
        _rand0 = MathUtils::rand0b(1, maxLen);
        _rand1 = MathUtils::rand0b(1, maxLen);
        if (_benchmarkLevel == BenchmarkLevel::DETAILED) {
            MpiUtils::send(&_rand0, _mpiTime);
            MpiUtils::send(&_rand1, _mpiTime);
        } else {
            MpiUtils::send(&_rand0);
            MpiUtils::send(&_rand1);
        }
    } else {
        _randK = MathUtils::rand0b(1, maxLen);
        if (_benchmarkLevel == BenchmarkLevel::DETAILED) {
            MpiUtils::recv(&_rand0, _mpiTime);
            MpiUtils::recv(&_rand1, _mpiTime);
        } else {
            MpiUtils::recv(&_rand0);
            MpiUtils::recv(&_rand1);
        }
    }
}

void RsaOtExecutor::process() {
    if (!_isSender) {
        int64_t start, end;
        if (_benchmarkLevel == BenchmarkLevel::DETAILED) {
            start = System::currentTimeMillis();
        }
        std::string ek = CryptUtils::rsaEncrypt(_randK, _pub);
        if (_benchmarkLevel == BenchmarkLevel::DETAILED) {
            end = System::currentTimeMillis();
            if (_isLogBenchmark) {
                Log::i(tag() + " RSA encryption time: " + std::to_string(end - start) + " ms.");
            }
            _rsaEncryptionTime = end - start;
        }
        std::string sumStr = MathUtils::add(ek, _i == 0 ? _rand0 : _rand1);
        if (_benchmarkLevel == BenchmarkLevel::DETAILED) {
            MpiUtils::send(&sumStr, _mpiTime);
        } else {
            MpiUtils::send(&sumStr);
        }

        std::string m0, m1;
        if (_benchmarkLevel == BenchmarkLevel::DETAILED) {
            MpiUtils::recv(&m0);
            MpiUtils::recv(&m1);
        } else {
            MpiUtils::recv(&m0, _mpiTime);
            MpiUtils::recv(&m1, _mpiTime);
        }

        _res = std::stoll(MathUtils::minus(_i == 0 ? m0 : m1, _randK));
    } else {
        std::string sumStr;
        if (_benchmarkLevel == BenchmarkLevel::DETAILED) {
            MpiUtils::recv(&sumStr, _mpiTime);
        } else {
            MpiUtils::recv(&sumStr);
        }
        int64_t start, end;
        if (_benchmarkLevel == BenchmarkLevel::DETAILED) {
            start = System::currentTimeMillis();
        }
        std::string k0 = CryptUtils::rsaDecrypt(
                MathUtils::minus(sumStr, _rand0), _pri
        );
        std::string k1 = CryptUtils::rsaDecrypt(
                MathUtils::minus(sumStr, _rand1), _pri
        );
        if (_benchmarkLevel == BenchmarkLevel::DETAILED) {
            end = System::currentTimeMillis();
            if (_isLogBenchmark) {
                Log::i(tag() + " RSA decryption time: " + std::to_string(end - start) + " ms.");
            }
            _rsaDecryptionTime = end - start;
        }
        std::string m0 = MathUtils::add(std::to_string(_m0), k0);
        std::string m1 = MathUtils::add(std::to_string(_m1), k1);
        if (_benchmarkLevel == BenchmarkLevel::DETAILED) {
            MpiUtils::send(&m0, _mpiTime);
            MpiUtils::send(&m1, _mpiTime);
        } else {
            MpiUtils::send(&m0);
            MpiUtils::send(&m1);
        }
    }
}

int64_t RsaOtExecutor::getRsaGenerationTime() const {
    return _rsaGenerationTime;
}

int64_t RsaOtExecutor::getRsaEncryptionTime() const {
    return _rsaEncryptionTime;
}

int64_t RsaOtExecutor::getRsaDecryptionTime() const {
    return _rsaDecryptionTime;
}

std::string RsaOtExecutor::tag() const {
    return "[RSA OT]";
}