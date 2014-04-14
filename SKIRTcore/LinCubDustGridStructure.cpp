/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include "FatalError.hpp"
#include "LinCubDustGridStructure.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

LinCubDustGridStructure::LinCubDustGridStructure()
{
}

////////////////////////////////////////////////////////////////////

void LinCubDustGridStructure::setupSelfBefore()
{
    CubDustGridStructure::setupSelfBefore();

    // verify property values
    if (_xmax <= 0) throw FATALERROR("the outer radius in the x direction should be positive");
    if (_Nx <= 0) throw FATALERROR("the number of grid points in the x direction should be positive");
    if (_ymax <= 0) throw FATALERROR("the outer radius in the y direction should be positive");
    if (_Ny <= 0) throw FATALERROR("the number of grid points in the y direction should be positive");
    if (_zmax <= 0) throw FATALERROR("the outer radius in the z direction should be positive");
    if (_Nz <= 0) throw FATALERROR("the number of grid points in the z direction should be positive");

    // grid distribution in x
    _xv.resize(_Nx+1);
    for (int i=0; i<=_Nx; ++i)
        _xv[i] = -_xmax + 2.0*i*_xmax/_Nx;

    // grid distribution in y
    _yv.resize(_Ny+1);
    for (int j=0; j<=_Ny; ++j)
        _yv[j] = -_ymax + 2.0*j*_ymax/_Ny;

    // grid distribution in z
    _zv.resize(_Nz+1);
    for (int k=0; k<=_Nz; ++k)
        _zv[k] = -_zmax + 2.0*k*_zmax/_Nz;

    // the total number of cells
    _Ncells = _Nx*_Ny*_Nz;
}

////////////////////////////////////////////////////////////////////

void LinCubDustGridStructure::setExtentX(double value)
{
    _xmax = value;
    _xmin = -value;
}

////////////////////////////////////////////////////////////////////

double LinCubDustGridStructure::extentX() const
{
    return _xmax;
}

////////////////////////////////////////////////////////////////////

void LinCubDustGridStructure::setPointsX(int value)
{
    _Nx = value;
}

////////////////////////////////////////////////////////////////////

int LinCubDustGridStructure::pointsX() const
{
    return _Nx;
}

////////////////////////////////////////////////////////////////////

void LinCubDustGridStructure::setExtentY(double value)
{
    _ymax = value;
    _ymin = -value;
}

////////////////////////////////////////////////////////////////////

double LinCubDustGridStructure::extentY() const
{
    return _ymax;
}

////////////////////////////////////////////////////////////////////

void LinCubDustGridStructure::setPointsY(int value)
{
    _Ny = value;
}

////////////////////////////////////////////////////////////////////

int LinCubDustGridStructure::pointsY() const
{
    return _Ny;
}

////////////////////////////////////////////////////////////////////

void LinCubDustGridStructure::setExtentZ(double value)
{
    _zmax = value;
    _zmin = -value;
}

////////////////////////////////////////////////////////////////////

double LinCubDustGridStructure::extentZ() const
{
    return _zmax;
}

////////////////////////////////////////////////////////////////////

void LinCubDustGridStructure::setPointsZ(int value)
{
    _Nz = value;
}

////////////////////////////////////////////////////////////////////

int LinCubDustGridStructure::pointsZ() const
{
    return _Nz;
}

//////////////////////////////////////////////////////////////////////