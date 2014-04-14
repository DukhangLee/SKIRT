/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include "ExtragalacticUnits.hpp"

////////////////////////////////////////////////////////////////////

ExtragalacticUnits::ExtragalacticUnits()
{
    // select the appropriate units
    _unitForQty["length"] = "pc";
    _unitForQty["distance"] = "Mpc";
    _unitForQty["wavelength"] = "micron";
    _unitForQty["grainsize"] = "micron";
    _unitForQty["section"] = "m2";
    _unitForQty["volume"] = "pc3";
    _unitForQty["velocity"] = "km/s";
    _unitForQty["mass"] = "Msun";
    _unitForQty["bulkmass"] = "kg";
    _unitForQty["bulkmassdensity"] = "kg/m3";
    _unitForQty["masssurfacedensity"] = "Msun/pc2";
    _unitForQty["massvolumedensity"] = "Msun/pc3";
    _unitForQty["opacity"] = "m2/kg";
    _unitForQty["energy"] = "J";
    _unitForQty["bolluminosity"] = "Lsun";
    _unitForQty["monluminosity"] = "Lsun/micron";
    _unitForQty["bolflux"] = "W/m2";
    _unitForQty["monflux"] = "W/m2/micron";
    _unitForQty["bolsurfacebrightness"] = "W/m2/arcsec2";
    _unitForQty["monsurfacebrightness"] = "W/m2/micron/arcsec2";
    _unitForQty["temperature"] = "K";
    _unitForQty["angle"] = "arcsec";
    _unitForQty["posangle"] = "deg";
    _unitForQty["solidangle"] = "arcsec2";
    _unitForQty["pressure"] = "K/m3";

    // initialize conversion factors in base class
    initCache();
}

////////////////////////////////////////////////////////////////////