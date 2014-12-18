/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "TrustGraphiteGrainComposition.hpp"

//////////////////////////////////////////////////////////////////////

TrustGraphiteGrainComposition::TrustGraphiteGrainComposition()
{
}

//////////////////////////////////////////////////////////////////////

TrustGraphiteGrainComposition::TrustGraphiteGrainComposition(SimulationItem *parent)
{
    setParent(parent);
    setup();
}

//////////////////////////////////////////////////////////////////////

void TrustGraphiteGrainComposition::setupSelfBefore()
{
    GrainComposition::setupSelfBefore();

    loadOpticalGrid(true, "GrainComposition/Trust/Gra_121_1201.dat", false, true, false, true);
    loadEnthalpyGrid(true, "GrainComposition/Trust/Graphitic_Calorimetry_1000.dat");
    setBulkDensity(2.24e3);
}

//////////////////////////////////////////////////////////////////////

QString TrustGraphiteGrainComposition::name() const
{
    return "Trust_Graphite";
}

//////////////////////////////////////////////////////////////////////
