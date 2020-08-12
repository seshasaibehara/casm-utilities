#include <casm/app/ClexDescription.hh>
#include <casm/app/ProjectSettings.hh>
#include <casm/clex/ConfigIsEquivalent.hh>
#include <casm/clex/Configuration.hh>
#include <casm/clex/PrimClex.hh>
#include <casm/clex/SimpleStructureTools.hh>
#include <casm/clex/Supercell.hh>
#include <casm/crystallography/AnisoValTraits.hh>
#include <casm/crystallography/Structure.hh>
#include <casm/database/ConfigDatabase.hh>
#include <casm/database/ScelDatabase.hh>
#include <casm/enumerator/Enumerator.hh>
#include <casm/symmetry/SymRepTools.hh>
#include <casmutils/strain/strain_enum.hpp>
#include <map>
#include <vector>

namespace casmutils
{
namespace strain
{
// A function which tells whether a configuration exists in the given vector of configurations
// Makes use of ConfigIsEquivalent class to compare configuration
// This function can be used to get unique configurations from a list of configurations
bool find_if_config_exists(CASM::Configuration& config, std::vector<CASM::Configuration>& configs)
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

std::vector<std::pair<std::string, xtal::Structure>> enumerate_strain(xtal::Structure& input_struc,
                                                                      StrainEnumOptions& input_options)
{
    CASM::xtal::BasicStructure casm_basic_struc = input_struc.__get<CASM::xtal::BasicStructure>();

    // Set strain dof for BasicStructure obtained from input_struc
    std::map<std::string, CASM::xtal::DoFSet> strain_dof;
    strain_dof.emplace(input_options.strain_metric, CASM::AnisoValTraits::strain(input_options.strain_metric));
    casm_basic_struc.set_global_dofs(strain_dof);

    // Make structure pointer from BasicStructure
    std::shared_ptr<CASM::Structure> casm_struc_ptr;
    casm_struc_ptr.reset(new CASM::Structure(casm_basic_struc));

    // Make Primclex from BasicStructure
    CASM::ProjectSettings project_settings("project");
    CASM::ClexDescription clex_name;
    project_settings.set_default_clex(clex_name);
    CASM::PrimClex primclex(project_settings, casm_struc_ptr);
    CASM::AnisoValTraits{"occ", {}, CASM::AnisoValTraits::LOCAL};

    CASM::Supercell super_cell(&primclex, Eigen::Matrix3i::Identity());
    CASM::ConfigEnumInput enum_input_conf(super_cell);

    // TODO: This is an empty filter expression. In future maybe it can be a part of input arguments
    std::vector<std::string> filter_expr;
    CASM::ConfigEnumStrain strained_configs =
        CASM::ConfigEnumStrain::run_without_inserting_configs(primclex,
                                                              enum_input_conf,
                                                              input_options.axes,
                                                              input_options.min_val,
                                                              input_options.max_val,
                                                              input_options.inc_val,
                                                              input_options.sym_axes,
                                                              input_options.auto_range,
                                                              input_options.trim_corners,
                                                              filter_expr);

    // Find unique configs from the strained configs
    // The configs need to be in canonical form to compare
    std::vector<CASM::Configuration> unique_configs;
    for (const auto& config : strained_configs)
    {
        CASM::Configuration canonical_config = config.canonical_form();
        if (casmutils::strain::find_if_config_exists(canonical_config, unique_configs) == false)
        {
            unique_configs.push_back(canonical_config);
        }
    }

    // Make casmutilities structure from unique configs
    std::vector<std::pair<std::string, xtal::Structure>> enumerated_structures;
    for (const auto& config : unique_configs)
    {
        enumerated_structures.emplace_back(
            std::make_pair(CASM::config_json_string(config), xtal::Structure(CASM::make_simple_structure(config))));
    }

    return enumerated_structures;
}
} // namespace strain
} // namespace casmutils
