#include "../../../autotools.hh"
#include <casmutils/enum/sitedofs_enum.hpp>
#include <casmutils/xtal/structure.hpp>
#include <gtest/gtest.h>

namespace cu = casmutils;
class SiteDoFsTest : public testing::Test
{
protected:
    using Structure = cu::xtal::Structure;
    void SetUp() override
    {
        // cu::fs::path con_bcc(cu::autotools::input_filesdir / "conventional_bcc.vasp");

        cu::fs::path con_bcc(cu::autotools::input_filesdir / "c.vasp");
        bcc_struc_ptr.reset(new Structure(Structure::from_poscar(con_bcc)));
        inc_val.resize(6, 1);
        max_val.resize(6, 1);
    }

    std::unique_ptr<Structure> bcc_struc_ptr;
    Eigen::VectorXd inc_val, max_val;
};

// TEST_F(SiteDoFsTest, enumeration)
//{
//    inc_val << 0.05, 0.05, 0.05, 0.05, 0.05, 0.05;
//    max_val << 0.1, 0.1, 0.1, 0.1, 0.1, 0.1;
//    std::string dof = "disp";
//    cu::enumerator::SiteDoFEnumOptions input_options(dof, max_val, inc_val);
//    std::vector<std::pair<std::string, Structure>> enumerated_strucs =
//        cu::enumerator::enumerate_sitedofs(*bcc_struc_ptr, input_options);
//    EXPECT_EQ(enumerated_strucs.size(), 92);
//}

// TEST_F(SiteDoFsTest, single_site_enumeration)
//{
//    inc_val << 0.05, 0.05, 0.05, 1.05, 1.05, 1.05;
//    max_val << 0.1, 0.1, 0.1, 0.1, 0.1, 0.1;
//    std::string dof = "disp";
//    cu::enumerator::SiteDoFEnumOptions input_options(dof, max_val, inc_val);
//    std::vector<std::pair<std::string, Structure>> enumerated_strucs =
//        cu::enumerator::enumerate_sitedofs(*bcc_struc_ptr, input_options);
//    EXPECT_EQ(enumerated_strucs.size(), 10);
//}
//
TEST_F(SiteDoFsTest, sym_axes_enumeration)
{
    Eigen::Vector3d ttt{0.99, -3.99, 0.5};
    std::cout << std::round(-3.99) << std::endl;
    std::cout << ttt.cast<int>() << std::endl;
    inc_val << 0.05, 0.05, 0.05, 0.05, 0.05, 0.05;
    max_val << 0.1, 0.1, 0.1, 0.1, 0.1, 0.1;
    std::string dof = "disp";
    cu::enumerator::SiteDoFEnumOptions input_options(dof, max_val, inc_val);
    input_options.sym_axes = true;
    std::vector<std::pair<std::string, Structure>> enumerated_strucs =
        cu::enumerator::enumerate_sitedofs(*bcc_struc_ptr, input_options);
    EXPECT_EQ(enumerated_strucs.size(), 165);
}
//
// TEST_F(SiteDoFsTest, sym_axes_single_irrep)
//{
//    inc_val << 0.05, 0.05, 0.05, 1.05, 1.05, 1.05;
//    max_val << 0.1, 0.1, 0.1, 0.1, 0.1, 0.1;
//    std::string dof = "disp";
//    cu::enumerator::SiteDoFEnumOptions input_options(dof, max_val, inc_val);
//    input_options.sym_axes = true;
//    std::vector<std::pair<std::string, Structure>> enumerated_strucs =
//        cu::enumerator::enumerate_sitedofs(*bcc_struc_ptr, input_options);
//    EXPECT_EQ(enumerated_strucs.size(), 10);
//}
//
// TEST_F(SiteDoFsTest, sym_axes_other_irrep)
//{
//    inc_val << 1.05, 1.05, 1.05, 0.05, 0.05, 0.05;
//    max_val << 0.1, 0.1, 0.1, 0.1, 0.1, 0.1;
//    std::string dof = "disp";
//    cu::enumerator::SiteDoFEnumOptions input_options(dof, max_val, inc_val);
//    input_options.sym_axes = true;
//    std::vector<std::pair<std::string, Structure>> enumerated_strucs =
//        cu::enumerator::enumerate_sitedofs(*bcc_struc_ptr, input_options);
//    EXPECT_EQ(enumerated_strucs.size(), 10);
//}
//
// TEST_F(SiteDoFsTest, max_nonzero)
//{
//    inc_val << 0.05, 0.05, 0.05, 0.05, 0.05, 0.05;
//    max_val << 0.1, 0.1, 0.1, 0.1, 0.1, 0.1;
//    std::string dof = "disp";
//    cu::enumerator::SiteDoFEnumOptions input_options(dof, max_val, inc_val);
//    input_options.max_nonzero = 3;
//    std::vector<std::pair<std::string, Structure>> enumerated_strucs =
//        cu::enumerator::enumerate_sitedofs(*bcc_struc_ptr, input_options);
//    std::cout << "=========" << std::endl;
//    EXPECT_EQ(enumerated_strucs.size(), 10);
//}
//
// TEST_F(SiteDoFsTest, min_max_non_zero)
//{
//    inc_val << 0.05, 0.05, 0.05, 0.05, 0.05, 0.05;
//    max_val << 0.1, 0.1, 0.1, 0.1, 0.1, 0.1;
//    std::string dof = "disp";
//    cu::enumerator::SiteDoFEnumOptions input_options(dof, max_val, inc_val);
//    input_options.min_nonzero = 3;
//    input_options.max_nonzero = 3;
//    std::vector<std::pair<std::string, Structure>> enumerated_strucs =
//        cu::enumerator::enumerate_sitedofs(*bcc_struc_ptr, input_options);
//    std::cout << "===============" << std::endl;
//    EXPECT_EQ(enumerated_strucs.size(), 10);
//}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
