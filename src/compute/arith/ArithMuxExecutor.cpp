//
// Created by 杜建璋 on 2024/10/23.
//

#include "compute/arith/ArithMuxExecutor.h"

#include "compute/arith/ArithMulExecutor.h"
#include "comm/IComm.h"
#include "compute/BoolExecutor.h"
#include "compute/bool/BoolToArithExecutor.h"

ArithMuxExecutor::ArithMuxExecutor(int64_t x, int64_t y, bool c, int l, int32_t objTag, int8_t msgTagOffset,
                                   int clientRank) : ArithExecutor(x, y, l, objTag, msgTagOffset, clientRank) {
    _cond_i = ArithExecutor(c, 1, _objTag, _currentMsgTag++, clientRank)._zi;
}

ArithMuxExecutor *ArithMuxExecutor::execute() {
    if (IComm::impl->isServer()) {
        BoolToArithExecutor e(_cond_i, _l, _objTag, _currentMsgTag++, -1);
        int64_t a_cond_i = _l == 1 ? _cond_i : e.execute()->_zi;

        auto mul0 = ArithMulExecutor(a_cond_i, _xi, _l, _objTag, );
        int64_t cx = mul0.execute()->_zi;
        auto mul0 = ArithMulExecutor(a_cond_i, _yi, _l, true);
        int64_t cy = mul_executor.execute()->_zi;
        _zi = ring(cx + _yi - cy);
    }
    return this;
}

std::string ArithMuxExecutor::className() const {
    return "[MuxArithExecutor]";
}

ArithMuxExecutor * ArithMuxExecutor::setBmts(Bmt bmt0, Bmt bmt1) {
    _bmt0 = bmt0;
    _bmt1 = bmt1;
    return this;
}
