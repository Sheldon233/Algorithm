#include "tspdynp.hpp"
#include <iostream> // for debug purposes
#include <limits> // for UINT_MAX
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <bitset>

void generate_combinations(int n, int k, int idx, int start, std::vector<int>& arr, std::string& bitstring, std::vector<std::string>& result) {
    if (idx == k) {
        for (int i = 0; i < k; i++) {
            bitstring[arr[i]] = '1';
        }
        result.push_back(bitstring);
        for (int i = 0; i < k; i++) {
            bitstring[arr[i]] = '0';
        }
        return;
    }

    for (int i = start; i < n; i++) {
        arr[idx] = i;
        generate_combinations(n, k, idx + 1, i + 1, arr, bitstring, result);
    }
}

std::vector<std::string> bitstrings_with_i_ones(int i, int n) {
    std::vector<int> arr(i);
    std::string bitstring(n, '0');
    std::vector<std::string> result;
    generate_combinations(n, i, 0, 0, arr, bitstring, result);
    return result;
}

std::vector<unsigned> tsp_dynamic_program(const std::vector<std::vector<long>> & graph, unsigned n) {
	std::vector<std::vector<float>> dis(n, std::vector<float>(n));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            dis[i][j] = static_cast<float>(graph[i][j]);
        }
    }	


    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (dis[i][j] == 0) {
                dis[i][j] = INFINITY;
            }
        }
    }

    std::vector<std::vector<float>> dp(n, std::vector<float>(1 << n, 0));
    std::vector<std::vector<int>> dp_next_step(n, std::vector<int>(1 << n, 0));

    for (int i = 0; i < n; i++) {
        dp[i][0] = dis[i][0];
    }

    for (int num_unvisit = 1; num_unvisit < n - 1; num_unvisit++) {
        std::vector<std::string> column_idx = bitstrings_with_i_ones(num_unvisit, n);

        for (int row = 1; row < n; row++) {
            for (const std::string& combine : column_idx) {
                if (combine[0] != '1' && combine[row] != '1') {
                    int next_city = std::bitset<32>(combine).to_ulong();
                    float total_mincost = INFINITY;
                    int next_min_city = -1;

                    for (int i = 0; i < n; i++) {
                        if (combine[i] == '1') {
                            float first_step_cost = dis[row][i];
                            std::string new_combine = combine.substr(0, i) + "0" + combine.substr(i + 1);
                            int new_combine_int = std::bitset<32>(new_combine).to_ulong();
                            float next_steps_cost = dp[i][new_combine_int];

                            if (first_step_cost + next_steps_cost <= total_mincost) {
                                total_mincost = first_step_cost + next_steps_cost;
                                next_min_city = i;
                            }
                        }
                    }

                    dp[row][next_city] = total_mincost;
                    dp_next_step[row][next_city] = next_min_city;
                }
            }
        }
    }

    float tsp_min_cost = std::numeric_limits<float>::infinity();
    int initial_step_city = 0;

    for (int i = 1; i < n; i++) {
        std::string bit_string(n, '1');
        bit_string[0] = '0';

        float first_step_cost = dis[0][i];
        bit_string[i] = '0';
        int new_bit_string = std::bitset<32>(bit_string).to_ulong();

        float next_steps_cost = dp[i][new_bit_string];

        if (first_step_cost + next_steps_cost <= tsp_min_cost) {
            tsp_min_cost = first_step_cost + next_steps_cost;
            initial_step_city = i;
        }
    }
    std::vector<unsigned> route(n);
    route[0] = 0;
    route[1] = initial_step_city;

    std::string bit_string(n, '1');
    bit_string[0] = '0';
    bit_string[initial_step_city] = '0';

    for (int i = 0; i < n - 2; i++) {
        std::bitset<32> bitset(bit_string);
        int bit_to_int = static_cast<int>(bitset.to_ulong());
        int next_step_city = dp_next_step[initial_step_city][bit_to_int];
        route[i + 2] = next_step_city;

        bit_string[next_step_city] = '0';
        initial_step_city = next_step_city;
    }
    return route;
}
// Returns the cost of the journey described here.
long costOfJourney(const std::vector<std::vector<long>> & graph, const std::vector<unsigned> & journey)
{
    long cost=0;

    for (size_t i =0; i< journey.size()-1; i++)
    {
        unsigned src=journey[i];
        unsigned dst = journey[i + 1];
        cost += graph[src][dst];
    }

    cost += graph[journey[journey.size() - 1]][journey[0]];
    return cost; // this is a stub and is unlikely to be the right answer for most cases. 
}