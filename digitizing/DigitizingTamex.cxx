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

#include "DigitizingTamex.h"

#include <algorithm>
#include <iostream>

#include "FairLogger.h"

namespace Neuland
{

    Tamex::Params::Params(TRandom3* rnd)
        : fPMTThresh(1.)                // [MeV]
        , fSaturationCoefficient(0.012) //
        , fExperimentalDataIsCorrectedForSaturation(true)
        , fTimeRes(0.15) // time + Gaus(0., fTimeRes) [ns]
        , fEResRel(0.05) // Gaus(e, fEResRel * e) []
        , fEnergyGain(15.0)
        , fPedestal(14.0)
        , fTimeMax(1000)
        , fTimeMin(1)
        , fQdcMin(0.67)
        , fRnd(rnd)
    {
    }

    Tamex::TmxPeak::TmxPeak()
        : TmxPeak(Digitizing::PMTHit{}, nullptr)
    {
    }

    Tamex::TmxPeak::TmxPeak(const Digitizing::PMTHit& hit, Channel* channel)
        : fChannel{ channel }
    {
        if (!fChannel)
            LOG(FATAL) << "no channel is linked to the signal peak!";
        auto side = fChannel->GetSide();
        auto par = fChannel->GetPar();

        auto light = hit.light;
        // apply saturation coefficent
        if (par.fExperimentalDataIsCorrectedForSaturation)
            fQdc = light / (1. + par.fSaturationCoefficient * light);
        else
            fQdc = light;

        // calculate the time and the width of the signal
        fTime = hit.time;
        fWidth = QdcToWidth(fQdc);
        CheckFire(fQdc);
    }

    Tamex::TmxPeak::operator Digitizing::Channel::Signal() const
    {
        if(!cachedSignal.valid())
            cachedSignal.set(fChannel->TmxPeakToSignal(*this));

        return cachedSignal.get();
    }

    bool Tamex::TmxPeak::operator==(const TmxPeak& sig) const
    {
        if (sig.fTime == 0 && fTime == 0)
            LOG(WARN) << "the times of both PMT signals are 0!";
        return !((fTime > sig.fTime + sig.fWidth) || (sig.fTime > fTime + fWidth));
    }

    void Tamex::TmxPeak::operator+=(TmxPeak& sig)
    {
        cachedSignal.invalidate();
        auto qdc_prev = fQdc;

        fTime = (fTime > sig.fTime) ? fTime : sig.fTime;
        fQdc += sig.fQdc;
        CheckFire(fQdc);

        // change the width of peak to make sure its correlation to qdc stays the same
        if (!fChannel)
            LOG(FATAL) << "no channel is linked to the signal peak!";
        auto par = fChannel->GetPar();
        if (par.fEnergyGain != 0)
            fWidth = QdcToWidth(fQdc);
    }

    Double_t Tamex::TmxPeak::QdcToWidth(Double_t qdc) const
    {
        Double_t width;
        auto par = fChannel->GetPar();
        if (qdc > par.fQdcMin)
            width = qdc * par.fEnergyGain + par.fPedestal;
        else
            width = qdc * par.fEnergyGain * (par.fPedestal + 1);
        return std::move(width);
    }

    bool Tamex::TmxPeak::CheckFire(Double_t qdc)
    {
        if (!fChannel)
            LOG(FATAL) << "no channel is linked to the signal peak!";
        auto par = fChannel->GetPar();
        if (qdc > par.fPMTThresh)
        {
            fChannel->Fire();
            return true;
        }
        else
            return false;
    }

    Tamex::Channel::Channel(TRandom3* rnd, const SideOfChannel side)
        : Digitizing::Channel{ side }
        , par{ rnd }
    {
    }

    Int_t Tamex::Channel::CheckOverlapping(TmxPeak& peak) const
    {
        auto it = std::find_if(fTmxPeaks.begin(), fTmxPeaks.end(), [&](const TmxPeak& s) { return (s == peak); });
        if (it == fTmxPeaks.end())
        {
            return -1;
        }
        else
        {
            return static_cast<Int_t>(it - fTmxPeaks.begin());
        }
    }

