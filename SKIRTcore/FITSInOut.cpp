/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <QDateTime>
#include <QMutex>
#include "FatalError.hpp"
#include "FITSInOut.hpp"
#include "fitsio.h"

using namespace std;

////////////////////////////////////////////////////////////////////

namespace
{
    // mutex to guard the FITS input/output operations
    QMutex _mutex;

    // function to report cfitsio errors
    void report_error(QString filepath, QString action, int status)
    {
        char message[FLEN_STATUS];
        ffgerr(status, message);
        throw FATALERROR("Error while " + action + " FITS file " + filepath + "\n" + QString(message));
    }
}

////////////////////////////////////////////////////////////////////

void FITSInOut::write(QString filepath, const Array& data, int nx, int ny, int nz,
                    double incx, double incy, QString dataUnits, QString xyUnits)
{
    // verify the data size
    int nelements = data.size();
    if (nelements != nx*ny*nz)
        throw FATALERROR("Inconsistent data size when creating FITS file " + filepath);
    long naxes[3] = {nx, ny, nz};

    // acquire a global lock since the cfitsio library is not guaranteed to be reentrant
    // (only when it is built with ./configure --enable-reentrant; make)
    QMutexLocker lock(&_mutex);

    // time stamp and temporaries
    std::string stamp = QDateTime::currentDateTime().toUTC().toString("yyyy-MM-ddThh:mm:ss").toStdString();
    std::string localpath = filepath.toLocal8Bit().constData();
    std::string dataunits = dataUnits.toStdString();
    std::string xyunits = xyUnits.toStdString();
    double zero = 0.;
    double one = 1.;
    double xref = (nx+1.0)/2.0;
    double yref = (ny+1.0)/2.0;

    // remove any existing file with the same name
    remove(localpath.c_str());

    // create the fits file
    int status = 0;
    fitsfile *fptr;
    ffdkinit(&fptr, localpath.c_str(), &status);
    if (status) report_error(filepath, "creating", status);

    // create the primary image (32-bit floating point pixels)
    ffcrim(fptr, FLOAT_IMG, (nz==1 ? 2 : 3), naxes, &status);
    if (status) report_error(filepath, "creating", status);

    // add the relevant keywords
    ffpky(fptr, TDOUBLE, "BSCALE", &one, "", &status);
    ffpky(fptr, TDOUBLE, "BZERO", &zero, "", &status);
    ffpkys(fptr, "DATE"  , const_cast<char*>(stamp.c_str()), "Date and time of creation (UTC)", &status);
    ffpkys(fptr, "ORIGIN", const_cast<char*>("SKIRT simulation"), "Astronomical Observatory, Ghent University", &status);
    ffpkys(fptr, "BUNIT" , const_cast<char*>(dataunits.c_str()), "Physical unit of the array values", &status);
    ffpky(fptr, TDOUBLE, "CRPIX1", &xref, "X-axis coordinate system reference pixel", &status);
    ffpky(fptr, TDOUBLE, "CRVAL1", &zero, "Coordinate system value at X-axis reference pixel", &status);
    ffpky(fptr, TDOUBLE, "CDELT1", &incx, "Coordinate increment along X-axis", &status);
    ffpkys(fptr, "CTYPE1", const_cast<char*>(xyunits.c_str()), "Physical units of the X-axis increment", &status);
    ffpky(fptr, TDOUBLE, "CRPIX2", &yref, "Y-axis coordinate system reference pixel", &status);
    ffpky(fptr, TDOUBLE, "CRVAL2", &zero, "Coordinate system value at Y-axis reference pixel", &status);
    ffpky(fptr, TDOUBLE, "CDELT2", &incy, "Coordinate increment along Y-axis", &status);
    ffpkys(fptr, "CTYPE2", const_cast<char*>(xyunits.c_str()), "Physical units of the Y-axis increment", &status);
    if (status) report_error(filepath, "writing", status);

    // write the array of pixels to the image
    ffpprd(fptr, 0, 1, nelements, const_cast<double*>(&data[0]), &status);
    if (status) report_error(filepath, "writing", status);

    // close the file
    ffclos(fptr, &status);
    if (status) report_error(filepath, "writing", status);
}

////////////////////////////////////////////////////////////////////

void FITSInOut::read(QString filepath, Array& data, int& nx, int& ny, int& nz)
{
    // acquire a global lock since the cfitsio library is not guaranteed to be reentrant
    QMutexLocker lock(&_mutex);

    // open the fits file
    int status = 0;
    fitsfile *fptr;
    ffdkopn(&fptr, filepath.toLocal8Bit().constData(), READONLY, &status);
    if (status) report_error(filepath, "opening", status);

    // get the dimensions of the primary image
    int naxis;
    long naxes[3];
    ffgidm(fptr, &naxis, &status);
    ffgisz(fptr, 3, naxes, &status);
    if (status) report_error(filepath, "reading", status);
    nx = naxis > 0 ? naxes[0] : 1;
    ny = naxis > 1 ? naxes[1] : 1;
    nz = naxis > 2 ? naxes[2] : 1;

    // resize the data container
    int nelements = nx*ny*nz;
    data.resize(nelements);

    // read the array of pixels from the image
    int dummy;
    ffgpvd(fptr, 0, 1, nelements, 0, &data[0], &dummy, &status);
    if (status) report_error(filepath, "reading", status);

    // close the file
    ffclos(fptr, &status);
    if (status) report_error(filepath, "reading", status);
}

////////////////////////////////////////////////////////////////////