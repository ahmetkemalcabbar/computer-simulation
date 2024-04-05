#include <iostream>
#include <vector>
#include <algorithm>
#include <random>


const int miniageOfPoliticianConsul = 42;
const int minServiceAedile = 2;
const int minServicePraetor = 2;
const int minServiceConsul = 2;
const int reElectionOfConsul = 10;
const int quaestoresOfEachYear = 20;
const int aedilesOfEachYear = 10;
const int praetorsOfEachYear = 8;
const int consulsOfEachYear = 2;
const int PSIstartPoint = 100;
const int penaltyPositionUnfilled = -5;
const int penaltyForReElectionConsecutiveConsul = -10;
const int meanOfAnnual_influx = 15;
const int stdDevOfAnnual_influx = 5;
const int meanOfLifeExpectancy = 55;
const int stdDevOfLifeExpectancy = 10;
const int years = 200;
const int minimumageOfPoliticianQuaestor = 30;
const int miniageOfPoliticianAedile = 36;
const int miniageOfPoliticianPraetor = 39;

class Politician {
public:
    Politician(int ageOfPolitician) : ageOfPolitician(ageOfPolitician) {}
    int getageOfPolitician() const { return ageOfPolitician; }
private:
    int ageOfPolitician;
};

class simulationOfRomeRepublic {
public:
    simulationOfRomeRepublic() : psi(PSIstartPoint), year(0) {
        politicians.reserve(quaestoresOfEachYear + aedilesOfEachYear + praetorsOfEachYear + consulsOfEachYear);
        for (int i = 0; i < quaestoresOfEachYear; ++i) {
            politicians.emplace_back(minimumageOfPoliticianQuaestor);
        }
        for (int i = 0; i < aedilesOfEachYear; ++i) {
            politicians.emplace_back(miniageOfPoliticianAedile);
        }
        for (int i = 0; i < praetorsOfEachYear; ++i) {
            politicians.emplace_back(miniageOfPoliticianPraetor);
        }
        for (int i = 0; i < consulsOfEachYear; ++i) {
            politicians.emplace_back(miniageOfPoliticianConsul);
        }
    }

    void yearSimulation(std::mt19937& gen, std::normal_distribution<>& lifeExpectancyDist) {
        std::shuffle(politicians.begin(), politicians.end(), gen);
        for (auto& politician : politicians) {
            politician = Politician(politician.getageOfPolitician() + 1);
        }

        politicians.erase(std::remove_if(politicians.begin(), politicians.end(),
            [&lifeExpectancyDist, &gen](const Politician& p) { return p.getageOfPolitician() > lifeExpectancyDist(gen); }), politicians.end());

        psi += penaltyPositionUnfilled * (quaestoresOfEachYear - politicianCounter(minimumageOfPoliticianQuaestor, miniageOfPoliticianAedile)) +
            penaltyPositionUnfilled * (aedilesOfEachYear - politicianCounter(miniageOfPoliticianAedile, miniageOfPoliticianPraetor)) +
            penaltyPositionUnfilled * (praetorsOfEachYear - politicianCounter(miniageOfPoliticianPraetor, miniageOfPoliticianConsul)) +
            penaltyPositionUnfilled * (consulsOfEachYear - politicianCounter(miniageOfPoliticianConsul, meanOfLifeExpectancy));

        if (year % reElectionOfConsul == 0) {
            psi += penaltyForReElectionConsecutiveConsul * (consulsOfEachYear - politicianCounter(miniageOfPoliticianConsul, meanOfLifeExpectancy));
        }
    }

    void runningFunction() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<> influxDist(meanOfAnnual_influx, stdDevOfAnnual_influx);
        std::normal_distribution<> lifeExpectancyDist(meanOfLifeExpectancy, stdDevOfLifeExpectancy);

