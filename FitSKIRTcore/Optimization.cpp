/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "Optimization.hpp"

#include "AdjustableSkirtSimulation.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"
#include "MasterSlaveManager.hpp"
#include "OligoFitScheme.hpp"
#include "Parallel.hpp"
#include "ParallelFactory.hpp"
#include "ParameterRange.hpp"
#include "ParameterRanges.hpp"
#include "ReferenceImages.hpp"
#include "Units.hpp"
#include <QDir>

using namespace std;

//////////////////////////////////////////////////////////////////////

void MPIEvaluator(GAPopulation & p)
{
    Optimization *opt = (Optimization *)p.userData();
    opt->PopEvaluate(p);
}

//////////////////////////////////////////////////////////////////////

Optimization::Optimization()
    :_genome(0)
{
        _bestChi2=1e20;
        _consec=0;
}

//////////////////////////////////////////////////////////////////////

void Optimization::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();
    FilePaths* path = find<FilePaths>();

    ParameterRanges* ranges = find<ParameterRanges>();
    foreach (ParameterRange* range, ranges->ranges())
    {
        GARealAlleleSet RealAllele(range->minimumValue(),range->maximumValue());
        _allelesetarray.add(RealAllele);
    }

    _genome= new GARealGenome(_allelesetarray);
    _genome->initializer(GARealGenome::UniformInitializer);
    _genome->mutator(GARealGaussianMutator);
    _genome->crossover(GARealUniformCrossover);
    _genome->userData(this);
    _ga= new GASteadyStateGA(*_genome);
    GASigmaTruncationScaling scaling;
    _ga->minimize();
    GAPopulation popu = _ga->population();
    popu.userData(this);
    popu.evaluator(MPIEvaluator);
    _ga->population(popu);
    _ga->populationSize(_popsize);
    _ga->nGenerations(_generations);
    _ga->pMutation(_pmut);
    _ga->pCrossover(_pcross);
    _ga->scaling(scaling);
    _ga->scoreFrequency(0);  // was 1e-22 implicitly converted to integer 0
    _ga->selectScores(GAStatistics::AllScores);
    _ga->flushFrequency(0);  // was 1e-22 implicitly converted to integer 0

    //must be done before initiliazing the GA
    MasterSlaveManager * mgr = find<MasterSlaveManager>();
    FitScheme * fitsch = find<FitScheme>();
    mgr->setLocalSlaveCount(fitsch->parallelSimulationCount());
    mgr->registerTask(this, &Optimization::chi2);
    if(mgr->isMaster())
    {
        QString filepath =path->outputPath()+path->outputPrefix()+"_allsimulations.dat";
        _stream.open(filepath.toLocal8Bit().constData());
        filepath =path->outputPath()+path->outputPrefix()+"_BESTsimulations.dat";
        _beststream.open(filepath.toLocal8Bit().constData());

    }
}

//////////////////////////////////////////////////////////////////////

void Optimization::setPopsize(int value)
{
    _popsize = value;
}

//////////////////////////////////////////////////////////////////////

int Optimization::popsize() const
{
    return _popsize;
}

//////////////////////////////////////////////////////////////////////

void Optimization::setGenerations(int value)
{
    _generations = value;
}

//////////////////////////////////////////////////////////////////////

int Optimization::generations() const
{
    return _generations;
}

//////////////////////////////////////////////////////////////////////

void Optimization::setPmut(double value)
{
    _pmut = value;
}

//////////////////////////////////////////////////////////////////////

double Optimization::pmut() const
{
    return _pmut;
}

//////////////////////////////////////////////////////////////////////

void Optimization::setPcross(double value)
{
    _pcross = value;
}

//////////////////////////////////////////////////////////////////////

double Optimization::pcross() const
{
    return _pcross;
}

//////////////////////////////////////////////////////////////////////

bool Optimization::done()
{
   return _ga->done();
}

//////////////////////////////////////////////////////////////////////

void Optimization::initialize()
{
   return _ga->initialize();
}

//////////////////////////////////////////////////////////////////////

QVariant Optimization::chi2(QVariant input)
{
    QList<QVariant> totalVarList = input.toList();
    int index = totalVarList[0].toInt();
    QList<QVariant> valuesVarList = totalVarList[1].toList();

    ParameterRanges* ranges = find<ParameterRanges>();
    int counter=0;
    QVector<double> currentValues;
    AdjustableSkirtSimulation::ReplacementDict * replacementsGenome =
            new AdjustableSkirtSimulation::ReplacementDict();
    foreach (ParameterRange* range, ranges->ranges())
    {
        double value = valuesVarList[counter].toDouble();
        (*replacementsGenome)[range->label()] = qMakePair(value, range->quantityString());
        counter++;
    }
    OligoFitScheme* oligofit = find<OligoFitScheme>();
    QList<double> DiskLuminosities, BulgeRatios, Chis;
    double chi_sum = oligofit->objective((*replacementsGenome),&DiskLuminosities,&BulgeRatios,&Chis, index);

    QList<QVariant> output, diskl, bulger, chivalues;
    for(int i=0; i<DiskLuminosities.size(); i++)
    {
        diskl.append(DiskLuminosities[i]);
        bulger.append(BulgeRatios[i]);
        chivalues.append(Chis[i]);
    }
    output.append(chi_sum);
    output.insert(output.size(),diskl);
    output.insert(output.size(),bulger);
    output.insert(output.size(),chivalues);

    return output;

}

