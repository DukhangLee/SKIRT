/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include "PhotonPackage.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

PhotonPackage::PhotonPackage()
    : _ynstellar(true),
      _ell(0),
      _L(0.0),
      _nscatt(0),
      _ad(0)
{
}

////////////////////////////////////////////////////////////////////

PhotonPackage::PhotonPackage(bool ynstellar,
                             int ell,
                             Position bfr,
                             Direction bfk,
                             double L,
                             int nscatt)
    : _ynstellar(ynstellar),
      _ell(ell),
      _bfr(bfr),
      _bfk(bfk),
      _L(L),
      _nscatt(nscatt),
      _ad(0)
{
}

////////////////////////////////////////////////////////////////////

void
PhotonPackage::set(bool ynstellar, int ell,
                   Position bfr, Direction bfk, double L, int nscatt)
{
    _ynstellar = ynstellar;
    _ell = ell;
    _bfr = bfr;
    _bfk = bfk;
    _L = L;
    _nscatt = nscatt;
    _ad = 0;
}

////////////////////////////////////////////////////////////////////

bool
PhotonPackage::ynstellar()
const
{
    return _ynstellar;
}

////////////////////////////////////////////////////////////////////

void
PhotonPackage::setynstellar(bool ynstellar)
{
    _ynstellar = ynstellar;
}

////////////////////////////////////////////////////////////////////

int
PhotonPackage::ell()
const
{
    return _ell;
}

////////////////////////////////////////////////////////////////////

void PhotonPackage::setell(int ell)
{
    _ell = ell;
}

////////////////////////////////////////////////////////////////////

Position
PhotonPackage::position()
const
{
    return _bfr;
}

////////////////////////////////////////////////////////////////////

Direction
PhotonPackage::direction()
const
{
    return _bfk;
}

////////////////////////////////////////////////////////////////////

void
PhotonPackage::setdirection(Direction bfk)
{
    _bfk = bfk;
}

////////////////////////////////////////////////////////////////////

double
PhotonPackage::luminosity()
const
{
    return _L;
}

////////////////////////////////////////////////////////////////////

void
PhotonPackage::setluminosity(double L)
{
    _L = L;
}

////////////////////////////////////////////////////////////////////

int
PhotonPackage::nscatt()
const
{
    return _nscatt;
}

////////////////////////////////////////////////////////////////////

void
PhotonPackage::setnscatt(int nscatt)
{
    _nscatt = nscatt;
}

////////////////////////////////////////////////////////////////////

const AngularDistribution* PhotonPackage::angularDistribution() const
{
    return _ad;
}

////////////////////////////////////////////////////////////////////

void PhotonPackage::setAngularDistribution(const AngularDistribution* ad)
{
    _ad = ad;
}

////////////////////////////////////////////////////////////////////

void
PhotonPackage::propagate(double s)
{
    _bfr = Position(_bfr + s*_bfk);
}

////////////////////////////////////////////////////////////////////