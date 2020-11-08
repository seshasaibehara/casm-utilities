#ifndef UTILS_COORDINATE_HH
#define UTILS_COORDINATE_HH

#include <casm/crystallography/Coordinate.hh>
#include <casmutils/definitions.hpp>

namespace casmutils
{
namespace xtal
{

class Lattice;
namespace coordinate
{
/**
 * The rewrap version of Coordinate does *not* have a home Lattice,
 * and instead is always set to CART mode. Any routines that involve
 * a lattice require passing it as an argument.
 */

/// Returns cartesian coordinates when fractional coordinates are given
Eigen::Vector3d get_cartesian_coordinates_from_fractional(const Eigen::Vector3d& fractional_coordinate,
                                                          const Lattice& lat);

/// Returns fractional coordinates when cartesian coordinates are given
Eigen::Vector3d get_fractional_coordinates(const Eigen::Vector3d& cartesian_coordinate, const Lattice& lat);

/// Brings the given cartesian coordinate within the given lattice
Eigen::Vector3d bring_within_lattice(const Eigen::Vector3d& cartesian_coordinate, const Lattice& lat);

/// Brings the given cartesian coordinates within the wigner seitz cell of the given lattice
Eigen::Vector3d bring_within_wigner_seitz(const Eigen::Vector3d& cartesian_coordinate, const Lattice& lat);

} // namespace coordinate

// TODO: make this binary, not unary (same with all other comparators)
struct CoordinateEquals_f
{
    /// for casmutils::xtal::Coordinate
public:
    /// Determines whether test is equal to reference site with a tolerance
    CoordinateEquals_f(const Eigen::Vector3d& ref_coordinate, double tol);
    /// Returns true if other is equal to the Coordinate the comparator was constructed with
    bool operator()(const Eigen::Vector3d& other);

private:
    Eigen::Vector3d ref_coordinate;
    double tol;
};

} // namespace xtal
} // namespace casmutils

#endif
