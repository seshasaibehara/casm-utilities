#include <casmutils/mush/twist.hpp>
#include <casmutils/xtal/coordinate.hpp>
#include <math.h>
#include <twist3d.hpp>

namespace casmutils
{
MoireLattice3d::MoireLattice3d(const xtal::Lattice& lattice, const Eigen::Matrix3d& transformation_matrix)
    : m_input_lattice(lattice),
      m_transformation_matrix(transformation_matrix),
      m_input_reciprocal_lattice(xtal::make_reciprocal(lattice)),
      m_rotated_lattice(xtal::Lattice(transformation_matrix * lattice.column_vector_matrix())),
      m_rotated_reciprocal_lattice(xtal::make_reciprocal(m_rotated_lattice)),
      m_reciprocal_difference(this->calculate_reciprocal_lattice_difference()),
      m_reciprocal_difference_within_input_lat_bz(
          this->bring_vectors_within_wigner_sietz(m_reciprocal_difference, m_input_reciprocal_lattice)),
      m_reciprocal_difference_within_rotated_lat_bz(
          this->bring_vectors_within_wigner_sietz(m_reciprocal_difference, m_rotated_reciprocal_lattice)),
      m_input_moire_lattice(xtal::make_reciprocal(xtal::Lattice(m_reciprocal_difference_within_input_lat_bz))),
      m_rotated_moire_lattice(xtal::make_reciprocal(xtal::Lattice(m_reciprocal_difference_within_rotated_lat_bz)))
{
}

MoireLattice3d::MoireLattice3d(const xtal::Lattice& lattice,
                               const Eigen::Vector3d& axis_of_rotation,
                               const double angle_of_rotation)
    : MoireLattice3d::MoireLattice3d(
          lattice, Eigen::AngleAxisd{(M_PI / 180) * angle_of_rotation, axis_of_rotation.normalized()}.matrix())
{
}

Eigen::Matrix3d MoireLattice3d::calculate_reciprocal_lattice_difference() const
{
    return this->m_rotated_reciprocal_lattice.column_vector_matrix() -
           this->m_input_reciprocal_lattice.column_vector_matrix();
}

Eigen::Matrix3d MoireLattice3d::bring_vectors_within_wigner_sietz(const Eigen::Matrix3d& col_vectors,
                                                                  const xtal::Lattice& lat)
{
    Eigen::Matrix3d result;
    for (int i = 0; i < 3; ++i)
    {
        result.col(i) = xtal::bring_within_wigner_seitz(col_vectors.col(i), lat);
    }
    return result;
}

MoireApproximant3d::MoireApproximant3d(const xtal::Lattice& moire_lat,
                                       const xtal::Lattice& original_lat,
                                       const xtal::Lattice& rotated_lat)
    : m_approximate_moire_lattice(xtal::Lattice(Eigen::Matrix3d::Zero()))
{
    std::unordered_map<LATTICE, const xtal::Lattice*> real_lattices;
    real_lattices.emplace(LATTICE::INPUT, &original_lat);
    real_lattices.emplace(LATTICE::ROTATED, &rotated_lat);

    // Figure out the integer transformations for both the original lattice and rotated lattice
    for (LATTICE lat : {LATTICE::INPUT, LATTICE::ROTATED})
    {
        const auto& M = moire_lat;
        const auto& L = *real_lattices[lat];

        std::tie(m_approximate_moire_integer_transformations[lat],
                 m_approximate_moire_integer_transformations_errors[lat]) =
            mush::approximate_integer_transformation(L, M);
    }

    // Make supercells of both the original lattice and rotated lattice using the approximate integer transformation
    // that you found above
    auto original_S =
        xtal::make_superlattice(original_lat, m_approximate_moire_integer_transformations[LATTICE::INPUT].cast<int>());
    auto rotated_S =
        xtal::make_superlattice(rotated_lat, m_approximate_moire_integer_transformations[LATTICE::ROTATED].cast<int>());

    // Construct an average of the superlattices computed above
    Eigen::Matrix3d S_bar = (original_S.column_vector_matrix() + rotated_S.column_vector_matrix()) / 2.0;

    m_approximate_moire_lattice = xtal::Lattice(S_bar);

    Eigen::Matrix3d original_F = S_bar * original_S.column_vector_matrix().inverse();
    Eigen::Matrix3d rotated_F = S_bar * rotated_S.column_vector_matrix().inverse();
    m_approximate_deformations[LATTICE::INPUT] = original_F;
    m_approximate_deformations[LATTICE::ROTATED] = rotated_F;

    // Determine the tiling units of input and rotated lattices which give out S_bar
    for (LATTICE lat : {LATTICE::INPUT, LATTICE::ROTATED})
    {
        const Eigen::Matrix3d& L = real_lattices[lat]->column_vector_matrix();
        Eigen::Matrix3d L_prime = S_bar * m_approximate_moire_integer_transformations[lat].cast<double>().inverse();
        m_approximate_lattices.emplace(lat, L_prime);
    }
}

} // namespace casmutils
