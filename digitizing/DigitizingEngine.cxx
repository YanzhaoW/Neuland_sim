/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "DigitizingEngine.h"
#include "FairLogger.h"
#include "R3BNeulandHitCalibrationBar.h"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace Neuland
{
    namespace Digitizing
    {
        template <Int_t iterations = 8>
        const Float_t FastExp(const Float_t val)
        {
            auto exp = 1. + val / (1 << iterations);
            for (auto i = 0; i < iterations; ++i)
            {
                exp *= exp;
            }
            return exp;
        }

        // Calculate the time of arrival and the amount of light that arrives at
        // the PMT based on the deposition in the paddle
        PMTHit::PMTHit(const Double_t mcTime, const Double_t mcLight, const Double_t dist)
            : fMcTime(mcTime)
            , fMcLight(mcLight)
            , fDist(dist)
        {
            time = fMcTime + (Paddle::gHalfLength + fDist) / Paddle::gCMedium;
            light = fMcLight * exp(-Paddle::gAttenuation * (Paddle::gHalfLength + fDist));
        }

        Channel::Channel(SideOfChannel side)
            : fSide(side)
        {
        }

        const std::vector<Channel::Signal>& Channel::GetSignals() const
        {
            if (!fSignals.valid())
                ConstructSignals();
            return fSignals.getRef();
        }

        Paddle::Paddle(const Int_t paddleID,
                       std::unique_ptr<Channel> l,
                       std::unique_ptr<Channel> r,
                       R3BNeulandHitPar* par)
            : fLeftChannel(std::move(l))
            , fRightChannel(std::move(r))
            , fPaddleId(paddleID)
        {
            if (fLeftChannel->GetSide() != Channel::left && fRightChannel->GetSide() != Channel::right)
                LOG(fatal) << "DigitizingEngine::Paddle ctor: side of channels is not correct!";
            SetHitModulePar(par);
            fLeftChannel->SetPaddle(this);
            fRightChannel->SetPaddle(this);
        }

        void Paddle::DepositLight(const Double_t time, const Double_t light, const Double_t dist)
        {
            fLeftChannel->AddHit(time, light, -1. * dist);
            fRightChannel->AddHit(time, light, dist);
        }

        bool Paddle::HasFired() const
        {
            if (fLeftChannel.get() && fRightChannel.get())
                return (fLeftChannel->HasFired() && fRightChannel->HasFired());
            else
            {
                LOG(FATAL) << "channels failed to be constructed when checking fire! ";
                return false;
            }
        }

        bool Paddle::HasHalfFired() const
        {
            return (fLeftChannel->HasFired() && !fRightChannel->HasFired()) ||
                   (!fLeftChannel->HasFired() && fRightChannel->HasFired());
        }

        inline Double_t Paddle::ComputeEnergy(const Channel::Signal& firstSignal,
                                              const Channel::Signal& secondSignal) const
        {
            return std::sqrt(firstSignal.energy * secondSignal.energy);
        }

        inline Double_t Paddle::ComputeTime(const Channel::Signal& firstSignal,
                                            const Channel::Signal& secondSignal) const
        {
            return (firstSignal.tdc + secondSignal.tdc) / 2. - gHalfLength / gCMedium;
        }

        inline Double_t Paddle::ComputePosition(const Channel::Signal& leftSignal,
                                                const Channel::Signal& rightSignal) const
        {
            assert((leftSignal.side == Channel::left) && (rightSignal.side == Channel::right));
            return (rightSignal.tdc - leftSignal.tdc) / 2. * gCMedium;
        }

        Float_t Paddle::CompareSignals(const Channel::Signal& firstSignal, const Channel::Signal& secondSignal) const
        {
            auto leftE = static_cast<Float_t>(firstSignal.energy);
            auto rightE = static_cast<Float_t>(secondSignal.energy);
            auto leftT = firstSignal.tdc;
            auto rightT = secondSignal.tdc;
            if (leftT > rightT)
                return std::abs((leftE / rightE) *
                                    FastExp<4>(static_cast<Float_t>(gAttenuation * gCMedium * (leftT - rightT))) -
                                1);
            else
                return std::abs((rightE / leftE) *
                                    FastExp<4>(static_cast<Float_t>(gAttenuation * gCMedium *
                                                                    static_cast<Float_t>(rightT - leftT))) -
                                1);
        }

        std::vector<Paddle::Pair<int>> Paddle::ConstructIndexMap(const ChannelSignals& leftSignals,
                                                                 const ChannelSignals& rightSignals) const
        {
            if (leftSignals.size() == 0 || rightSignals.size() == 0)
                LOG(fatal) << "DigitizingEngine.cxx::GetIndexMap(): Can't get index map from the signals with size 0.";

            assert((leftSignals[0].side == Channel::left) && (rightSignals[0].side == Channel::right));

            auto indexMap = std::vector<Pair<int>>(leftSignals.size(), { -1, -1 });
            auto valueMap = std::vector<Double_t>(leftSignals.size(), 0);

            for (auto i = 0; i < indexMap.size(); i++)
            {
                std::vector<Double_t> compareValues(rightSignals.size());
                for (auto j = 0; j < rightSignals.size(); j++)
                {
                    compareValues[j] = CompareSignals(leftSignals[i], rightSignals[j]);
                }
                auto compareValues_it = std::min_element(
                    compareValues.begin(), compareValues.end(), [](const Double_t& left, const Double_t& right) {
                        return left < right;
                    });

                if (compareValues_it == compareValues.end())
                    LOG(fatal) << "DigitizingEngine.cxx::GetIndexMap(): failed to find minimum value!";

                valueMap[i] = *compareValues_it;
                auto indexMatch = static_cast<int>(compareValues_it - compareValues.begin());

                auto findResult = std::find_if(indexMap.begin(), indexMap.end(), [indexMatch](const Pair<int>& pair) {
                    return pair.right == indexMatch;
                });
                if (findResult == indexMap.end())
                {
                    indexMap[i] = { i, indexMatch };
                }
                else
                {
                    if (*compareValues_it > valueMap[findResult->left])
                        indexMap[i] = { i, -1 };
                    else
                    {
                        indexMap[findResult->left].right = -1;
                        indexMap[i] = { i, indexMatch };
                    }
                }
            }
            return indexMap;
        }

        std::vector<Paddle::Signal> Paddle::ConstructPannelSignals(const ChannelSignals& leftSignals,
                                                                   const ChannelSignals& rightSignals,
                                                                   const std::vector<Pair<int>>& indexMapping) const
        {
            if (leftSignals.size() == 0 || rightSignals.size() == 0)
                LOG(fatal) << "DigitizingEngine.cxx::ConstructPannelSignals(): Can't construct from the channel "
                              "signals with size 0.";

            assert((leftSignals[0].side == Channel::left) && (rightSignals[0].side == Channel::right));

            auto pannelSignals = std::vector<Signal>(0);
            pannelSignals.reserve(indexMapping.size());

            for (const auto it : indexMapping)
            {
                if (it.right < 0)
                    continue;

                assert((it.left < leftSignals.size()) && (it.right < rightSignals.size()));

                auto pannelSignal = Signal{};

                pannelSignal.energy = ComputeEnergy(leftSignals[it.left], rightSignals[it.right]);
                pannelSignal.channelE.left = leftSignals[it.left].energy;
                pannelSignal.channelE.right = rightSignals[it.right].energy;

                pannelSignal.channelQdc.left = leftSignals[it.left].qdc;
                pannelSignal.channelQdc.right = rightSignals[it.right].qdc;

                pannelSignal.time = ComputeTime(leftSignals[it.left], rightSignals[it.right]);
                pannelSignal.channelTdc.left = leftSignals[it.left].tdc;
                pannelSignal.channelTdc.right = rightSignals[it.right].tdc;

                pannelSignal.position = ComputePosition(leftSignals[it.left], rightSignals[it.right]);

                pannelSignals.push_back(std::move(pannelSignal));
            }
            return pannelSignals;
        }

        void Paddle::SetHitModulePar(R3BNeulandHitPar* par)
        {
            if (fPaddleId < 0)
                LOG(FATAL) << "NeulandPaddle: parameters cannot be initilizied without setting paddle ID first!";

            if (par)
            {
                auto PaddleId_max = par->GetNumModulePar();
                if (fPaddleId > PaddleId_max)
                {
                    LOG(FATAL) << "Paddle id exceeds the id in the parameter file!";
                }
                else
                {
                    fHitModulePar = par->GetModuleParAt(fPaddleId - 1);
                    if (!fHitModulePar)
                        LOG(WARN) << "hitModulePar failed to be read!";
                    if (fPaddleId != fHitModulePar->GetModuleId())
                        LOG(ERROR) << "Wrong paddleID for the parameters!";
                }
            }
        }

        const std::vector<Paddle::Signal>& Paddle::GetSignals() const
        {
            if (!fSignals.valid())
            {
                if (HasFired())
                {
                    auto indexmap = std::vector<Pair<int>>{ { 0, 0 } };

                    if (fLeftChannel->GetSignals().size() != 1 || fRightChannel->GetSignals().size() != 1)
                        indexmap = ConstructIndexMap(fLeftChannel->GetSignals(), fRightChannel->GetSignals());

                    // // printing info:
                    // if (std::max(fLeftChannel->GetSignals().size(), fRightChannel->GetSignals().size()) > 1)
                    // {
                    //     LOG(info) << "printing signals >>>>>>>>>: ";
                    //     LOG(info) << "==========Right=========";
                    //     for (const auto& it : fLeftChannel->GetSignals())
                    //     {
                    //         LOG(info) << "----------------------";
                    //         LOG(info) << "from left Channel energy: " << it.energy;
                    //         LOG(info) << "from left Channel qdc: " << it.qdc;
                    //         LOG(info) << "from left Channel tdc: " << it.tdc;
                    //     }

                    //     LOG(info) << "==========Left=========";
                    //     for (const auto& it : fRightChannel->GetSignals())
                    //     {
                    //         LOG(info) << "----------------------";
                    //         LOG(info) << "from right Channel energy: " << it.energy;
                    //         LOG(info) << "from right Channel qdc: " << it.qdc;
                    //         LOG(info) << "from right Channel tdc: " << it.tdc;
                    //     }
                    //     LOG(info) << "=========index==========";
                    //     for (const auto& it : indexmap)
                    //     {
                    //         LOG(info) << "----------------------";
                    //         LOG(info) << "left index: " << it.left << " ----> right index " << it.right;
                    //     }
                    //     LOG(info) << "";
                    // }
                    // printing ends

                    auto signals = ConstructPannelSignals(
                        fLeftChannel->GetSignals(), fRightChannel->GetSignals(), std::move(indexmap));
                    fSignals.set(std::move(signals));
                }
                else
                    fSignals.set({});
            }
            return fSignals.getRef();
        }

    } // namespace Digitizing

    void DigitizingEngine::DepositLight(const Int_t paddle_id,
                                        const Double_t time,
                                        const Double_t light,
                                        const Double_t dist)
    {
        if (paddles.find(paddle_id) == paddles.end())
        {
            paddles[paddle_id] = std::unique_ptr<Digitizing::Paddle>(
                new Digitizing::Paddle(paddle_id,
                                       this->BuildChannel(Digitizing::Channel::left),
                                       this->BuildChannel(Digitizing::Channel::right),
                                       fNeulandHitPar));
        }
        paddles.at(paddle_id)->DepositLight(time, light, dist);
    }

    Double_t DigitizingEngine::GetTriggerTime() const
    {
        Double_t triggerTime = 1e100;
        for (const auto& kv : paddles)
        {
            const auto& paddle = kv.second;

            // TODO: Should be easier with std::min?
            if (paddle->GetLeftChannel()->HasFired() && paddle->GetLeftChannel()->GetTrigTime() < triggerTime)
            {
                triggerTime = paddle->GetLeftChannel()->GetTrigTime();
            }
            if (paddle->GetRightChannel()->HasFired() && paddle->GetRightChannel()->GetTrigTime() < triggerTime)
            {
                triggerTime = paddle->GetRightChannel()->GetTrigTime();
            }
        }
        return triggerTime;
    }

    std::map<Int_t, std::unique_ptr<Digitizing::Paddle>> DigitizingEngine::ExtractPaddles()
    {
        return std::move(paddles);
    }

} // namespace Neuland
