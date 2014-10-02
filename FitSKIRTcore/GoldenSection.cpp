/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "GoldenSection.hpp"
#include "FatalError.hpp"
#include <math.h>
#include <iostream>

using namespace std;

////////////////////////////////////////////////////////////////////

GoldenSection::GoldenSection()
{
    SimulationItem::setupSelfBefore();
}

////////////////////////////////////////////////////////////////////

void GoldenSection::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();
}

////////////////////////////////////////////////////////////////////

void GoldenSection::setMinlum(double value)
{
    _minLum = value;
}

////////////////////////////////////////////////////////////////////

double GoldenSection::minlum() const
{
    return _minLum;
}

////////////////////////////////////////////////////////////////////

void GoldenSection::setMaxlum(double value)
{
    _maxLum = value;
}

////////////////////////////////////////////////////////////////////

double GoldenSection::maxlum() const
{
    return _maxLum;
}

////////////////////////////////////////////////////////////////////

double GoldenSection::function(Array *frame, double x)
{
    double chi = 0;
    int arraysize = frame->size();

    for (int m=0; m<arraysize; m++)
    {
        double total_sim = x * ((*frame)[m]);
        double sigma = sqrt( abs((*_ref)[m]) + total_sim);
        if ((*_ref)[m]==0)
        {
            (*frame)[m] = 0;
            total_sim = 0;
            sigma = 0;
        }
        else
        {
            chi += pow( ((*_ref)[m] - total_sim) / sigma,2);
        }
    }
    return chi;
}

////////////////////////////////////////////////////////////////////

void GoldenSection::optimize(const Array *Rframe, Array *frame, double &lum, double &chi2)
{

    _ref = Rframe;
    Array *sim = frame;
    double GOLD = 1-0.61803;

    double chi = 1;
    double lumvalue = 1;
    double prev_lumvalue=1;
    double a0 = _minLum;
    double b0 = _maxLum;

    for(int j=0;j<300;j++){

        double d=(b0-a0)*GOLD;
        double a1=a0+d;
        double b1=b0-d;
        double chia1= function(sim, a1);
        double chib1= function(sim, b1);

        if(chia1<chib1){
            b0=b1;
            lumvalue=a1;
            chi=chia1;
        }
        else{
            a0=a1;
            lumvalue=b1;
            chi=chib1;
        }
        if(abs(prev_lumvalue-lumvalue)/lumvalue <= 1e-5 && j!=0) j=300;
        else prev_lumvalue = lumvalue;
    }
    chi2=chi;
    lum=lumvalue;
    frame = sim;

}

////////////////////////////////////////////////////////////////////