        for (year = 0; year < years; ++year) {
            int influx = static_cast<int>(influxDist(gen));
            for (int i = 0; i < influx; ++i) {
                politicians.emplace_back(minimumageOfPoliticianQuaestor);
            }

            yearSimulation(gen, lifeExpectancyDist);
        }
    }

    int PSIcalculation() const { return psi; }

    void ageOfPoliticianDistributionCalculation() {
        std::vector<int> quaestorageOfPoliticians;
        std::vector<int> aedileageOfPoliticians;
        std::vector<int> praetorageOfPoliticians;
        std::vector<int> consulageOfPoliticians;

        for (const auto& politician : politicians) {
            int ageOfPolitician = politician.getageOfPolitician();
            if (ageOfPolitician >= minimumageOfPoliticianQuaestor && ageOfPolitician < miniageOfPoliticianAedile) {
                quaestorageOfPoliticians.push_back(ageOfPolitician);
            }
            else if (ageOfPolitician >= miniageOfPoliticianAedile && ageOfPolitician < miniageOfPoliticianPraetor) {
                aedileageOfPoliticians.push_back(ageOfPolitician);
            }
            else if (ageOfPolitician >= miniageOfPoliticianPraetor && ageOfPolitician < miniageOfPoliticianConsul) {
                praetorageOfPoliticians.push_back(ageOfPolitician);
            }
            else if (ageOfPolitician >= miniageOfPoliticianConsul) {
                consulageOfPoliticians.push_back(ageOfPolitician);
            }
        }

        auto calculateSummary = [](const std::vector<int>& ageOfPoliticians) -> std::pair<unsigned __int64, double> {
            if (ageOfPoliticians.empty()) {
                return std::make_pair(0ULL, 0.0);
            }

            unsigned __int64 totalageOfPoliticians = 0;
            for (int ageOfPolitician : ageOfPoliticians) {
                totalageOfPoliticians += ageOfPolitician;
            }
            double averageOfPoliticianageOfPolitician = static_cast<double>(totalageOfPoliticians) / ageOfPoliticians.size();

            return std::make_pair(static_cast<unsigned __int64>(ageOfPoliticians.size()), averageOfPoliticianageOfPolitician);
        };

        auto summaryQuaestor = calculateSummary(quaestorageOfPoliticians);
        auto summaryAedile = calculateSummary(aedileageOfPoliticians);
        auto summaryPraetor = calculateSummary(praetorageOfPoliticians);
        auto summaryConsul = calculateSummary(consulageOfPoliticians);

        std::cout << "ageOfPolitician Distribution:" << std::endl;
        std::cout << "Quaestor: Count=" << summaryQuaestor.first << ", AverageOfPolitician ageOfPolitician=" << summaryQuaestor.second << std::endl;
        std::cout << "Aedile: Count=" << summaryAedile.first << ", AverageOfPolitician ageOfPolitician=" << summaryAedile.second << std::endl;
        std::cout << "Praetor: Count=" << summaryPraetor.first << ", AverageOfPolitician ageOfPolitician=" << summaryPraetor.second << std::endl;
        std::cout << "Consul: Count=" << summaryConsul.first << ", AverageOfPolitician ageOfPolitician=" << summaryConsul.second << std::endl;
    }

private:
    int psi;
    int year;
    std::vector<Politician> politicians;

    int politicianCounter(int minageOfPolitician, int maxageOfPolitician) {
        return std::count_if(politicians.begin(), politicians.end(),
            [minageOfPolitician, maxageOfPolitician](const Politician& p) { return p.getageOfPolitician() >= minageOfPolitician && p.getageOfPolitician() < maxageOfPolitician; });
    }
};

int main() {
    simulationOfRomeRepublic simulationOfRomeRepublic;
    simulationOfRomeRepublic.runningFunction();

    std::cout << "End-of-simulationOfRomeRepublic PSI: " << simulationOfRomeRepublic.PSIcalculation() << std::endl;

    std::cout << "Annual Fill Rate:" << std::endl;
    std::cout << "Quaestor: " << static_cast<double>(quaestoresOfEachYear) / years * 100 << "%" << std::endl;
    std::cout << "Aedile: " << static_cast<double>(aedilesOfEachYear) / years * 100 << "%" << std::endl;
    std::cout << "Praetor: " << static_cast<double>(praetorsOfEachYear) / years * 100 << "%" << std::endl;
    std::cout << "Consul: " << static_cast<double>(consulsOfEachYear) / years * 100 << "%" << std::endl;

    simulationOfRomeRepublic.ageOfPoliticianDistributionCalculation();

    return 0;
}