    Int_t Tamex::Channel::RecheckOverlapping(Int_t index)
    {
        auto it = fTmxPeaks.begin();
        if (index >= fTmxPeaks.size())
            LOG(FATAL) << "DigitizingTamex::RecheckOverlapping: cannot check the peak with overflowing index!";
        while (it != fTmxPeaks.end())
        {
            Int_t i = 0;
            it = std::find_if(fTmxPeaks.begin(),
                              fTmxPeaks.end(),
                              [&](const TmxPeak& p)
                              {
                                  bool res = false;
                                  if (index != i)
                                  {
                                      res = (p == fTmxPeaks[index]);
                                  }
                                  i++;
                                  return res;
                              });

            if (it == fTmxPeaks.end())
                continue;

            i = static_cast<int>(it - fTmxPeaks.begin());
            LOG(DEBUG1) << "reoverlapping occurs:>>>>>>>>>>>>>>>";
            LOG(DEBUG1) << "index of influenced peak: " << index;
            LOG(DEBUG1) << "index of removed peak: " << i;
            LOG(DEBUG1) << "time of influenced peak: " << fTmxPeaks[index].GetTime();
            LOG(DEBUG1) << "time of removed peak: " << it->GetTime();
            LOG(DEBUG1) << "<<<<<<<<<<<<<<<<<<<<<<";

            if (index == fTmxPeaks.size() - 1)
            {
                std::swap(index, i);
            }
            fTmxPeaks[index] += fTmxPeaks[i];
            RemovePeakAt(i);
        }
        return index;
    }

    void Tamex::Channel::AddHit(Double_t mcTime, Double_t mcLight, Double_t dist)
    {
        auto newHit = Digitizing::PMTHit{ mcTime, mcLight, dist };
        if (newHit.time < par.fTimeMin || newHit.time > par.fTimeMax)
            return;

        fSignals.invalidate();
        auto peak = TmxPeak{ std::move(newHit), this };
        fNonZeroPeak = false;

        LOG(DEBUG1) << "------new peak: ------" << fSide << "\tpaddleID:" << fPaddle->GetPaddleId();
        LOG(DEBUG1) << "time: " << peak.GetTime();
        LOG(DEBUG1) << "width: " << peak.GetWidth();
        LOG(DEBUG1) << "qdc: " << peak.GetQDC();
        auto index = CheckOverlapping(peak);

        if (index < 0)
        {
            LOG(DEBUG1) << "new peak added;" << fSide;
            if (fTmxPeaks.size() != 0)
            {
                LOG(DEBUG1) << "************double hits: **************";
                for (auto const& it : fTmxPeaks)
                {
                    LOG(DEBUG1) << "------peak: ------";
                    LOG(DEBUG1) << "time: " << it.GetTime();
                    LOG(DEBUG1) << "width: " << it.GetWidth();
                    LOG(DEBUG1) << "qdc: " << it.GetQDC();
                }
            }
            fTmxPeaks.push_back(std::move(peak));
        }
        else
        {
            LOG(DEBUG1) << ">>>>>>>>overlapping occurs: >>>>>>>>>" << fSide << "\tpaddleID:" << fPaddle->GetPaddleId();
            LOG(DEBUG1) << "Peak index: " << index;
            LOG(DEBUG1) << "time: " << fTmxPeaks[index].GetTime();
            LOG(DEBUG1) << "width: " << fTmxPeaks[index].GetWidth();
            LOG(DEBUG1) << "qdc: " << fTmxPeaks[index].GetQDC();
            LOG(DEBUG1) << "new peak: ";
            LOG(DEBUG1) << "time: " << peak.GetTime();
            LOG(DEBUG1) << "width: " << peak.GetWidth();
            LOG(DEBUG1) << "qdc: " << peak.GetQDC();
            fTmxPeaks[index] += peak;
            index = RecheckOverlapping(index);
            LOG(DEBUG1) << "changed time: " << fTmxPeaks[index].GetTime();
            LOG(DEBUG1) << "changed width: " << fTmxPeaks[index].GetWidth();
            LOG(DEBUG1) << "changed qdc: " << fTmxPeaks[index].GetQDC();
            LOG(DEBUG1) << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<";
        }
        LOG(DEBUG1) << "***************************************";
    }

