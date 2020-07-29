#include <casm/app/ProjectSettings.hh>
#include <casm/clex/Configuration.hh>
#include <casm/clex/PrimClex.hh>
#include <casm/clex/SimpleStructureTools.hh>
#include <casm/clex/Supercell.hh>
#include <casm/crystallography/AnisoValTraits.hh>
#include <casm/crystallography/Structure.hh>
#include <casm/database/ConfigDatabase.hh>
#include <casm/database/ScelDatabase.hh>
#include <casm/enumerator/Enumerator.hh>
#include <casmutils/strain/strain_enum.hpp>
#include <map>
#include <vector>

namespace casmutils
{
namespace strain
{
std::vector<std::pair<std::string, xtal::Structure>> enumerate_strain(xtal::Structure& input_struc,
                                                                      StrainEnumOptions& input_options)
{
    CASM::xtal::BasicStructure casm_basic_struc = input_struc.__get<CASM::xtal::BasicStructure>();

    // Set strain dof for BasicStructure obtained from input_struc
    std::map<std::string, CASM::xtal::DoFSet> strain_dof;
    strain_dof.emplace(input_options.strain_metric, CASM::AnisoValTraits::strain(input_options.strain_metric));
    casm_basic_struc.set_global_dofs(strain_dof);

    // Make Primclex from BasicStructure
    std::shared_ptr<CASM::Structure> casm_struc_ptr;
    casm_struc_ptr.reset(new CASM::Structure(casm_basic_struc));
    CASM::ProjectSettings project_settings("project");
    CASM::PrimClex primclex(project_settings, casm_struc_ptr);

    // Make a supercell object of size 1 and stuff it in primclex
    CASM::Supercell super_cell(&primclex, Eigen::Matrix3i::Identity());
    primclex.db<CASM::Supercell>().insert(super_cell);

    // Now make a configuration from the supercell created and stuff it in primclex
    CASM::Configuration super_cell_conf(super_cell);
    primclex.db<CASM::Configuration>().insert(super_cell_conf);

    // Now make a ConfigEnumInput object from the configuration created
    CASM::ConfigEnumInput enum_input_conf(super_cell_conf);

    // TODO: This is an empty filter expression. In future maybe it can be a part of input arguments
    std::vector<std::string> filter_expr;

    // Enumerating by calling the run in casm
    CASM::ConfigEnumStrain::run(primclex,
                                enum_input_conf,
                                input_options.axes,
                                input_options.min_val,
                                input_options.max_val,
                                input_options.inc_val,
                                input_options.sym_axes,
                                input_options.auto_range,
                                input_options.trim_corners,
                                filter_expr,
                                input_options.dry_run);

    // Assuming that now primclex got stuffed with the enumerated configs need a way to extract them out.
    auto& config_db = primclex.db<CASM::Configuration>();

    // Once you have the config database, get the structure and config.json string
    std::vector<std::pair<std::string, xtal::Structure>> enumerated_structures;

    for (auto& config : config_db)
    {
        enumerated_structures.emplace_back(
            std::make_pair(CASM::config_json_string(config), xtal::Structure(CASM::make_simple_structure(config))));
    }

    return enumerated_structures;
}
} // namespace strain
} // namespace casmutils
