#ifndef TWIST_HH
#define TWIST_HH

#include "casmutils/xtal/structure.hpp"
#include <array>
#include <casmutils/sym/cartesian.hpp>
#include <casmutils/xtal/lattice.hpp>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include <casm/crystallography/Superlattice.hh>

namespace casmutils
{
namespace mush
{
/// Create a rotation matrix that rotates the given lattice by the specified angle
/// within the plane spanned by the ab vectors of the lattice.
/// The rotation should be applied to column vectors.
Eigen::Matrix3d make_twist_rotation_matrix(const xtal::Lattice& lat, double degrees);

/// Create a new lattice that has been rotated about the ab normal by the specified angle
xtal::Lattice make_twisted_lattice(const xtal::Lattice& lat, double degrees);

/// Returns the same lattice, but the c vector has been modified to be orthogonal to the
/// ab vectors. This will break periodicity, but not the thickness of the slab.
xtal::Lattice make_prismatic_lattice(const xtal::Lattice& lat);

/// Return the closest integer transformation matrix T that relates L to M such that
/// M=L*T. The first parameter returned is the integer transformation T, and the second
/// parameter is the error E that arises when rouding values to make T an integer matrix
std::pair<Eigen::Matrix3l, Eigen::Matrix3d> approximate_integer_transformation(const xtal::Lattice& L,
                                                                               const xtal::Lattice& M);

xtal::Lattice make_right_handed_by_ab_swap(const xtal::Lattice& left_handed_lattice);

/// Constructs Moire lattice for the given angle, and keeps information of the steps made along
/// the way. Before anything happens, the input lattice will be transformed to an aligned
/// prismatic one, breaking periodicity along the c axis.
/// The Moire lattice is likely not coincident with the smaller lattices. For constructing
/// periodic interference patterns, a small deformation must be introduced, accomplished
/// using ApproximantMoireLattice
struct MoireLattice
{
    enum class LATTICE
    {
        ALIGNED,
        ROTATED
    };

    using ZONE = LATTICE;

    MoireLattice(const xtal::Lattice& lat, double degrees);

    /// The lattice given at construction (not necessarily aligned)
    xtal::Lattice input_lattice;

    /// The degrees given at construction
    double input_degrees;

    /// The input lattice, after being transfromed to be prismatic and properly aligned along the xy plane
    xtal::Lattice aligned_lattice;
    xtal::Lattice reciprocal_aligned_lattice;

    /// The aligned lattice after being rotated by the input degrees
    xtal::Lattice rotated_lattice;
    xtal::Lattice reciprocal_rotated_lattice;

    /// Returns the real lattice of either ALIGNED or ROTATED
    const xtal::Lattice& real(LATTICE lat) const { return lat == LATTICE::ALIGNED ? aligned_lattice : rotated_lattice; }

    /// Returns the reciprocal lattice of either ALIGNED or ROTATED
    const xtal::Lattice& reciprocal(LATTICE lat) const
    {
        return lat == LATTICE::ALIGNED ? reciprocal_aligned_lattice : reciprocal_rotated_lattice;
    }

    /// Result from subtracting the reciprocal aligned and rotated lattices.
    /// There is no c vector difference, it's zero by construction.
    Eigen::Matrix2d full_reciprocal_difference;

    /// The reciprocal difference, brought into the first Brillouin zone of the
    /// aligned lattice
    Eigen::Matrix2d aligned_brillouin_zone_reciprocal_difference;

    /// The reciprocal difference, brought into the first Brillouin zone of the
    /// rotated lattice
    Eigen::Matrix2d rotated_brillouin_zone_reciprocal_difference;

    /// Moire lattice constructed from the reciprocal difference brought within the
    /// aligned lattice brillouin zone (this is equivalent to using the rotated
    /// lattice brillouin zone, but results in different lattice vectors)
    xtal::Lattice aligned_moire_lattice;

    /// Moire lattice constructed from the reciprocal difference brought within the
    /// rotated lattice brillouin zone (this is equivalent to using the aligned
    /// lattice brillouin zone, but results in different lattice vectors)
    xtal::Lattice rotated_moire_lattice;

    /// Returns the moire lattice of either ALIGNED or ROTATED Brillouin zone
    const xtal::Lattice& moire(ZONE bz) const
    {
        return bz == ZONE::ALIGNED ? aligned_moire_lattice : rotated_moire_lattice;
    }

