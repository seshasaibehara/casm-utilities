#include "../../../autotools.hh"
#include <casmutils/strain/strain_enum.hpp>
#include <casmutils/xtal/structure.hpp>
#include <filesystem>
#include <gtest/gtest.h>

namespace cu = casmutils;
class StrainTest : public testing::Test
{
protected:
    using Structure = cu::xtal::Structure;

    void SetUp() override {}
};

TEST_F(StrainTest, enumeration)
{

    cu::fs::path strain_input_path(cu::autotools::input_filesdir / "strain_test.vasp");
    cu::xtal::Structure test_struc = cu::xtal::Structure::from_poscar(strain_input_path);
    Eigen::VectorXd inc_val(6), max_val(6), min_val(6);
    inc_val << 0.05, 0.05, 0.05, 0.05, 0.05, 0.05;
    max_val << 0.1, 0.1, 0.1, 0.1, 0.1, 0.1;
    min_val << -0.05, -0.05, -0.05, -0.05, -0.05, -0.05;

    cu::strain::StrainEnumOptions input_options(max_val, inc_val);
    input_options.sym_axes = true;
    input_options.min_val = min_val;
    cu::strain::enumerate_strain(test_struc, input_options);
    //    cu::strain::enumerate_strain(test_struc, input_options);
    EXPECT_TRUE(true);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
