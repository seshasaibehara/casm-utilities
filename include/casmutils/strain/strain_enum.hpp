#ifndef STRAIN_ENUM_HH
#define STRAIN_ENUM_HH
#include <algorithm>
#include <casm/clex/ConfigEnumStrain.hh>
#include <casmutils/xtal/structure.hpp>

namespace casmutils
{
namespace strain
{

// TODO: Explain options and what it does
// TODO: sym_axes false seg faults, even in CLI. CASM issue
struct StrainEnumOptions
{
public:
    StrainEnumOptions(Eigen::VectorXd& max_val, Eigen::VectorXd& inc_val)
        : axes(Eigen::MatrixXd::Identity(max_val.rows(), max_val.cols())),
          min_val(Eigen::VectorXd::Zero(max_val.rows())),
          max_val(max_val),
          inc_val(inc_val),
          auto_range(false),
          trim_corners(true),
          sym_axes(true),
          strain_metric("Hstrain")
    {
    }

    std::string strain_metric;
    Eigen::MatrixXd axes;
    Eigen::VectorXd min_val;
    Eigen::VectorXd max_val;
    Eigen::VectorXd inc_val;
    bool auto_range;
    bool trim_corners;
    bool sym_axes;
};

std::vector<std::pair<std::string, xtal::Structure>> enumerate_strain(xtal::Structure& input_struc,
                                                                      StrainEnumOptions& input_options);

} // namespace strain
} // namespace casmutils

#endif /* ifndef STRAIN_ENUM_HH */