    /// Maps the address of a Moire lattice to an array that specifies if its reciprocal vectors
    /// fall within the Brillouin zone of the other (rotated/aligned) lattice. For example
    /// brillouin_zone_overlap[LATTICE::ALIGNED][0] returns true if the reciprocal <a> vector
    /// of the aligned moire lattice falls within the first Brillouin zone of the rotated
    /// moire lattice
    std::unordered_map<LATTICE, std::array<bool, 2>> is_within_brillouin_zone_overlap;

    /// Brings the given vectors (columns in matrix) into the first voronoi (Wigner Seitz) cell
    /// of the provided lattice. Note that the function assumes vectors are in the xy plane
    ///(no z component) and that the lattice used for the Brillouin zone is prismatic
    /// and aligned.
    static Eigen::Matrix2d bring_vectors_into_voronoi(const Eigen::Matrix2d& col_vectors, const xtal::Lattice& lat);

    /// Given the a and b reciprocal Moire lattice vectors, transform them into a real
    /// Moire lattice, assigning the c vector as the last provided parameter.
    /// Vectors must have no z component.
    static xtal::Lattice make_moire_lattice_from_reciprocal_difference(const Eigen::Matrix2d diff,
                                                                       const Eigen::Vector3d& real_c_vector);

private:
    Eigen::Matrix2d calculate_reciprocal_difference() const;
    bool is_within_voronoi(const Eigen::Vector2d& v, const xtal::Lattice& lat) const;
};

/// Helper struct to convert an aligned and rotated lattice to a superlattice that's as close
/// as possible to the Moire lattice. The superlattices are calculated by finding the closest
/// integer transformation for each of the aligned and rotated lattices, then applying a deformation
/// to make them coincident. This class makes no checks whatsoever on the input parameters, and assumes
/// that you're giving something sensible that came from the MoireLattice class.
/// Members include unordered maps that use Lattice pointers as keys, the expected pointers are the
/// addresses of the aligned and rotated lattices given at construction.
/// All input values should be consistent for a single Brillouin zone.
struct MoireApproximant
{
    using LATTICE = MoireLattice::LATTICE;

    MoireApproximant(const xtal::Lattice& moire_lat,
                     const xtal::Lattice& aligned_lat,
                     const xtal::Lattice& rotated_lat);

    /// The moire lattice after straining it a bit to make the aligned and rotated lattices coincident
    xtal::Lattice approximate_moire_lattice;

    // TODO: Rename to "tiling unit"? There's too much "lattice" flying around.
    /// The aligned and rotated lattices with some strain introduced, such creating superlattices
    /// from them results in fully periodic Moire lattices
    std::unordered_map<LATTICE, xtal::Lattice> approximate_lattices;

    /// Integer transformation matrices that convert the approximate lattices into the Moire lattice.
    std::unordered_map<LATTICE, Eigen::Matrix3l> approximate_moire_integer_transformations;

    /// Error left over from forcing the true transformation matrix to be an integer matrix
    std::unordered_map<LATTICE, Eigen::Matrix3d> approximate_moire_integer_transformation_errors;

    /// Deformation introduced by making the approximations to introduce complete periodicity
    std::unordered_map<LATTICE, Eigen::Matrix3d> approximation_deformations;

private:
    xtal::Lattice default_lattice() { return xtal::Lattice(Eigen::Matrix3d::Zero()); }
};

/// Relevant information about the generated Moire lattice. This IS the class you're looking for:
/// * Requested Brillouin zone used to create it (aligned vs rotated)
/// * Requested layer (aligned vs rotated)
/// * The true Moire lattice (unlikely to be coincident)
/// * Transformation matrix of the true Moire lattice applied before creating the approximate Moire lattice
/// * The approximated Moire lattice for the requested Brillouin zone
/// * Tiling unit of the appriximated Moire lattice for the requested layer (deformed input lattice)
/// * Transformation matrix to go from the tiling unit to the approximated Moire lattice
/// * Rouding error that resulted from using the original tiling unit to get the tiling unit transformation matrix
/// * Deformation matrix required for the approximation
/// * Rotation portion of the deformation matrix
/// * Strain portion of the deformation
struct MoireLatticeReport
{
    using LATTICE = MoireLattice::LATTICE;
    using ZONE = MoireLattice::ZONE;

