#include <iostream>
#include <random>
#include <vector>
#include <algorithm>

const int lifeExpectancyMeanDistribution = 55;
const int lifeExpectancyStandartDeviation = 10;
const int years = 200;
const int quaestorAge = 30;
const int aedileAge = 36;
const int praetorAge = 39;
const int consulAge = 42;
const int aedileService = 2;
const int preatorService = 2;
const int consulService = 2;
const int reelectionTime = 10;
const int quaestorPosition = 20;
const int aedilesPosition = 10;
const int praetorsPosition = 8;
const int consulsPosition = 2;
const int PSIFirst = 100;
const int penaltyUnfilledPosition = -5;
const int penaltyConsulReelection = -10;
const int annualMeanDistribution = 15;
const int annualStandartDeviation = 5;


class Politicians {
public:
    Politicians(int age) : age(age) {}
    int getAge() const { return age; }
private:
    int age;
};

class SimulationOfPoliticians {
public:
    SimulationOfPoliticians() : psi(PSIFirst), year(0) {
        politicians.reserve(quaestorPosition + aedilesPosition + praetorsPosition + consulsPosition);
        for (int i = 0; i < quaestorPosition; ++i) {
            politicians.emplace_back(quaestorAge);
        }
        for (int i = 0; i < aedilesPosition; ++i) {
            politicians.emplace_back(aedileAge);
        }
        for (int i = 0; i < praetorsPosition; ++i) {
            politicians.emplace_back(praetorAge);
        }
        for (int i = 0; i < consulsPosition; ++i) {
            politicians.emplace_back(consulAge);
        }
    }

    void yearSimulation(std::mt19937& gen, std::normal_distribution<>& lifeExpectancyDist) {
        std::shuffle(politicians.begin(), politicians.end(), gen);
        for (auto& politician : politicians) {
            politician = Politicians(politician.getAge() + 1);
        }

        politicians.erase(std::remove_if(politicians.begin(), politicians.end(),
            [&lifeExpectancyDist, &gen](const Politicians& p) { return p.getAge() > lifeExpectancyDist(gen); }), politicians.end());

        psi += penaltyUnfilledPosition * (quaestorPosition - countPoliticians(quaestorAge, aedileAge)) +
            penaltyUnfilledPosition * (aedilesPosition - countPoliticians(aedileAge, praetorAge)) +
            penaltyUnfilledPosition * (praetorsPosition - countPoliticians(praetorAge, consulAge)) +
            penaltyUnfilledPosition * (consulsPosition - countPoliticians(consulAge, lifeExpectancyMeanDistribution));

        if (year % reelectionTime == 0) {
            psi += penaltyConsulReelection * (consulsPosition - countPoliticians(consulAge, lifeExpectancyMeanDistribution));
        }
    }

    void runTheSimulation() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<> influxDist(annualMeanDistribution, annualStandartDeviation);
        std::normal_distribution<> lifeExpectancyDist(lifeExpectancyMeanDistribution, lifeExpectancyStandartDeviation);

        for (year = 0; year < years; ++year) {
            int influx = static_cast<int>(influxDist(gen));
            for (int i = 0; i < influx; ++i) {
                politicians.emplace_back(quaestorAge);
            }

            yearSimulation(gen, lifeExpectancyDist);
        }
    }

    int lastPSI() const { return psi; }

    void distributionOfAge() {
        std::vector<int> quaestorAges;
        std::vector<int> aedileAges;
        std::vector<int> praetorAges;
        std::vector<int> consulAges;

        for (const auto& politician : politicians) {
            int age = politician.getAge();
            if (age >= quaestorAge && age < aedileAge) {
                quaestorAges.push_back(age);
            }
            else if (age >= aedileAge && age < praetorAge) {
                aedileAges.push_back(age);
            }
            else if (age >= praetorAge && age < consulAge) {
                praetorAges.push_back(age);
            }
            else if (age >= consulAge) {
                consulAges.push_back(age);
            }
        }

        auto calculateSummary = [](const std::vector<int>& ages) -> std::pair<unsigned __int64, double> {
            if (ages.empty()) {
                return std::make_pair(0ULL, 0.0);
            }

            unsigned __int64 totalAges = 0;
            for (int age : ages) {
                totalAges += age;
            }
            double averageAge = static_cast<double>(totalAges) / ages.size();

            return std::make_pair(static_cast<unsigned __int64>(ages.size()), averageAge);
        };

        auto quaestorInfo = calculateSummary(quaestorAges);
        auto aedileInfo = calculateSummary(aedileAges);
        auto praetorInfo = calculateSummary(praetorAges);
        auto consulInfo = calculateSummary(consulAges);

        std::cout << "Age Distribution:" << std::endl;
        std::cout << "Quaestor: Count=" << quaestorInfo.first << ", Average Age=" << quaestorInfo.second << std::endl;
        std::cout << "Aedile: Count=" << aedileInfo.first << ", Average Age=" << aedileInfo.second << std::endl;
        std::cout << "Praetor: Count=" << praetorInfo.first << ", Average Age=" << praetorInfo.second << std::endl;
        std::cout << "Consul: Count=" << consulInfo.first << ", Average Age=" << consulInfo.second << std::endl;
    }

private:
    int psi;
    int year;
    std::vector<Politicians> politicians;

    int countPoliticians(int minAge, int maxAge) {
        return std::count_if(politicians.begin(), politicians.end(),
            [minAge, maxAge](const Politicians& p) { return p.getAge() >= minAge && p.getAge() < maxAge; });
    }
};

int main() {
    SimulationOfPoliticians simulation;
    simulation.runTheSimulation();

    std::cout << "End-of-Simulation PSI: " << simulation.lastPSI() << std::endl;

    std::cout << "Annual Fill Rate:" << std::endl;
    std::cout << "Quaestor: " << static_cast<double>(quaestorPosition) / years * 100 << "%" << std::endl;
    std::cout << "Aedile: " << static_cast<double>(aedilesPosition) / years * 100 << "%" << std::endl;
    std::cout << "Praetor: " << static_cast<double>(praetorsPosition) / years * 100 << "%" << std::endl;
    std::cout << "Consul: " << static_cast<double>(consulsPosition) / years * 100 << "%" << std::endl;

    simulation.distributionOfAge();

    return 0;
}