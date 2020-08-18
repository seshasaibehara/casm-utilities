#include "../../../autotools.hh"
#include <casmutils/enum/sitedofs_enum.hpp>
#include <casmutils/xtal/structure.hpp>
#include <gtest/gtest.h>

namespace cu = casmutils;
class SiteDoFsTest : public testing::Test
{
protected:
    using Structure = cu::xtal::Structure;
    void SetUp() override {}
};

TEST_F(SiteDoFsTest, enumeration)
{
    cu::fs::path bcc_mo_path(cu::autotools::input_filesdir / "BCC_Mo.vasp");
    Structure bcc_struc = Structure::from_poscar(bcc_mo_path);
    Eigen::VectorXd inc_val(3), max_val(3);
    inc_val << 0.5, 0.5, 0.5;
    max_val << 1, 1, 1;
    std::string dof = "disp";
    cu::enumerator::SiteDoFEnumOptions input_options(dof, max_val, inc_val);
    std::vector<std::pair<std::string, Structure>> enumerated_strucs =
        cu::enumerator::enumerate_sitedofs(bcc_struc, input_options);
    EXPECT_EQ(enumerated_strucs.size(), 10);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
