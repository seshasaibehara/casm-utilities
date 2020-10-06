#ifndef SITEDOFS_ENUM_HH
#define SITEDOFS_ENUM_HH
#include <casm/clex/ConfigEnumSiteDoFs.hh>
#include <casmutils/xtal/structure.hpp>

namespace casmutils
{
namespace enumerator
{
// TODO: Explain options explicitly
struct SiteDoFEnumOptions
{
public:
    SiteDoFEnumOptions(const std::string& dof,
                       const int& tot_dim,
                       const Eigen::VectorXd& max_val,
                       const Eigen::VectorXd& inc_val)
        : dof(dof),
          axes(Eigen::MatrixXd::Identity(tot_dim, tot_dim)),
          inc_val(inc_val),
          max_val(max_val),
          min_val(Eigen::VectorXd::Zero(max_val.rows())),
          sym_axes(false),
          min_nonzero(0),
          max_nonzero(max_val.rows()),
          trans_modes(true)
    {
    }

    std::string dof;
    Eigen::VectorXd inc_val;
    Eigen::VectorXd min_val;
    Eigen::VectorXd max_val;
    Eigen::MatrixXd axes;
    bool sym_axes;
    bool trans_modes;
    int min_nonzero;
    int max_nonzero;
};

std::vector<std::pair<std::string, xtal::Structure>> enumerate_sitedofs(xtal::Structure& input_struc,
                                                                        SiteDoFEnumOptions& input_options);

} // namespace enumerator
} // namespace casmutils

#endif /* ifndef SITEDOFS_ENUM_HH */
