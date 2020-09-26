#include <casm/clex/ConfigIsEquivalent.hh>
#include <casmutils/enum/misc.hpp>

namespace extend
{

bool find_if_config_exists(const CASM::Configuration& config, const std::vector<CASM::Configuration>& configs)
{

    CASM::ConfigIsEquivalent config_comparator(config, 1e-5);
    auto it = std::find_if(configs.begin(), configs.end(), [&](const CASM::Configuration& other_config) {
        return config_comparator(other_config);
    });

    if (it == configs.end())
    {
        return false;
    }

    return true;
}

std::vector<CASM::Configuration> get_unique_configurations(const std::vector<CASM::Configuration>& configs)
{
    std::vector<CASM::Configuration> unique_canonical_configs;
    std::vector<CASM::Configuration> unique_configs;

    for (const auto& config : configs)
    {
        CASM::Configuration canonical_config = config.canonical_form();
        if (find_if_config_exists(canonical_config, unique_canonical_configs) == false)
        {
            unique_canonical_configs.push_back(canonical_config);
            unique_configs.push_back(config);
        }
    }

    return unique_configs;
}

} // namespace extend
