#ifndef TWIST3D_HPP
#define TWIST3D_HPP

#include <casmutils/xtal/lattice.hpp>
#include <unordered_map>

namespace casmutils
{

/// TODO: Need to add condition where if the input lattice is prismatic it should use the 2d variant
/// TODO: checking for infinite moire lattices
/// TODO: Need to build tests and check if what you wrote make sense
class MoireLattice3d
{
private:
    Eigen::Matrix3d m_transformation_matrix;

    xtal::Lattice m_input_lattice;
    xtal::Lattice m_rotated_lattice;

    xtal::Lattice m_input_reciprocal_lattice;
    xtal::Lattice m_rotated_reciprocal_lattice;

    Eigen::Matrix3d m_reciprocal_difference;
    Eigen::Matrix3d m_reciprocal_difference_within_input_lat_bz;
    Eigen::Matrix3d m_reciprocal_difference_within_rotated_lat_bz;

    xtal::Lattice m_input_moire_lattice;
    xtal::Lattice m_rotated_moire_lattice;

    Eigen::Matrix3d calculate_reciprocal_lattice_difference() const;

public:
    enum class LATTICE
    {
        INPUT,
        ROTATED
    };

    using ZONE = LATTICE;

    MoireLattice3d(const xtal::Lattice& lattice, const Eigen::Matrix3d& transformation_matrix);

    MoireLattice3d(const xtal::Lattice& lattice,
                   const Eigen::Vector3d& axis_of_rotation,
                   const double angle_of_rotation);

    xtal::Lattice real_lattice(LATTICE lat) const
    {
        return lat == LATTICE::INPUT ? this->m_input_lattice : this->m_rotated_lattice;
    }

    xtal::Lattice reciprocal_lattice(LATTICE lat) const
    {
        return lat == LATTICE::INPUT ? this->m_input_reciprocal_lattice : this->m_rotated_reciprocal_lattice;
    }

    const xtal::Lattice& moire_lattice(ZONE bz) const
    {
        return bz == ZONE::INPUT ? this->m_input_moire_lattice : this->m_rotated_moire_lattice;
    }

    Eigen::Matrix3d transformation_matrix() const { return this->m_transformation_matrix; }

    Eigen::Matrix3d reciprocal_difference() const { return this->m_reciprocal_difference; }

    Eigen::Matrix3d reciprocal_difference_within_input_lat_bz() const
    {
        return this->m_reciprocal_difference_within_input_lat_bz;
    }

    Eigen::Matrix3d reciprocal_difference_within_rotated_lat_bz() const
    {
        return this->m_reciprocal_difference_within_rotated_lat_bz;
    }

    static Eigen::Matrix3d bring_vectors_within_wigner_sietz(const Eigen::Matrix3d& col_vectors,
                                                             const xtal::Lattice& lat);
};

/// Uses the same approximate integer transformation function that John used to find the superlattice information
class MoireApproximant3d
{
public:
    using LATTICE = MoireLattice3d::LATTICE;

    MoireApproximant3d(const xtal::Lattice& moire_lat,
                       const xtal::Lattice& original_lat,
                       const xtal::Lattice& rotated_lat);

    xtal::Lattice approximate_moire_lattice() const { return this->m_approximate_moire_lattice; }

    /// Returns the approxiate moire lattice that fits
    std::unordered_map<LATTICE, xtal::Lattice> approximate_lattices() const { return this->m_approximate_lattices; }
    std::unordered_map<LATTICE, Eigen::Matrix3l> approximate_moire_integer_transformations() const
    {
        return this->m_approximate_moire_integer_transformations;
    }
    std::unordered_map<LATTICE, Eigen::Matrix3d> approximate_moire_integer_transformations_errors() const
    {
        return this->m_approximate_moire_integer_transformations_errors;
    }
    std::unordered_map<LATTICE, Eigen::Matrix3d> approximate_deformations() const
    {
        return this->m_approximate_deformations;
    }

private:
    xtal::Lattice m_approximate_moire_lattice;
    std::unordered_map<LATTICE, xtal::Lattice> m_approximate_lattices;
    std::unordered_map<LATTICE, Eigen::Matrix3l> m_approximate_moire_integer_transformations;
    std::unordered_map<LATTICE, Eigen::Matrix3d> m_approximate_moire_integer_transformations_errors;
    std::unordered_map<LATTICE, Eigen::Matrix3d> m_approximate_deformations;
};

} // namespace casmutils

#endif /* ifndef TWIST3D_HPP */