    MoireLatticeReport(ZONE zone,
                       LATTICE layer,
                       /* double angle, */
                       const xtal::Lattice& true_moire,
                       const Eigen::Matrix3l& true_moire_supercell_matrix,
                       const xtal::Lattice& approximate_moire,
                       const xtal::Lattice& approximate_tiling_unit,
                       const Eigen::Matrix3l& tiling_unit_supercell_matrix,
                       const Eigen::Matrix3d& tiling_unit_supercell_rounding_error,
                       const Eigen::Matrix3d& approximation_deformation)
        : zone(zone),
          layer(layer),
          /* angle(angle), */
          true_moire(true_moire),
          true_moire_supercell_matrix(true_moire_supercell_matrix),
          approximate_moire(approximate_moire),
          approximate_tiling_unit(approximate_tiling_unit),
          tiling_unit_supercell_matrix(tiling_unit_supercell_matrix),
          tiling_unit_supercell_rounding_error(tiling_unit_supercell_rounding_error),
          approximation_deformation(approximation_deformation)
    {
        /* std::tie(approximation_rotation, approximation_strain) =
         * xtal::polar_decomposition(approximation_deformation); */
    }

    /// Requested Brillouin zone used to create it (ALIGNED vs ROTATED)
    ZONE zone;
    /// Requested layer (ALIGNED vs ROTATED)
    LATTICE layer;
    /// Requested rotation angle
    /* double angle; */
    /// The true Moire lattice (unlikely to be coincident)
    xtal::Lattice true_moire;
    /// Transformation matrix of the true Moire lattice applied before creating the approximate Moire lattice
    Eigen::Matrix3l true_moire_supercell_matrix;
    /// The approximated Moire lattice for the requested Brillouin zone
    xtal::Lattice approximate_moire;
    /// Tiling unit of the appriximated Moire lattice for the requested layer (deformed input lattice)
    xtal::Lattice approximate_tiling_unit;
    /// Transformation matrix to go from the tiling unit to the approximated Moire lattice
    Eigen::Matrix3l tiling_unit_supercell_matrix;
    /// Rouding error left over from trying to squeeze the original tiling unit onto the true Moire
    /// supercell.
    Eigen::Matrix3d tiling_unit_supercell_rounding_error;
    /// Deformation matrix required for the approximation
    Eigen::Matrix3d approximation_deformation;
    /// Rotation portion of the deformation matrix (polar decomposition)
    /* Eigen::Matrix3d approximation_rotation; */
    /// Strain portion of the deformation (polar decomposition)
    /* Eigen::Matrix3d approximation_strain; */

    int num_moirons() const
    {
        return true_moire_supercell_matrix.determinant();
    }
};

/// Decomposes the defromation matrix into a rotation and deformation matrix.
/// Additional values like dilation and deviatoric strain that can be extracted
/// will only make sense if the deformation matrix is really just a 3d version
/// of a deformation that occurs on the xy plane.
struct DeformationReport
{
    /// Input as a 2d matrix, an extra dimension will be added to all the internal values
    /* DeformationReport(const Eigen::Matrix2d& deformation); */
    DeformationReport(const Eigen::Matrix3d& deformation);

    /// The deformation matrix used at construction
    Eigen::Matrix3d deformation;
    /// Rotation component of the deformation matrix
    Eigen::Matrix3d rotation;
    /// Strain component of the deformation matrix
    Eigen::Matrix3d strain;

    /// Extracted rotation angle of the rotation matrix
    double rotation_angle;

    /// Alternative strain metrics extracted from the strain matrix (U), defined as:
    /// \eta_1=\frac{1}{\sqrt{2}}(E_{11}+E_{22})
    /// \eta_2=\frac{1}{\sqrt{2}}(E_{11}-E_{22})
    /// \eta_3=\sqrt{2}(E_{12})
    /// where E=U-I
    std::array<double, 3> strain_metrics;

    /// Dilation strain, i.e. \eta_1 strain metric
    double dilation_strain;

    /// Deviatoric strain, defined as \sqrt{\eta_2^2+\eta_3^2}
    double deviatoric_strain;
};

/// Interface class to access the Moire lattice, which can be relative do different Brillouin zones,
/// as well as relevant deformations on each lattice. Basically just a wrapper class for everythin
/// in MoireLattice and MoireApproximant
class MoireApproximator
{
public:
    using LATTICE = MoireLattice::LATTICE;
    using ZONE = MoireLattice::ZONE;

private:
    using MoireScel = std::pair<MoireApproximant, Eigen::Matrix3l>;
    using MoireScelMap = std::map<int, std::vector<MoireScel>>;

    MoireLattice moire;

    /// Pointer acces to the true moire unit lattices via ZONE enum.
    /// Simply points into this->moire->alinged/rotated_moire_lattice members.
    std::unordered_map<ZONE, const xtal::Lattice*> moire_units;

    /// Approximations made using the Moire lattice generated using either the rotated
    /// or aligned Brillouin zone
    std::unordered_map<ZONE, MoireApproximant> moire_unit_approximants;

