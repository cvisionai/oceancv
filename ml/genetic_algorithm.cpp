#include "oceancv/ml/genetic_algorithm.h"

namespace ocv {

	GeneticAlgorithm::GeneticAlgorithm(float (*quality)(const std::vector<int>&), size_t individual_size, size_t population_amount, size_t population_size, float mutation_rate, float keep_rate, float mutation_rate_drop) : _quality(quality),  _population_amount(population_amount), _population_size(population_size), _mutation_rate(mutation_rate), _keep_rate(keep_rate), _mutation_rate_drop(mutation_rate_drop) {
		
		if(individual_size > 0)
			initRandom(individual_size);
		
	}
	
	void GeneticAlgorithm::initRandom(size_t individual_size) {
	
		// Create random population
		_populations = {};
		_fitnesses = {};
		
		float total_max_fitness = 0.0;
		
		int random;
		for(std::size_t p = 0; p < _population_amount; p++) {
			
			_populations.push_back({});
			_fitnesses.push_back({});
			
			for(std::size_t i = 0; i < _population_size; i++) {

				_populations[p].push_back({});

				for(std::size_t c = 0; c < individual_size; c++) {
					random = rand() % 100;
					if(random > 50)
						_populations[p][i].push_back(1);
					else
						_populations[p][i].push_back(0);
				}
			
				_fitnesses[p].push_back(std::tuple<int,float,size_t>(i,_quality(_populations[p][i]),0));
				
				if(std::get<1>(_fitnesses[p][i]) > total_max_fitness) {
					total_max_fitness = std::get<1>(_fitnesses[p][i]);
					_total_max_population = p;
				}
				
			}
			
			std::sort(_fitnesses[p].begin(),_fitnesses[p].end(),_sortFitness);
			
		}
		
	}
	
	std::pair<size_t,float> GeneticAlgorithm::optimize(float niche_merging, size_t niche_wanderers, size_t max_iterations, float max_fitness) {
	
		float tmp_fitness = 0.0, random;
		size_t i;
		
		for(i = 0; i < max_iterations; i++) {
		
			// Perform evolution step
			tmp_fitness = evolve();
			
			// Check if maximum fitness criterion has been reached
			if(tmp_fitness >= max_fitness) {
				break;
			}
			
			// Eventually move individuals between populations
			random = ((double) rand() / (RAND_MAX));
			if(random < niche_merging) {
				mergeNiches(niche_wanderers);
			}
			
			// Eventually reduce mutation rate
			mutationRate(mutationRate() + _mutation_rate_drop);

		}
		
		return std::pair<size_t,float>(i,tmp_fitness);
		
	}
	
