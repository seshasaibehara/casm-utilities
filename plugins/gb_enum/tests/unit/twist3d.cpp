#include <casmutils/xtal/lattice.hpp>
#include <gtest/gtest.h>
#include <twist3d.hpp>

namespace cu = casmutils;

class GBEnumTest : public testing::Test
{
protected:
    void SetUp() override {}
};

TEST_F(GBEnumTest, Twist3d)
{

    Eigen::Matrix3d col_mat;
    col_mat = Eigen::Matrix3d::Random();
    std::cout << col_mat << std::endl;
//    col_mat << 0, 0.5, 0.5,
//            0.5, 0, 0.5,
//            0.5, 0.5, 0;
    std::cout << "im here" << std::endl;

    cu::MoireLattice3d p(cu::xtal::Lattice(col_mat), Eigen::Vector3d(1, 1, 1), 60);
    auto in = cu::MoireLattice3d::LATTICE::INPUT;
    auto ro = cu::MoireLattice3d::LATTICE::ROTATED;

    // std::cout << p.real_lattice(in).column_vector_matrix() << std::endl;
    // std::cout << "----" << std::endl;
    // std::cout << "----" << std::endl;
    // std::cout << p.real_lattice(ro).column_vector_matrix() << std::endl;
    // std::cout << "----" << std::endl;
    // std::cout << p.transformation_matrix() << std::endl;
    // std::cout << "----" << std::endl;
    // std::cout << p.reciprocal_lattice(in).column_vector_matrix() << std::endl;
    // std::cout << "---" << std::endl;

    // std::cout << p.reciprocal_lattice(ro).column_vector_matrix() << std::endl;
    // std::cout << "---" << std::endl;
    std::cout << "----" << std::endl;
     std::cout << p.reciprocal_difference() << std::endl;
     std::cout << "---" << std::endl;
    // std::cout << p.reciprocal_difference_within_input_lat_bz() << std::endl;
    // std::cout << "---" << std::endl;
    // std::cout << p.reciprocal_difference_within_rotated_lat_bz() << std::endl;
    // std::cout << "---" << std::endl;
   // std::cout << p.reciprocal_difference_within_input_lat_bz() << std::endl;
    auto cp = p.reciprocal_difference().col(0).cross(
        p.reciprocal_difference().col(1));
    std::cout << cp << std::endl;
    auto dp = cp.dot(p.reciprocal_difference().col(2));

    std::cout << dp << std::endl;
    auto xx = cu::xtal::make_reciprocal(cu::xtal::Lattice(p.reciprocal_difference()));
    std::cout << xx.column_vector_matrix() << std::endl;
    // std::cout << pp.moire_lattice(in).column_vector_matrix() << std::endl;
    // std::cout << "---" << std::endl;
    // std::cout << pp.moire_lattice(ro).column_vector_matrix() << std::endl;
}

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
