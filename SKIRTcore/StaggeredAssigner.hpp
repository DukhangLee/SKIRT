/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef STAGGEREDASSIGNER_HPP
#define STAGGEREDASSIGNER_HPP

#include "ProcessAssigner.hpp"

//////////////////////////////////////////////////////////////////////

/** The StaggeredAssigner class is a subclass of the ProcessAssigner class, representing objects
    that assign work to different processes. An object of the StaggeredAssigner class distributes
    the work amongst the different processes in a staggered way. This means that the first value
    (index zero) is assigned to the first process (index zero), the second value to the second, and
    so on until the cycle is repeated with the first process and the indices exceeding the number
    of processes. This kind of assignment is useful if the CPU time is not expected to be the same
    for the different tasks, but is a function of the index of each tasks. An example is the
    simulation of the life cycle of photon packages, for which the CPU time depends on the
    wavelength. An efficient parallelization of the simulation over the wavelengths therefore
    requires an object of the StaggeredAssigner class, in order to obtain a good load balancing
    amongst the parallel processes. After performing the work in parallel, communication is
    typically needed to accumulate the results stored at different processes.

    The assignment mechanism explained above is represented graphically in the following figure.

    \image html staggeredassigner.png "The StaggeredAssigner class distributes the work amongst the processes in a staggered way." */
class StaggeredAssigner : public ProcessAssigner
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an assigner that assigns the work to the processes in a staggered way")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE StaggeredAssigner();

    //======================== Other Functions =======================

public:
    /** This function invokes the assignment procedure. As an argument, it takes the number of parts of
        work \f$n\f$ that need to be performed. Based on this number, the number of processes and the
        rank of this process, this function determines the number of tasks that are assigned to this
        process and stores it in the _nvalues member. This is done as follows. First, a pointer to the
        PeerToPeerCommunicator object is obtained by using the find operation. Then, in a loop over all
        indices \f$t\f$ from zero to \f$n-1\f$, it is checked whether the process, which has rank
        \f$i\f$ is assigned to the value with index \f$t\f$. If this is so, the _nvalues member is
        incremented. The condition that is checked for each index \f$t\f$ can be written mathematically
        as: \f[ t \bmod{N_P} = i \f] where \f$N_P\f$ is the number of processes and \f$i\f$ is the rank
        of the process. */
    void assign(size_t size);

    /** This function takes the relative index of a certain part of the work assigned to this process
        as an argument and returns the absolute index of that part, a value from zero to the total
        amount of parts that need to be executed in the simulation. This absolute index \f$t\f$ is
        determined by the following formula: \f[ t = i + u \cdot N_P \f] where \f$i\f$ is the rank of
        the process, \f$u\f$ is the relative index and \f$N_P\f$ is the number of processes. */
    size_t absoluteIndex(size_t relativeIndex);

    /** This function returns the rank of the process that is assigned to a certain part of the work.
        This part is identified by its absolute index, passed to this function as an argument. The rank
        \f$j\f$ is calculated as follows: \f[ j = t \bmod{N_P} \f] where \f$t\f$ is the absolute index
        of the task and \f$N_P\f$ the number of processes. */
    int rankForIndex(size_t index) const;

    /** This function returns true if the different parts of work are distributed amongst the different
        processes and returns false if each process is assigned to the same work. In this class, the
        processes are assigned to different work so this function returns true. */
    bool parallel() const;
};

//////////////////////////////////////////////////////////////////////

#endif // STAGGEREDASSIGNER_HPP
