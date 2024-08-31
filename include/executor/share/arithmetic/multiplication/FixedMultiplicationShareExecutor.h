//
// Created by 杜建璋 on 2024/7/27.
//

#ifndef DEMO_FIXEDMULTIPLICATIONSHAREEXECUTOR_H
#define DEMO_FIXEDMULTIPLICATIONSHAREEXECUTOR_H
#include "AbstractMultiplicationShareExecutor.h"

class FixedMultiplicationShareExecutor : public AbstractMultiplicationShareExecutor {
public:
    FixedMultiplicationShareExecutor(int64_t x, int l) : AbstractMultiplicationShareExecutor(x, l) {}

protected:
    [[nodiscard]] std::string tag() const override;
    void obtainMultiplicationTriple() override;
};


#endif //DEMO_FIXEDMULTIPLICATIONSHAREEXECUTOR_H