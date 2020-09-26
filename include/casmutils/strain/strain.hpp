#include <casm/crystallography/Strain.hh>

namespace casmutils
{
namespace strain
{

Eigen::Matrix3d deformation_tensor_to_strain_metric(const std::string strain_metric,
                                                    const Eigen::Matrix3d& deformation_tensor);

Eigen::Matrix3d strain_metric_to_deformation_tensor(const std::string strain_metric,
                                                    const Eigen::Matrix3d& strain_tensor);
} // namespace strain
} // namespace casmutils
