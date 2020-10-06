#include <casm/app/ClexDescription.hh>
#include <casm/app/ProjectSettings.hh>
#include <casm/casm_io/json/jsonParser.hh>
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
#include <casmutils/enum/sitedofs_enum.hpp>

namespace casmutils
{
namespace enumerator
{

std::vector<std::pair<std::string, xtal::Structure>> enumerate_sitedofs(xtal::Structure& input_struc,
                                                                        SiteDoFEnumOptions& input_options)
{
    CASM::xtal::BasicStructure casm_basic_struc = input_struc.__get<CASM::xtal::BasicStructure>();

    // Set site dofs with whatever dof you want to enumerate over
    std::map<std::string, CASM::xtal::SiteDoFSet> site_dofs;
    site_dofs.emplace(input_options.dof, CASM::AnisoValTraits{input_options.dof});
    for (CASM::xtal::Site& site : casm_basic_struc.set_basis())
    {
        site.set_dofs(site_dofs);
    }

    // Make structure pointer from BasicStructure
    std::shared_ptr<CASM::Structure> casm_struc_ptr;
    casm_struc_ptr.reset(new CASM::Structure(casm_basic_struc));

    // Make Primclex from structure pointer
    CASM::ProjectSettings project_settings("project");
    CASM::ClexDescription clex_name;
    project_settings.set_default_clex(clex_name);
    CASM::PrimClex primclex(project_settings, casm_struc_ptr);
    CASM::AnisoValTraits{"occ", {}, CASM::AnisoValTraits::LOCAL};

    CASM::Supercell super_cell(&primclex, Eigen::Matrix3i::Identity());
    CASM::ConfigEnumInput input_config(super_cell);

    CASM::DoFSpace dof_space(input_config, input_options.dof);
    CASM::jsonParser sym_report;
    bool calc_wedges = false;
    sym_report = CASM::vector_space_sym_report(dof_space, calc_wedges);
    std::filesystem::path current_path = std::filesystem::current_path();
    std::string file_name = current_path.string() + "/dof_sym_analyis.json";
    sym_report.write(file_name);

    std::vector<std::string> filter_expr;
    CASM::ConfigEnumSiteDoFs enum_configs =
        CASM::ConfigEnumSiteDoFs::run_without_inserting_configs(primclex,
                                                                input_config,
                                                                input_options.dof,
                                                                input_options.axes.transpose(),
                                                                input_options.min_val,
                                                                input_options.max_val,
                                                                input_options.inc_val,
                                                                input_options.sym_axes,
                                                                input_options.trans_modes,
                                                                input_options.min_nonzero,
                                                                input_options.max_nonzero,
                                                                filter_expr);

    // Get configurations form enum_configs
    std::vector<CASM::Configuration> configs;
    for (const auto& config : enum_configs)
    {
        configs.push_back(config);
    }

    // Get unique configurations from configs
    std::vector<CASM::Configuration> unique_configs = extend::get_unique_configurations(configs);

    // Make casmutils structure to return
    std::vector<std::pair<std::string, xtal::Structure>> enumerated_structures;
    for (const auto& config : unique_configs)
    {
        std::cout << CASM::config_json_string(config) << std::endl;
        std::cout << "-------------" << std::endl;
        enumerated_structures.emplace_back(
            std::make_pair(CASM::config_json_string(config), xtal::Structure(CASM::make_simple_structure(config))));
    }

    return enumerated_structures;
}

} // namespace enumerator
} // namespace casmutils
