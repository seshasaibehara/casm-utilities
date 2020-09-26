#include <casm/clex/Configuration.hh>
#include <vector>

namespace extend
{

// A function which tells whether a configuration exists in the given vector of configurations
bool find_if_config_exists(const CASM::Configuration& config, const std::vector<CASM::Configuration>& configs);

// A function which gives out unique configurations from a given vector of configurations
std::vector<CASM::Configuration> get_unique_configurations(const std::vector<CASM::Configuration>& configs);

} // namespace extend