//////////////////////////////////////////////////////////////////////

void Optimization::splitChi()
{

    QVector<QVariant> data(_genValues.size());

    for(int i =0;i<_genValues.size();i++)
    {
        QList<QVariant> valuesVarList;
        for(int j = 0; j<_genValues[0].size(); j++)
        {
            valuesVarList.append((double)(_genValues[i])[j]);
        }
        QList<QVariant> totalVarList;
        totalVarList.append(i);
        totalVarList.insert(totalVarList.size(),valuesVarList);
        data[i]=totalVarList;
    }
    MasterSlaveManager* mgr = find<MasterSlaveManager>();
    data = mgr->performTask(data);

    for(int i =0;i<_genValues.size();i++)
    {
        QList<QVariant> output = data[i].toList();
        double chi_sum = output[0].toDouble();
        QList<QVariant> diskl = output[1].toList();
        QList<QVariant> bulger = output[2].toList();
        QList<QVariant> chivalues = output[3].toList();
        QList<double> DiskLuminosities, BulgeRatios, Chis;
        for(int j = 0; j<diskl.size(); j++)
        {
            DiskLuminosities.append(diskl[j].toDouble());
            BulgeRatios.append(bulger[j].toDouble());
            Chis.append(chivalues[j].toDouble());
        }
        _genScores[i]=chi_sum;
        _genLum[i]=DiskLuminosities;
        _genBulgeratios[i]=BulgeRatios;
        _genChis[i]=Chis;

    }

}

//////////////////////////////////////////////////////////////////////

void Optimization::step()
{
    _ga->step();
}

//////////////////////////////////////////////////////////////////////

void Optimization::writeList(std::ofstream *stream, QList<double> list)
{
    for (int i=0; i<list.size(); i++)
        (*stream)<<list[i]<<" ";
}

//////////////////////////////////////////////////////////////////////

void Optimization::writeBest(int index, int consec)
{
    _beststream<<consec<<" ";
    writeLine(&_beststream, index);
    ReferenceImages* refs = find<ReferenceImages>();
    refs->writeOutBest(index, consec);
}

//////////////////////////////////////////////////////////////////////

void Optimization::writeLine(std::ofstream *stream, int i)
{
    for(int j=0;j<_genUnitsValues[0].size();j++)
        (*stream)<<(_genUnitsValues[i])[j]<<" ";
    (*stream)<<_genScores[i]<<" ";
    writeList(stream, _genLum[i]);
    writeList(stream, _genBulgeratios[i]);
    writeList(stream, _genChis[i]);
    (*stream)<<endl;
}

//////////////////////////////////////////////////////////////////////

void Optimization::PopEvaluate(GAPopulation & p)
{
    find<Log>()->info("------ Evaluating current generation -----");

    //creating a temporary folder to store the simulations
    QString folderpath = find<FilePaths>()->output("tmp");
    if(!QDir(folderpath).exists())
        QDir().mkdir(folderpath);

    //loop over all individuals and make replacement for all unevaluated individuals
    for (int i=0; i<p.size(); i++)
    {
        if (p.individual(i).isEvaluated()==gaFalse)
        {
            GARealGenome & genome = (GARealGenome &)p.individual(i);
            ParameterRanges* ranges = find<ParameterRanges>();

            //loop over all ranges to use the correct label but use the genome values to create the replacement
            int counter=0;
            QVector<double> currentUnitsValues, currentValues;
            foreach (ParameterRange* range, ranges->ranges())
            {
                double value = genome.gene(counter);
                currentValues.push_back(value);
                if (range->quantityString()!="")
                    value = find<Units>()->out(range->quantityString(),value);
                currentUnitsValues.push_back(value);
                counter++;
            }
            _genIndices.append(i);
            _genValues.append(currentValues);
            _genUnitsValues.append(currentUnitsValues);
        }
    }
    _genScores.resize(_genIndices.size());
    _genLum.resize(_genIndices.size());
    _genBulgeratios.resize(_genIndices.size());
    _genChis.resize(_genIndices.size());

    //Calculate the objective function values in parallel
    splitChi();

    //set the individuals scores and write out all and the best solutions
    find<Log>()->info("------ Setting Scores -----");
    for (int i=0; i<_genIndices.size(); i++)
    {
        p.individual(_genIndices[i]).score(_genScores[i]);
        _stream<<p.geneticAlgorithm()->generation()<<" ";
        writeLine(&_stream, i);
        if (_genScores[i]<_bestChi2)
        {
            _bestChi2=_genScores[i];
            writeBest(i,_consec);
            _consec++;
        }
    }
    clearGen(folderpath);
}

//////////////////////////////////////////////////////////////////////

void Optimization::clearGen(const QString & dirName)
{
    _genReplacement.clear();
    _genIndices.clear();
    _genScores.clear();
    _genValues.clear();
    _genLum.clear();
    _genBulgeratios.clear();
    _genChis.clear();
    _genUnitsValues.clear();

    QDir dir(dirName);
    find<Log>()->info("removing: "+dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  |
                                                    QDir::AllDirs | QDir::Files, QDir::DirsFirst))
        {
            if (!info.isDir())
                QFile::remove(info.absoluteFilePath());
        }
    }
    dir.rmdir(dirName);
}

//////////////////////////////////////////////////////////////////////