	float GeneticAlgorithm::evolve() {
		
		std::vector<std::tuple<int,float, size_t>> tmp_fitness, new_fitness;
		std::vector<std::vector<int>> tmp_population, new_population, partner_population;
		std::vector<int> individuum;
		std::size_t partner, source;
		double rate;
		float sum_fitness, min_fitness, max_fitness, total_max_fitness = 0.0;
		size_t multiplication;
		
		// Evolve each population individually
		for(std::size_t p = 0; p < _population_amount; p++) {
		
			tmp_fitness = {};
			new_fitness = {};
			
			tmp_population = {};
			new_population = {};
			partner_population = {};
			
			sum_fitness = 0.0;
			min_fitness = 1.0;
			max_fitness = 0.0;
		
			// Compute total fitness
			for(std::tuple<int,float, size_t> pp : _fitnesses[p]) {
				sum_fitness += std::get<1>(pp);
				min_fitness = std::min(min_fitness,std::get<1>(pp));
				max_fitness = std::max(max_fitness,std::get<1>(pp));
			}
			if(min_fitness == 0.0) {
				min_fitness = 0.000001;
			}

			// Compute multiplication factors to support fitter individuals in mating
			for(std::tuple<int,float,size_t> pp : _fitnesses[p]) {
				individuum = _populations[p][std::get<0>(pp)];
				multiplication = std::get<1>(pp) / min_fitness;
				for(size_t i = 0; i < multiplication; i++) {
					partner_population.push_back(individuum);
				}
			}
		
			// Crossover of individuums with partner space
			for(std::size_t i = 0; i < _population_size; i++) {
				
				individuum = _populations[p][i];
				
				// Find a random partner
				partner = rand() % partner_population.size();
				
				// Combine with individuum from partner population
				for(std::size_t c = 0; c < individuum.size(); c++) {
					source = rand() % 2;
					if(source == 0) {
						individuum[c] = partner_population[partner][c];
					}
				}
				
				// Mutate
				for(std::size_t c = 0; c < individuum.size(); c++) {
					rate = ((double) rand() / (RAND_MAX));
					if(rate > _mutation_rate) {
						individuum[c] = 1 - individuum[c];
					}
				}
				
				tmp_population.push_back(individuum);
				tmp_fitness.push_back(std::tuple<int,float,size_t>(i,_quality(individuum),0));
				
			}
		
			// Keep fittest fraction of lasts generation and also add mutants of them
			std::size_t limit = _population_size * _keep_rate;
			for(std::tuple<int,float,size_t> pp : _fitnesses[p]) {
				
				individuum = _populations[p][std::get<0>(pp)];
	
				new_population.push_back(individuum);
				new_fitness.push_back(std::tuple<int,float,size_t>(new_fitness.size(),std::get<1>(pp),std::get<2>(pp)+1));
				
				// Mutate kept individuums
				for(std::size_t c = 0; c < individuum.size(); c++) {
					rate = ((double) rand() / (RAND_MAX));
					if(rate > _mutation_rate) {
						individuum[c] = 1 - individuum[c];
					}
				}
				
				new_population.push_back(individuum);
				new_fitness.push_back(std::tuple<int,float,size_t>(new_fitness.size(),_quality(individuum),std::get<2>(pp)+1));
				
				if(new_fitness.size() > 2 * limit)
					break;
			
			}
			_fitnesses[p].erase(_fitnesses[p].begin(),_fitnesses[p].begin() + limit);
			
			// Keep fittest fraction of new generation and also add mutants of them
			std::sort(tmp_fitness.begin(),tmp_fitness.end(),_sortFitness);
			for(std::tuple<int,float,size_t> pp : tmp_fitness) {
				
				individuum = tmp_population[std::get<0>(pp)];
	
				new_population.push_back(individuum);
				new_fitness.push_back(std::tuple<int,float,size_t>(new_fitness.size(),std::get<1>(pp),std::get<2>(pp)+1));
				
				// Mutate kept individuums
				for(std::size_t c = 0; c < individuum.size(); c++) {
					rate = ((double) rand() / (RAND_MAX));
					if(rate > _mutation_rate) {
						individuum[c] = 1 - individuum[c];
					}
				}
				
				new_population.push_back(individuum);
				new_fitness.push_back(std::tuple<int,float,size_t>(new_fitness.size(),_quality(individuum),std::get<2>(pp)+1));
				
				if(new_fitness.size() > 4 * limit)
					break;
			
			}
			tmp_fitness.erase(tmp_fitness.begin(),tmp_fitness.begin() + limit);
			
			// Add 90 percent fittest of last and current generation
			for(std::tuple<int,float,size_t> pp : _fitnesses[p]) {
				tmp_population.push_back(_populations[p][std::get<0>(pp)]);
				tmp_fitness.push_back(std::tuple<int,float,size_t>(tmp_fitness.size(),std::get<1>(pp),std::get<2>(pp)));
			}

			// Pick fittest of offsprings
			std::sort(tmp_fitness.begin(),tmp_fitness.end(),_sortFitness);
			for(std::tuple<int,float,size_t> pp : tmp_fitness) {
				new_population.push_back(tmp_population[std::get<0>(pp)]);
				new_fitness.push_back(std::tuple<int,float,size_t>(new_fitness.size(),std::get<1>(pp),std::get<2>(pp)));
				if(new_fitness.size() == _population_size)
					break;
			}
			
			_populations[p] = new_population;
			_fitnesses[p] = new_fitness;
			
			std::sort(_fitnesses[p].begin(),_fitnesses[p].end(),_sortFitness);

			if(std::get<1>(_fitnesses[p][0]) > total_max_fitness) {
				total_max_fitness = std::get<1>(_fitnesses[p][0]);
				_total_max_population = p;
			}
			
		}
		
		return total_max_fitness;
		
	}
	
