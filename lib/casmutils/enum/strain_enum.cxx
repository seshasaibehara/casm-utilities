#include <casm/app/ClexDescription.hh>
#include <casm/app/ProjectSettings.hh>
#include <casm/casm_io/json/jsonParser.hh>
#include <casm/clex/Configuration.hh>
#include <casm/clex/PrimClex.hh>
#include <casm/clex/SimpleStructureTools.hh>
#include <casm/clex/Supercell.hh>
#include <casm/crystallography/AnisoValTraits.hh>
#include <casm/crystallography/Structure.hh>
#include <casm/enumerator/DoFSpace.hh>
#include <casm/enumerator/Enumerator.hh>
#include <casm/symmetry/SymRepTools.hh>
#include <casm/symmetry/io/json/SymRepTools.hh>
#include <casmutils/enum/misc.hpp>
#include <casmutils/enum/strain_enum.hpp>

namespace casmutils
{
namespace enumerator
{
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

    // Make Primclex from structure pointer
    CASM::ProjectSettings project_settings("project");
    CASM::ClexDescription clex_name;
    project_settings.set_default_clex(clex_name);
    CASM::PrimClex primclex(project_settings, casm_struc_ptr);
    CASM::AnisoValTraits{"occ", {}, CASM::AnisoValTraits::LOCAL};

    // Make supercell from primclex and input configuration from supercell
    CASM::Supercell super_cell(&primclex, Eigen::Matrix3i::Identity());
    CASM::ConfigEnumInput input_config(super_cell);

    // Printing strain symmetry analysis
    // TODO: Need to move this out somewhere so that symmetry analysis can be printed out for any structure
    CASM::DoFSpace dof_space(input_config, input_options.strain_metric);
    CASM::jsonParser sym_report;
    sym_report = CASM::vector_space_sym_report(dof_space, input_options.sym_axes);
    std::filesystem::path current_path = std::filesystem::current_path();
    std::string file_name = current_path.string() + "/strain_sym_analysis.json";
    sym_report.write(file_name);
    //    sym_report.print(std::cout);

    // TODO: This is an empty filter expression. In future maybe it can be a part of input arguments
    std::vector<std::string> filter_expr;

    // Setting auto_range based on whether min_val is provided or not
    // If min_val is provided, it is false else it is the same as sym_axes
    // Logic taken from how it works in CASM. See ConfigEnumStrain.cc.
    bool auto_range = false;
    if (input_options.min_val.isZero() == true)
    {
        auto_range = input_options.sym_axes;
    }

    CASM::ConfigEnumStrain strained_configs =
        CASM::ConfigEnumStrain::run_without_inserting_configs(primclex,
                                                              input_config,
                                                              input_options.axes.transpose(),
                                                              input_options.min_val,
                                                              input_options.max_val,
                                                              input_options.inc_val,
                                                              input_options.sym_axes,
                                                              auto_range,
                                                              input_options.trim_corners,
                                                              filter_expr);

    // Get configurations from ConfigEnumStrain object
    std::vector<CASM::Configuration> configs;
    for (const auto& config : strained_configs)
    {
        configs.push_back(config);
    }

    // Find unique configs from the strained configs
    std::vector<CASM::Configuration> unique_configs = extend::get_unique_configurations(configs);

    // Make casmutilities structure from unique configs
    std::vector<std::pair<std::string, xtal::Structure>> enumerated_structures;
    for (const auto& config : unique_configs)
    {
        enumerated_structures.emplace_back(
            std::make_pair(CASM::config_json_string(config), xtal::Structure(CASM::make_simple_structure(config))));
    }

    return enumerated_structures;
}
} // namespace enumerator
} // namespace casmutils
