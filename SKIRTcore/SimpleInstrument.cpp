/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "FatalError.hpp"
#include "PeelOffPhotonPackage.hpp"
#include "SimpleInstrument.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

SimpleInstrument::SimpleInstrument()
{
}

////////////////////////////////////////////////////////////////////

void SimpleInstrument::setupSelfBefore()
{
    SingleFrameInstrument::setupSelfBefore();

    int Nlambda = find<WavelengthGrid>()->Nlambda();
    _ftotv.resize(Nlambda*_Nxp*_Nyp);
    _Ftotv.resize(Nlambda);
}

////////////////////////////////////////////////////////////////////

void
SimpleInstrument::detect(const PeelOffPhotonPackage* pp)
{
    int l = pixelondetector(pp);
    int ell = pp->ell();
    int m = l + ell*_Nxp*_Nyp;
    double L = pp->luminosity();
    double taupath = pp->opticaldepth();
    double extf = exp(-taupath);
    double Lextf = L*extf;

    record(&_Ftotv[ell], Lextf);
    if (l>=0) record(&_ftotv[m], Lextf);
}

////////////////////////////////////////////////////////////////////

void
SimpleInstrument::write()
{
    // lists of f-array and F-array pointers, and the corresponding file and column names
    QList< Array* > farrays, Farrays;
    QStringList fnames, Fnames;
    farrays << &_ftotv;
    Farrays << &_Ftotv;
    fnames << "total";
    Fnames << "total flux";

    // calibrate and output the arrays
    calibrateAndWriteDataCubes(farrays, fnames);
    calibrateAndWriteSEDs(Farrays, Fnames);
}

////////////////////////////////////////////////////////////////////