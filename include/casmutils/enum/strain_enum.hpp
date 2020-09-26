#ifndef STRAIN_ENUM_HH
#define STRAIN_ENUM_HH
#include <casm/clex/ConfigEnumStrain.hh>
#include <casmutils/xtal/structure.hpp>
#include <vector>

namespace casmutils
{
namespace enumerator
{
// TODO: Explain options and what it does
// TODO: sym_axes false seg faults, even in CLI. CASM issue
// TODO: auto_range is not an interfacing option. it's the same as sym_axes if there is no min_val provided,
// else it's false. change this part
//
struct StrainEnumOptions
{
public:
    StrainEnumOptions(const Eigen::VectorXd& max_val, const Eigen::VectorXd& inc_val)
        : axes(Eigen::MatrixXd::Identity(max_val.rows(), max_val.rows())),
          min_val(Eigen::VectorXd::Zero(max_val.rows())),
          max_val(max_val),
          inc_val(inc_val),
          trim_corners(true),
          sym_axes(false),
          strain_metric("Hstrain")
    {
    }

    std::string strain_metric;
    Eigen::MatrixXd axes;
    Eigen::VectorXd min_val;
    Eigen::VectorXd max_val;
    Eigen::VectorXd inc_val;
    bool trim_corners;
    bool sym_axes;
};

std::vector<std::pair<std::string, xtal::Structure>> enumerate_strain(xtal::Structure& input_struc,
                                                                      StrainEnumOptions& input_options);
} // namespace enumerator
} // namespace casmutils

#endif /* ifndef STRAIN_ENUM_HH */
