#pragma once

#include "EvolutionConcepts.h"
#include "EvolutionImpl.h"
#include <iostream>
#include <algorithm>

// This library encapsulates evolutional learning through genetic algorithms 
// The library is header only and easy to use. 
// You have a type (the chromosome) whose values you want to optimize through evolutional learning
// and you have a challenge your type needs to master
// 

// These functions must be added to your chromosome type (the one you want to optimize)
//struct Chromosome {
//	void crossover(Chromosome const& other); make sure to randomly choose what data members should be taken from this or other
//	void mutate(); make sure to not randomly mutate too much of your chromosome type (only one data member at a time is recommended)
// std::string toString() const; provide some output for the shape of the chromosome
//};

// These functions must be added to your challenge type
//struct Challenge {
//	double score(Chromosome const& chromosome); the chromosome faces the challenge and its performance needs to be evaluated with a double (0 means bad, the higher the better the performance)
//};

namespace evol {

template<Chromosome Chrom, class Chall> 
requires Challenge<Chall, Chrom>
std::vector<Chrom>
evolution(
	std::vector<Chrom> population, // feed with a few (20 recommended) random chromosomes
	int num_generations, // the number of generations to cross
	Chall const& challenge, // the challenge 
	double& winningAccuracy, // the winning performance is an out parameter
	int log = 1 // logging level to see how far the algorithm progressed
)
{
	auto candidates = population;
	for (int i = 0; i < num_generations; ++i) {
		// let the chromosomes face the challenge
		std::multimap<double, const Chrom*> fitness = fitnessCalculation(candidates, challenge);
		// logging
		if (log >= 1) {
			std::cout << "generation nr. " << i + 1 << " / " << num_generations << '\n';
			if (log >= 2) {
				for (auto& f : fitness)
					std::cout << "  fitness: " << f.first << '\n' << f.second->toString() << '\n';
				std::cout << '\n';
			}
		}
		// half of the chromosomes are winners
		std::vector<const Chrom*> winners = selectMatingPool(fitness, 2);
		// return the winner of the last generation
		if (i >= num_generations - 1)
		{
			std::vector<Chrom> ret;
			for (auto w : winners)
				ret.push_back(*w);
			winningAccuracy = fitness.rbegin()->first;
			return ret;
		}
		// sort the chromosomes according to their fitness
		typename std::vector<Chrom>::iterator sep = std::stable_partition(
			candidates.begin(), candidates.end(), 
			[&winners](Chrom const& ch) {
			return std::find_if(winners.begin(), winners.end(),
				[&ch](const Chrom* address) { return &ch == address; }) != winners.end();
		});
		// cross parents
		crossover<Chrom>(candidates.begin(), sep, sep, candidates.end());
		// mutate children
		randomMutation<Chrom>(sep, candidates.end());
	}
	return candidates;
}

}
