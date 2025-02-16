/*
 *  Copyright (c) 2016, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements common MeshCoP timestamp processing.
 */

#include "timestamp.hpp"

#include "common/code_utils.hpp"
#include "common/num_utils.hpp"

namespace ot {
namespace MeshCoP {

void Timestamp::ConvertTo(otTimestamp &aTimestamp) const
{
    aTimestamp.mSeconds       = GetSeconds();
    aTimestamp.mTicks         = GetTicks();
    aTimestamp.mAuthoritative = GetAuthoritative();
}

void Timestamp::SetFromTimestamp(const otTimestamp &aTimestamp)
{
    SetSeconds(aTimestamp.mSeconds);
    SetTicks(aTimestamp.mTicks);
    SetAuthoritative(aTimestamp.mAuthoritative);
}

int Timestamp::Compare(const Timestamp *aFirst, const Timestamp *aSecond)
{
    int rval;

    if (aFirst == nullptr)
    {
        // When `aFirst` is null but `aSecond is not, we return -1,
        // (indicate `aFirst (null) < aSecond (non-null)`).
        ExitNow(rval = (aSecond == nullptr) ? 0 : -1);
    }

    if (aSecond == nullptr)
    {
        // When `aFirst` is not null, but `aSecond` is, we return +1,
        // (indicate `aFirst (non-null) > aSecond (null)`).
        ExitNow(rval = 1);
    }

    // Both are non-null.

    rval = Compare(*aFirst, *aSecond);

exit:
    return rval;
}

int Timestamp::Compare(const Timestamp &aFirst, const Timestamp &aSecond)
{
    int rval;

    rval = ThreeWayCompare(aFirst.GetSeconds(), aSecond.GetSeconds());
    VerifyOrExit(rval == 0);

    rval = ThreeWayCompare(aFirst.GetTicks(), aSecond.GetTicks());
    VerifyOrExit(rval == 0);

    rval = ThreeWayCompare(aFirst.GetAuthoritative(), aSecond.GetAuthoritative());

exit:
    return rval;
}

void Timestamp::AdvanceRandomTicks(void)
{
    uint16_t ticks = GetTicks();

    ticks += Random::NonCrypto::GetUint32InRange(1, kMaxTicks + 1);

    if (ticks > kMaxTicks)
    {
        ticks -= (kMaxTicks + 1);
        SetSeconds(GetSeconds() + 1);
    }

    SetTicks(ticks);
}

} // namespace MeshCoP
} // namespace ot
