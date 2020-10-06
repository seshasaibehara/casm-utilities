#include "../../../autotools.hh"
#include <casmutils/enum/strain_enum.hpp>
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

    cu::fs::path bcc_mo_path(cu::autotools::input_filesdir / "BCC_Mo.vasp");
    Structure bcc_struc = Structure::from_poscar(bcc_mo_path);
    Eigen::VectorXd inc_val(6), max_val(6), min_val(6);
    Eigen::MatrixXd axes(6, 6);
    axes << 0.5774, 0.5774, 0.5774, 0, 0, 0, -0.8165, 0.4082, 0.4082, 0, 0, 0, 0, -0.7071, 0.7071, 0, 0, 0, 0, 0, 0, 0,
        1, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 1, 0, 0;
    inc_val << 0.05, 0.05, 0.05, 0.05, 0.05, 0.05;
    max_val << 0.1, 0.1, 0.1, 0.1, 0.1, 0.1;
    min_val << -0.05, -0.05, -0.05, -0.05, -0.05, -0.05;

    cu::enumerator::StrainEnumOptions input_options(6, max_val, inc_val);
    input_options.min_val = min_val;
    input_options.axes = axes;
    // input_options.sym_axes = true;
    std::vector<std::pair<std::string, Structure>> enumerated_structures =
        cu::enumerator::enumerate_strain(bcc_struc, input_options);
    EXPECT_EQ(enumerated_structures.size(), 196);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
