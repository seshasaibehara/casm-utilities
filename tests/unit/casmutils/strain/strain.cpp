#include <casmutils/strain/strain.hpp>
#include <gtest/gtest.h>

class StrainTest : public testing::Test
{
protected:
    double tol = 1e-5;
    Eigen::Matrix3d F;
    void SetUp() override { F << 0.5, 0.2, 0.3, 0.2, 0.3, 0.6, 0.3, 0.6, 0.1; }
};

TEST_F(StrainTest, StrainToDeformationAndBack)
{

    Eigen::Matrix3d computed_F0 = casmutils::strain::strain_metric_to_deformation_tensor(
        "H", casmutils::strain::deformation_tensor_to_strain_metric("H", F));
    EXPECT_TRUE((computed_F0.transpose() * computed_F0).isApprox(F.transpose() * F));

    Eigen::Matrix3d computed_F1 = casmutils::strain::strain_metric_to_deformation_tensor(
        "G", casmutils::strain::deformation_tensor_to_strain_metric("G", F));
    EXPECT_TRUE((computed_F1.transpose() * computed_F1).isApprox(F.transpose() * F));

    Eigen::Matrix3d computed_F2 = casmutils::strain::strain_metric_to_deformation_tensor(
        "B", casmutils::strain::deformation_tensor_to_strain_metric("B", F));
    EXPECT_TRUE((computed_F2.transpose() * computed_F2).isApprox(F.transpose() * F));

    Eigen::Matrix3d computed_F3 = casmutils::strain::strain_metric_to_deformation_tensor(
        "E", casmutils::strain::deformation_tensor_to_strain_metric("E", F));
    EXPECT_TRUE((computed_F3.transpose() * computed_F3).isApprox(F.transpose() * F));
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
