/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *              GNU Lesser General Public Licence (LGPL) version 3,             *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -------------------------------------------------------------------------
// -----                    R3BNeulandField source file                  -----
// -----                Created 30/01/07  by M. Al/Turany              -----
// -------------------------------------------------------------------------
#include "R3BNeulandField.h"

#include "FairConstPar.h"    // for FairConstPar
#include "FairLogger.h"      // for logging
#include "FairRun.h"         // for FairRun
#include "FairRuntimeDb.h"   // for FairRuntimeDb

#include <TString.h>   // for operator<<, TString
#include <iomanip>     // for operator<<, setw
#include "R3BFieldPar.h"

using std::setprecision;
using std::setw;

R3BNeulandField::R3BNeulandField()
    : fXmin(0)
    , fXmax(0)
    , fYmin(0)
    , fYmax(0)
    , fZmin(0)
    , fZmax(0)
    , fBx(0)
    , fBy(0)
    , fBz(0)
{
    fType = 0;
}

R3BNeulandField::R3BNeulandField(const char* name,
                               Double_t xMin,
                               Double_t xMax,
                               Double_t yMin,
                               Double_t yMax,
                               Double_t zMin,
                               Double_t zMax,
                               Double_t bX,
                               Double_t bY,
                               Double_t bZ)
    : FairField(name)
    , fXmin(xMin)
    , fXmax(xMax)
    , fYmin(yMin)
    , fYmax(yMax)
    , fZmin(zMin)
    , fZmax(zMax)
    , fBx(bX)
    , fBy(bY)
    , fBz(bZ)

{
    fType = 0;
}

R3BNeulandField::R3BNeulandField(FairConstPar* fieldPar)
    : FairField()
    , fXmin(0)
    , fXmax(0)
    , fYmin(0)
    , fYmax(0)
    , fZmin(0)
    , fZmax(0)
    , fBx(0)
    , fBy(0)
    , fBz(0)
{
    if (!fieldPar) {
        LOG(warn) << "empty parameter container!";
        fType = -1;
    } else {
        fXmin = fieldPar->GetXmin();
        fXmax = fieldPar->GetXmax();
        fYmin = fieldPar->GetYmin();
        fYmax = fieldPar->GetYmax();
        fZmin = fieldPar->GetZmin();
        fZmax = fieldPar->GetZmax();
        fBx = fieldPar->GetBx();
        fBy = fieldPar->GetBy();
        fBz = fieldPar->GetBz();
        fType = fieldPar->GetType();
    }
}

R3BNeulandField::~R3BNeulandField() {}

void R3BNeulandField::SetFieldRegion(Double_t xMin,
                                    Double_t xMax,
                                    Double_t yMin,
                                    Double_t yMax,
                                    Double_t zMin,
                                    Double_t zMax)
{
    fXmin = xMin;
    fXmax = xMax;
    fYmin = yMin;
    fYmax = yMax;
    fZmin = zMin;
    fZmax = zMax;
}

void R3BNeulandField::SetField(Double_t bX, Double_t bY, Double_t bZ)
{
    fBx = bX;
    fBy = bY;
    fBz = bZ;
}

Double_t R3BNeulandField::GetBx(Double_t x, Double_t y, Double_t z)
{
    if (x < fXmin || x > fXmax || y < fYmin || y > fYmax || z < fZmin || z > fZmax) {
        return 0.;
    }
    return fBx;
}

Double_t R3BNeulandField::GetBy(Double_t x, Double_t y, Double_t z)
{
    if (x < fXmin || x > fXmax || y < fYmin || y > fYmax || z < fZmin || z > fZmax) {
        return 0.;
    }
    return fBy;
}

Double_t R3BNeulandField::GetBz(Double_t x, Double_t y, Double_t z)
{
    if (x < fXmin || x > fXmax || y < fYmin || y > fYmax || z < fZmin || z > fZmax) {
        return 0.;
    }
    return fBz;
}

void R3BNeulandField::Print(Option_t*) const
{
    LOG(info) << "======================================================";
    LOG(info) << "----  " << fTitle << " : " << fName;
    LOG(info) << "----";
    LOG(info) << "----  Field type    : constant";
    LOG(info) << "----";
    LOG(info) << "----  Field regions : ";
    LOG(info) << "----        x = " << setw(4) << fXmin << " to " << setw(4) << fXmax << " cm";
    LOG(info) << "----        y = " << setw(4) << fYmin << " to " << setw(4) << fYmax << " cm";
    LOG(info) << "----        z = " << setw(4) << fZmin << " to " << setw(4) << fZmax << " cm";
    LOG(info) << "----  B = ( " << setprecision(4) << fBx << ", " << fBy << ", " << fBz << " ) kG";
    LOG(info) << "======================================================";
}

void R3BNeulandField::FillParContainer()
{
    TString MapName = GetName();
    //  LOG(info) << "R3BNeulandField::FillParContainer() ";
    FairRun* fRun = FairRun::Instance();
    FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
    // Bool_t kParameterMerged = kTRUE;
    R3BFieldPar* Par = static_cast<R3BFieldPar*>(rtdb->getContainer("R3BFieldPar"));
    Par->SetParameters(this);
    Par->setInputVersion(fRun->GetRunId(), 1);
    Par->setChanged();
}