    /// Measures how badly the moire lattice is from landing on coindicent lattice sites
    double error_metric(const xtal::Lattice& moire, const xtal::Lattice& aligned, const xtal::Lattice& rotated) const;

    /// Creates the reduced cell, but keeps the c vector pointing the same direction.
    /// Will only work if the c vector doesn't need to be corrected to create the reduced cell.
    xtal::Lattice make_reduced_cell(const xtal::Lattice& lat) const;

    /// Translates maximum lattice sites to the supercell size of the Moire unit
    int maximum_lattice_sites_to_moire_supercell_size(ZONE bz, long max_lattice_sites) const;

    /// Maps the Moire supercell size (relative to the Moire unit, NOT the max lattice sites) to
    /// all the Moire supercells of that size, for both Brillouin zone constructions.
    /// This is basically a cache to avoid enumerating the same values twice, but also
    /// keeps the cells organized by size.
    std::unordered_map<ZONE, MoireScelMap> enumerated_moire_supercells;

    /// Calculates all Moire superlattices of a particular size (relative to Moire unit) and
    /// saves them
    void insert_moire_supercells_of_size(int num_moire_units);

    /// Helper construction of a MoireLatticeReport
    MoireLatticeReport
    make_report(ZONE bz, LATTICE layer, const std::pair<MoireApproximant, Eigen::Matrix3l>& data) const;

    /// Unroll the map that holds all the Moire supercells in order
    std::vector<MoireScel> all_candidates(ZONE bz) const;

    /// Return index of the best Moire supercell within the provided tolerance
    int best_candidate(const std::vector<MoireScel>& moire_supercells, double minimum_cost) const;

public:
    /// Give the original unrotated lattice and rotation angle.
    explicit MoireApproximator(const xtal::Lattice& input_lat, double degrees, long max_lattice_sites = 0);

    /// Calculates Moire supercells allowed to contain as many lattice sites as specified
    void expand(long max_lattice_sites);

    /// Returns collection of information for the requested brillouin zone and half bilayer,
    /// including the true Moire lattice, and the approximated one.
    /// The maximum lattice sites
    /// parameter will determine how many moirons are allowed to fit in the Moire lattice.
    /// If the values is less than the number of lattice sites that fit in a single moiron
    /// lattice, then the smallest possible Moire lattice is used.
    /// The minimum_error specifies a minimum improvement that must be achieved for larger
    /// Moire superlattices to be considered better.
    MoireLatticeReport best_smallest(ZONE brillouin, LATTICE layer, double minimum_cost = 1e-8) const;

    /// Return reports of every Moire supercell calculated so far
    std::vector<MoireLatticeReport> all(ZONE bz, LATTICE layer) const;

    /// Return reports of the best Moire supercell for each size
    std::vector<MoireLatticeReport> best_of_each_size(ZONE bz, LATTICE layer) const;

    /// The true Moire lattice, not necessarily commensurate
    const xtal::Lattice& true_moire(ZONE brillouin) const { return moire.moire(brillouin); }

    /// Calculates how many lattice sites are needed to construct the aligned and rotated
    /// smallest possible unit Moire layers
    long minimum_lattice_sites(ZONE bz) const;

};

/// Everything in the MoireLatticeReport, but also has a crystal structure, not just the lattice
struct MoireStructureReport : public MoireLatticeReport
{
    MoireStructureReport(const MoireLatticeReport& lattice_report, const xtal::Structure& approx_tiling_unit);
    xtal::Structure approximate_tiling_unit_structure;
    xtal::Structure approximate_moire_structure;
};

/// Generates slab superstructures that can be stacked together to create bilayers with Moire
/// patterns
class MoireStructureApproximator : MoireApproximator
{
public:
    using ZONE = MoireApproximator::ZONE;
    using LATTICE = MoireApproximator::LATTICE;
    using Structure = xtal::Structure;
    /* using MoireApproximator::degrees; */

    explicit MoireStructureApproximator(const Structure& slab_unit, double degrees, long max_lattice_sites = 0);

    MoireStructureReport best_smallest(ZONE brillouin, LATTICE layer, double minimum_cost = 1e-8) const;
    std::vector<MoireStructureReport> best_of_each_size(ZONE bz, LATTICE layer) const;

    using MoireApproximator::expand;
    using MoireApproximator::minimum_lattice_sites;

private:
    const Structure slab_unit;

    MoireStructureReport lattice_report_to_structure_report(const MoireLatticeReport& lat_report) const;
};
} // namespace mush
} // namespace casmutils

#endif