    void Tamex::Channel::RemovePeakAt(Int_t i) const
    {
        if (i >= fTmxPeaks.size())
        {
            LOG(FATAL) << "DigitizingTamex::RemovePeakAt: Cannot remove the peak with the overflowing index! ";
            return;
        }
        LOG(DEBUG1) << "Removing peak from side " << fSide << " Paddle " << fPaddle->GetPaddleId()
                    << ": >>>>>>>>>>>>>>>>>>";
        LOG(DEBUG1) << "qdc: " << fTmxPeaks[i].GetQDC();
        LOG(DEBUG1) << "tdc: " << fTmxPeaks[i].GetTime();
        LOG(DEBUG1) << "width: " << fTmxPeaks[i].GetWidth();
        LOG(DEBUG1) << "<<<<<<<<<<<<<<<<<<<<<<<<";
        if (i != fTmxPeaks.size() - 1)
        {
            fTmxPeaks[i] = std::move(fTmxPeaks.back());
        }
        fTmxPeaks.pop_back();
    }

    Digitizing::Channel::Signal Tamex::Channel::TmxPeakToSignal(const TmxPeak& peak) const
    {
        auto peakQdc = peak.GetQDC();
        auto peakTime = peak.GetTime();
        auto qdc = ToQdc(peakQdc);

        return {qdc, ToTdc(peakTime), ToEnergy(qdc)};
    }
    
    // void Tamex::Channel::RemoveZeroPeaks() const
    // {
    //     int i = 0;
    //     while (i < fTmxPeaks.size())
    //     {
    //         while (i < fTmxPeaks.size() && GetQDC(i) == 0)
    //             RemovePeakAt(i);
    //         i++;
    //     }
    //     fNonZeroPeak = true;
    // }

    // Int_t Tamex::Channel::GetNHits() const
    // {
    //     if (!fNonZeroPeak)
    //         RemoveZeroPeaks();
    //     return fTmxPeaks.size();
    // }

    void Tamex::Channel::ConstructSignals() const
    {
        auto signals = std::vector<Signal>{};
        signals.reserve(fTmxPeaks.size());

        std::transform(fTmxPeaks.begin(), fTmxPeaks.end(), std::back_inserter(signals), [](TmxPeak& peak){ return static_cast<Signal>(peak);} );
        fSignals.set(std::move(signals));
    }

    void Tamex::Channel::SetPaddle(Digitizing::Paddle* paddle)
    {
        fPaddle = paddle;
        auto hitModulePar = paddle->GetHitModulePar();
        if (hitModulePar)
        {
            par.fSaturationCoefficient = hitModulePar->GetPMTSaturation(fSide);
            par.fEnergyGain = hitModulePar->GetEnergyGain(fSide);
            par.fPedestal = hitModulePar->GetPedestal(fSide);
            par.fPMTThresh = hitModulePar->GetPMTThreshold(fSide);
            par.fQdcMin = 1 / par.fEnergyGain;
            // LOG(INFO) << "parameter info: >>>>>>>>>>>>>>>>" << fSide;
            // LOG(INFO) << "PMT saturation: " << hitModulePar->GetPMTSaturation(fSide);
            // LOG(INFO) << "Energy gain: " << hitModulePar->GetEnergyGain(fSide);
            // LOG(INFO) << "Pesdestal: " << hitModulePar->GetPedestal(fSide);
            // LOG(INFO) << "PMT threshold: " << hitModulePar->GetPMTThreshold(fSide);
            // LOG(INFO) << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<";
        }
    }

    // Double_t Tamex::Channel::GetQDC(UShort_t index) const
    // {
        // if (!HasFired())
        // {
        //     LOG(ERROR) << "Error: Cannot get QDC values from unfired NeuLAND paddle!";
        //     return 0;
        // }

        // if (index >= cachedQDC.size())
        // {
        //     LOG(ERROR) << "FATAL: ";
        //     LOG(ERROR) << "index: " << index;
        //     ErrorPrint();
        //     LOG(FATAL) << "Cannot get qdc with overflowing index!";
        //     return 0;
        // }

        // if (!cachedQDC[index].valid())
        // {
        //     // Get qdc from the signal peak
        //     auto qdc = fTmxPeaks[index].GetQDC();

        //     // apply energy smearing
        //     qdc = par.fRnd->Gaus(qdc, par.fEResRel * qdc);

        //     // set qdc to zero if below PMT threshold
        //     qdc = (qdc > par.fPMTThresh) ? qdc : 0.0;

        //     cachedQDC[index].set(qdc);
        // }
        // return cachedQDC[index].get();
    //     return 0.0;
    // }