	void GeneticAlgorithm::mergeNiches(int wanderer) {
		
		if(wanderer * _population_amount > _population_size) {
			wanderer = _population_size / _population_amount - 1;
			if(wanderer < 0)
				wanderer = 0;
		}
		
		std::vector<std::vector<std::tuple<int,float,size_t>>> tmp_fitnesses = {};
		std::vector<std::vector<std::vector<int>>> tmp_populations = {};

		std::vector<int> individuum;
		
		float total_max_fitness = 0.0;
		std::size_t partner;
		
		for(size_t p = 0; p < _population_amount; p++) {
			
			tmp_populations.push_back({});
			tmp_fitnesses.push_back({});
			
			// Take two random samples from each other population
			for(size_t q = 0; q < _population_amount; q++) {
				
				if(p == q)
					continue;
				
				for(int i = 0; i < wanderer; i++) {
					partner = rand() % _population_size;
					individuum = _populations[q][std::get<0>(_fitnesses[q][partner])];
					tmp_populations[p].push_back(individuum);
					tmp_fitnesses[p].push_back(std::tuple<int,float,size_t>(tmp_fitnesses[p].size(),std::get<1>(_fitnesses[q][partner]),std::get<2>(_fitnesses[q][partner])));
				}
				
			}

			
			// Fill tmp population with best of this population
			for(std::tuple<int,float,size_t> pp : _fitnesses[p]) {
				tmp_populations[p].push_back(_populations[p][std::get<0>(pp)]);
				tmp_fitnesses[p].push_back(std::tuple<int,float,size_t>(tmp_fitnesses[p].size(),std::get<1>(pp),std::get<2>(pp)));
				if(tmp_fitnesses[p].size() == _population_size)
					break;
			}
			
			
			// Sort This populations fitness
			std::sort(tmp_fitnesses[p].begin(), tmp_fitnesses[p].end(), _sortFitness);
			
			// Find global fittest individual
			for(size_t i = 0; i < _population_size; i++) {
				if(std::get<1>(tmp_fitnesses[p][i]) > total_max_fitness) {
					total_max_fitness = std::get<1>(tmp_fitnesses[p][i]);
					_total_max_population = p;
				}
			}
			
		}
		
		// Store temporary variables
		_populations = tmp_populations;
		_fitnesses = tmp_fitnesses;
		
	}
	
	std::vector<int> GeneticAlgorithm::winner() const {
		return _populations[_total_max_population][std::get<0>(_fitnesses[_total_max_population][0])];
	}
	
	void GeneticAlgorithm::keepRate(float rate) {
		_keep_rate = rate;
	}
	
	void GeneticAlgorithm::mutationRate(float rate) {
		_mutation_rate = std::max(0.00f,std::min(1.f,rate));
	}
	
	void GeneticAlgorithm::mutationRateDrop(float rate) {
		_mutation_rate_drop = std::min(0.00f,std::max(-1.f,rate));
	}
	
	void GeneticAlgorithm::populationSize(std::size_t size) {
		_population_size = size;
	}
	
	size_t GeneticAlgorithm::populationSize() const {
		return _population_size;
	}
	
	float GeneticAlgorithm::mutationRate() const {
		return _mutation_rate;
	}
	
	float GeneticAlgorithm::mutationRateDrop() const {
		return _mutation_rate_drop;
	}
	
	float GeneticAlgorithm::keepRate() const {
		return _keep_rate;
	}
	
	bool GeneticAlgorithm::_sortFitness(const std::tuple<int,float,size_t>& f1, const std::tuple<int,float,size_t>& f2) {
		return std::get<1>(f1) > std::get<1>(f2);
	}
	
}
