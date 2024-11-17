//
// Created by 杜建璋 on 2024/7/15.
//

#include "utils/Comm.h"
#include <mpi.h>
#include <iostream>
#include <limits>
#include <vector>
#include "utils/Log.h"
#include "utils/System.h"

// init
const int Comm::CLIENT_RANK = 2;
bool Comm::_envInited = false;
int Comm::_mpiRank = 0;
int Comm::_mpiSize = 0;

void Comm::finalize() {
    if (_envInited) {
        MPI_Finalize();
        _envInited = false;
    }
}

void Comm::init(int argc, char **argv) {
    if (!_envInited) {
        // init MPI env
        MPI_Init(&argc, &argv);
        // process _mpiRank and sum
        MPI_Comm_rank(MPI_COMM_WORLD, &_mpiRank);
        MPI_Comm_size(MPI_COMM_WORLD, &_mpiSize);
        if (_mpiSize != 3) {
            throw std::runtime_error("3 parties restricted.");
        }
        _envInited = true;
    }
}

void Comm::sexch(const int64_t *source, int64_t *target) {
    ssend(source);
    srecv(target);
}

void Comm::ssend(const int64_t *source) {
    send(source, 1 - _mpiRank);
}

void Comm::ssend(const std::string *source) {
    send(source, 1 - _mpiRank);
}

void Comm::srecv(int64_t *target) {
    recv(target, 1 - _mpiRank);
}

void Comm::srecv(std::string *target) {
    recv(target, 1 - _mpiRank);
}

bool Comm::inited() {
    return _envInited;
}

int Comm::size() {
    return _mpiSize;
}

int Comm::rank() {
    return _mpiRank;
}

void Comm::send(const int64_t *source, int receiverRank) {
    MPI_Send(source, 1, MPI_INT64_T, receiverRank, 0, MPI_COMM_WORLD);
}

void Comm::send(const std::string *source, int receiverRank) {
    if (source->length() > static_cast<size_t>(std::numeric_limits<int>::max())) {
        std::cerr << "String size exceeds MPI_Send limit." << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    MPI_Send(source->data(), static_cast<int>(source->length()), MPI_CHAR, receiverRank, 0, MPI_COMM_WORLD);
}

void Comm::recv(int64_t *target, int senderRank) {
    MPI_Recv(target, 1, MPI_INT64_T, senderRank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void Comm::recv(std::string *target, int senderRank) {
    MPI_Status status;
    MPI_Probe(senderRank, 0, MPI_COMM_WORLD, &status);

    int count;
    MPI_Get_count(&status, MPI_CHAR, &count);

    std::vector<char> buffer(count);
    MPI_Recv( buffer.data(), count, MPI_CHAR, senderRank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    *target = std::string(buffer.data(), count);
}

bool Comm::isServer() {
    return _mpiRank != CLIENT_RANK;
}

bool Comm::isClient() {
    return !isServer();
}

void Comm::send(const bool *source, int receiverRank) {
    MPI_Send(source, 1, MPI_CXX_BOOL, receiverRank, 0, MPI_COMM_WORLD);
}

void Comm::recv(bool *target, int senderRank) {
    MPI_Recv(target, 1, MPI_CXX_BOOL, senderRank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void Comm::ssend(const bool *source) {
    send(source, 1 - _mpiRank);
}

void Comm::srecv(bool *target) {
    recv(target, 1 - _mpiRank);
}

void Comm::sexch(const bool *source, bool *target) {
    ssend(source);
    srecv(target);
}