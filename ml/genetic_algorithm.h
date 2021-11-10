#pragma once

#include <vector>
#include <map>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <tuple>

namespace ocv {

class GeneticAlgorithm {
	
public:

	/**
	 * Constructor that initializes the optimization. Creates totally random individuals.
	 * Does not conduct the optimization!
	 * @param quality a function that assesses the fitness of an individual. Will be given a std::vector<int>
	 *        and needs to compute a fitness in [0,..,1]
	 * @param individual_size Size (vector length) of one individual. If its >0 then a random population will be created
	 * @param population_amount How many parallel populations are to be evolved
	 * @param population_size How many individuals shall exist per population
	 * @param mutation_rate How likely a mutation is
	 * @param keep_rate How large the fraction of individuals is that is to be retained for the next iteration
	 * @param mutation_rate_drop by how much the mutation_rate shall be reduced per optimization step
	 */
	GeneticAlgorithm(float (*quality)(const std::vector<int>&), size_t individual_size = 0, size_t population_amount = 10, size_t population_size = 100, float mutation_rate = 0.9, float keep_rate = 0.1, float mutation_rate_drop = -0.001);
	
	/**
	 * Initializes all populations randomly
	 */
	void initRandom(size_t individual_size);
	
	/**
	 * Runs multiple evolution iterations until a termination criterion is reached.
	 * @param niche_merging How likely a niche merging is
	 * @param niche_wanderers How many individuals cross the populations
	 * @param max_iterations Maximum number of iterations to run
	 * @param max_fitness Maximum fitness to be obtained
	 * @return std::pair<size_t,float> with the number of executed evolution steps and the maximum fitness that was obtained
	 */
	std::pair<size_t,float> optimize(float niche_merging = 0.01, size_t niche_wanderers = 1, size_t max_iterations = 5000, float max_fitness = 0.9);
	
	/**
	 * Performs one evolution step.
	 * @return the highest fitness obtained in this step.
	 */
	float evolve();
	
	/**
	 * Pretend that some individuals from the disconnected populations
	 * make it into the other populations. Simulates "gene transfer".
	 * @param wanderer the number of indidividuals to copy between the populations.
	 */
	void mergeNiches(int wanderer);
	
	/**
	 * Returns the one winner individual after the evolution
	 * @return std::vector<int> Binary vector of the winning individual
	 */
	std::vector<int> winner() const;
	
	// Some getter and setter functions
	void keepRate(float rate);
	
	void mutationRate(float rate);
	
	void mutationRateDrop(float rate);
	
	void populationSize(std::size_t size);
	
	size_t populationSize() const;
	
	float mutationRate() const;
	
	float mutationRateDrop() const;
	
	float keepRate() const;
	
private:

	// Function pointer to the quality function
	float (*_quality)(const std::vector<int>& individuum);
	
	// Helper function to sort a vector containing fitness values (like _fitnesses).
	static bool _sortFitness(const std::tuple<int,float,size_t>& f1, const std::tuple<int,float,size_t>& f2);
	
	// Contains the different populations of individuals. Size depoends on parameters
	std::vector<std::vector<std::vector<int>>> _populations;
	
	// The global list of individual's fitness
	std::vector<std::vector<std::tuple<int,float, size_t>>> _fitnesses;

	// Index to the best population
	size_t _total_max_population;
	
	// Number of populations to set up
	size_t _population_amount;
	
	// Upper size limit for each of the populations
	size_t _population_size;
	
	// Rate at which random inversions occur. A rate of 0.1 means in 1 of 10 cases a mutation will occur.
	float _mutation_rate;
	
	// How many of the fittest individuals are to be retained towards the next iteration
	float _keep_rate;
	
	// By how much the mutation rate shall be reduced per mutation step
	float _mutation_rate_drop;
	
};

}
