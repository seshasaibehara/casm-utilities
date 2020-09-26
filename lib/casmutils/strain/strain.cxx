#include <casmutils/strain/strain.hpp>

namespace casmutils
{
namespace strain
{

Eigen::Matrix3d deformation_tensor_to_strain_metric(const std::string strain_metric,
                                                    const Eigen::Matrix3d& deformation_tensor)
{

    char m = std::tolower(strain_metric[0]);

    switch (m)
    {
    case 'g':
        return CASM::strain::deformation_tensor_to_metric<CASM::strain::METRIC::GREEN_LAGRANGE>(deformation_tensor);
    case 'b':
        return CASM::strain::deformation_tensor_to_metric<CASM::strain::METRIC::BIOT>(deformation_tensor);
    case 'h':
        return CASM::strain::deformation_tensor_to_metric<CASM::strain::METRIC::HENCKY>(deformation_tensor);
    case 'e':
        return CASM::strain::deformation_tensor_to_metric<CASM::strain::METRIC::EULER_ALMANSI>(deformation_tensor);
    default:
        throw std::runtime_error("Bad strain metric");
    }
}

Eigen::Matrix3d strain_metric_to_deformation_tensor(const std::string strain_metric,
                                                    const Eigen::Matrix3d& strain_tensor)
{

    char m = std::tolower(strain_metric[0]);

    switch (m)
    {
    case 'g':
        return CASM::strain::metric_to_deformation_tensor<CASM::strain::METRIC::GREEN_LAGRANGE>(strain_tensor);
    case 'b':
        return CASM::strain::metric_to_deformation_tensor<CASM::strain::METRIC::BIOT>(strain_tensor);
    case 'h':
        return CASM::strain::metric_to_deformation_tensor<CASM::strain::METRIC::HENCKY>(strain_tensor);
    case 'e':
        return CASM::strain::metric_to_deformation_tensor<CASM::strain::METRIC::EULER_ALMANSI>(strain_tensor);
    default:
        throw std::runtime_error("Bad strain metric");
    }
}
} // namespace strain
} // namespace casmutils