    Double_t Tamex::Channel::ToQdc(Double_t qdc) const
    {
            // apply energy smearing
            qdc = par.fRnd->Gaus(qdc, par.fEResRel * qdc);

            // set qdc to zero if below PMT threshold
            qdc = (qdc > par.fPMTThresh) ? qdc : 0.0;
            return qdc;
    }

    Double_t Tamex::Channel::ToTdc(Double_t time) const
    {
            return time + par.fRnd->Gaus(0., par.fTimeRes);
    }

    Double_t Tamex::Channel::ToEnergy(Double_t e) const
    {
            // Apply reverse saturation
            if (par.fExperimentalDataIsCorrectedForSaturation)
            {
                e = e / (1. - par.fSaturationCoefficient * e);
            }
            // Apply reverse attenuation
            e = e * exp((2. * (Digitizing::Paddle::gHalfLength)) * Digitizing::Paddle::gAttenuation / 2.);
            return e;
    }

    // Double_t Tamex::Channel::GetTDC(UShort_t index) const
    // {
        // if (!HasFired())
        // {
        //     LOG(ERROR) << "Error: Cannot get TDC values from unfired NeuLAND paddle!";
        //     return 0;
        // }

        // if (index >= cachedTDC.size())
        // {
        //     LOG(FATAL) << "Cannot get tdc with overflowing index!";
        //     return 0;
        // }

        // if (!cachedTDC[index].valid())
        // {
        //     cachedTDC[index].set(fTmxPeaks[index].GetTime() + par.fRnd->Gaus(0., par.fTimeRes));
        // }

        // return cachedTDC[index].get();
    //     return 0.0;
    // }

    // Double_t Tamex::Channel::GetEnergy(UShort_t index) const
    // {
        // if (!HasFired())
        // {
        //     LOG(ERROR) << "Error: Cannot get energy values from unfired NeuLAND paddle!";
        //     return 0;
        // }

        // if (index >= cachedEnergy.size())
        // {
        //     LOG(ERROR) << "FATAL: ";
        //     LOG(ERROR) << "index: " << index;
        //     ErrorPrint();
        //     LOG(FATAL) << "Cannot get energy with overflowing index!";
        //     return 0;
        // }

        // if (!cachedEnergy[index].valid())
        // {
        //     Double_t e = GetQDC(index);

        //     // Apply reverse saturation
        //     if (par.fExperimentalDataIsCorrectedForSaturation)
        //     {
        //         e = e / (1. - par.fSaturationCoefficient * e);
        //     }
        //     // Apply reverse attenuation
        //     e = e * exp((2. * (Digitizing::Paddle::gHalfLength)) * Digitizing::Paddle::gAttenuation / 2.);
        //     cachedEnergy[index].set(e);
        // }
        // return cachedEnergy[index].get();
    //     return 0.0;
    // }

    // void Tamex::Channel::ErrorPrint() const
    // {
    //     if(fSide == leftside)
    //         LOG(ERROR) << "current channel is on left side";
    //     else
    //         LOG(ERROR) << "current channel is on right side";
    //     // LOG(ERROR) << "current size of qdc values : " << cachedQDC.size();
    //     // LOG(ERROR) << "current size of energy values: " << cachedEnergy.size();
    //     // LOG(ERROR) << "current size of tdc values: " << cachedTDC.size();

    //     LOG(ERROR) << "size of paddle signals: " << fPaddle->GetNHits();
    //     LOG(ERROR) << "size of right channel signals: " << fPaddle->GetRightChannel()->GetNHits();
    //     LOG(ERROR) << "size of left channel signals: " << fPaddle->GetLeftChannel()->GetNHits();

    //     auto indexmap = fPaddle->GetIndexMap();
    //     LOG(ERROR) << "index mapping for the paddle:";
    //     for(auto i = 0; i < indexmap.size(); i++)
    //     {
    //         LOG(ERROR) << "left channel " << i << " to right channel " << indexmap[i];
    //     }
    //         
    // }

    DigitizingTamex::DigitizingTamex()
        : fRnd(new TRandom3{})
    {
    }

    std::unique_ptr<Digitizing::Channel> DigitizingTamex::BuildChannel(Digitizing::Channel::SideOfChannel side)
    {
        return std::unique_ptr<Digitizing::Channel>(new Tamex::Channel(fRnd.get(), side));
    }

} // namespace Neuland
