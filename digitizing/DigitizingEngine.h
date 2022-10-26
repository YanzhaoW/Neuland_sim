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

#ifndef NEULAND_DIGITIZING_ENGINE_H
#define NEULAND_DIGITIZING_ENGINE_H

#include "FairLogger.h"
#include "R3BNeulandHitPar.h"
#include "Rtypes.h"
#include "Validated.h"
#include <map>
#include <memory>
#include <vector>

namespace Neuland
{
    namespace Digitizing
    {
        struct PMTHit
        {
            Double_t time;
            Double_t light;
            Double_t fMcTime;
            Double_t fMcLight;
            Double_t fDist;

            bool operator<(const PMTHit& rhs) const { return (time < rhs.time); }

            PMTHit() = default;
            PMTHit(Double_t mcTime, Double_t mcLight, Double_t dist);
        };

        class Paddle;
        class Channel
        {
          public:
            enum SideOfChannel
            {
                right = 1,
                left = 2
            };
            struct Signal 
            {
                Double_t qdc        = 0.0;
                Double_t tdc        = 0.0;
                Double_t energy     = 0.0;
                Signal() = default;
                Signal(Double_t q, Double_t t, Double_t e): qdc{ q }, tdc{t}, energy{e}{}
            };
            Channel(SideOfChannel);
            virtual ~Channel() = default; // FIXME: Root doesn't like pure virtual destructors (= 0;)
            virtual void AddHit(Double_t mcTime, Double_t mcLight, Double_t dist) = 0;
            virtual bool HasFired() const = 0;
            virtual void SetPaddle(Paddle* paddle) { fPaddle = paddle; };
            virtual void ConstructSignals() const = 0;

            const std::vector<Signal>& GetSignals() const;
            const SideOfChannel GetSide() const { return fSide; }

            // for backward compatibility
            virtual Double_t GetQDC() const { return 0.0; }
            virtual Double_t GetTDC() const { return 0.0; }
            virtual Double_t GetEnergy() const { return 0.0; }

          protected:
            const SideOfChannel fSide;
            mutable Validated<std::vector<Signal>> fSignals;
            Paddle* fPaddle; // pointer to the paddle who owns this channel
            std::vector<PMTHit> fPMTHits;
            // R3BNeulandHitModulePar* fNeulandHitModulePar = nullptr;
        };

        class Paddle
        {
          public:
              template<class T>
              struct Pair
              {
                  T left;
                  T right;
              };
              struct Values
              {
                  Double_t right = 0.0;
                  Double_t left = 0.0;
                  Double_t value = 0.0;
                  Values() = default;
                  Values(Double_t r, Double_t l): right(r), left(l){}
              };
              struct Signal
              {
                  Values energy{};
                  Values tdc{};
                  Values qdc{};
                  Double_t position;
                  Signal() = default;
                  Signal(Values e, Values t, Values q): energy(e), tdc(t), qdc(q){}
              };
            Paddle(const Int_t paddleID,
                   std::unique_ptr<Channel> l,
                   std::unique_ptr<Channel> r,
                   R3BNeulandHitPar* par = nullptr);
            void DepositLight(Double_t time, Double_t light, Double_t dist);

            bool HasFired() const;
            bool HasHalfFired() const;
            R3BNeulandHitModulePar* GetHitModulePar() const { return fHitModulePar; }
            Int_t GetPaddleId() const { return fPaddleId; }
            const std::vector<Signal>& GetSignals() const;

            const Channel* GetLeftChannel() const { return fLeftChannel.get(); }
            const Channel* GetRightChannel() const { return fRightChannel.get(); }
            void SetHitModulePar(R3BNeulandHitPar* par);

          private:
            std::vector<std::pair<int, int>> ConstructIndexMap(const std::vector<Channel::Signal>& leftSignals, const std::vector<Channel::Signal>& rightSignals) const;
            std::vector<Signal> ConstructPannelSignals(const std::vector<Channel::Signal>& leftSignals, const std::vector<Channel::Signal>& rightSignals, const std::vector<std::pair<int, int>>& indexMapping) const;
            Float_t CompareSignals(const Channel::Signal& firstSignal, const Channel::Signal& secondSignal) const;
            Double_t ComputeTime(const Channel::Signal& firstSignal, const Channel::Signal& secondSignal) const;
            Double_t ComputeEnergy(const Channel::Signal& firstSignal, const Channel::Signal& secondSignal) const;
            Double_t ComputePosition(const Channel::Signal& rightSignal, const Channel::Signal& leftSignal) const;
            mutable Validated<std::vector<Signal>> fSignals;

          protected:
            std::unique_ptr<Channel> fLeftChannel;
            std::unique_ptr<Channel> fRightChannel;
            R3BNeulandHitModulePar* fHitModulePar = nullptr;
            const Int_t fPaddleId;

          public:
            static constexpr Double_t gHalfLength = 135.;   // [cm]
            static constexpr Double_t gCMedium = 14.;       // speed of light in material in [cm/ns]
            static constexpr Double_t gAttenuation = 0.008; // light attenuation of plastic scintillator [1/cm]
            static constexpr Double_t gLambda = 1. / 2.1;
        };
    } // namespace Digitizing

    // abstract class, cannot be instantiated, to be used as a base class.
    class DigitizingEngine
    {
      public:
        virtual ~DigitizingEngine() = default; // FIXME: Root doesn't like pure virtual destructors (= 0;)
        virtual std::unique_ptr<Digitizing::Channel> BuildChannel(Digitizing::Channel::SideOfChannel) = 0;

        void SetHitPar(R3BNeulandHitPar* par) { fNeulandHitPar = par; }
        void DepositLight(Int_t paddle_id, Double_t time, Double_t light, Double_t dist);
        Double_t GetTriggerTime() const;
        std::map<Int_t, std::unique_ptr<Digitizing::Paddle>> ExtractPaddles();

      protected:
        std::map<Int_t, std::unique_ptr<Digitizing::Paddle>> paddles;
        R3BNeulandHitPar* fNeulandHitPar = nullptr;
    };
} // namespace Neuland

#endif // NEULAND_DIGITIZING_ENGINE_H
