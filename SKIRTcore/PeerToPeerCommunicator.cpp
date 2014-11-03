/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "Array.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "ProcessManager.hpp"

////////////////////////////////////////////////////////////////////

void PeerToPeerCommunicator::sum(Array& arr)
{
    if (!isMultiProc()) return;

    Array results(arr.size());

    ProcessManager::sum(&(arr[0]),&results[0],arr.size(),0);
    if (isRoot()) arr = results;
}

////////////////////////////////////////////////////////////////////

void PeerToPeerCommunicator::sum_all(Array& arr)
{
    if (!isMultiProc()) return;

    Array results(arr.size());

    ProcessManager::sum_all(&(arr[0]),&results[0],arr.size());
    arr = results;
}

////////////////////////////////////////////////////////////////////

bool PeerToPeerCommunicator::isRoot()
{
    return !getRank();
}

////////////////////////////////////////////////////////////////////