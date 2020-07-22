from . import _mapping
from .structure import MappingReport
from ..xtal import Structure
from ..xtal import _xtal
from ..xtal import symmetry


def structure_score(mapping_data):
    """Calculates the individual lattice and basis scores from the ideal lattice,
    stretch tensor, and displacement matrix present in the provided MappingReport.
    Values are returned as a pair of [lattice_score, basis_score]

    Parameters
    ----------
    mapping_data : MappingReport

    Returns
    -------
    [float,float]

    """
    return _mapping.structure_score(mapping_data._pybind_value)


def map_structure(reference_struc, mapped_struc):
    """Using the default parameters for the StructureMapper, return a MappingReport report
    that maps one structure onto the other

    Parameters
    ----------
    reference_struc : xtal.Structure
    mapped_struc : xtal.Structure

    Returns
    -------
    MappingReport

    """
    return [
        MappingReport(r) for r in _mapping.map_structure(
            reference_struc._pybind_value, mapped_struc._pybind_value)
    ]


def symmetry_preserving_mapping_report(mapping_report, structure, tol):
    """Returns the symmetry preserving mapping report

    Parameters
    ----------
    mapping_report : MappingReport
    structure : xtal.Structure
    tol : float

    Returns
    -------
    MappingReport

    """
    pybind_factor_group = _xtal.make_factor_group(structure._pybind_value, tol)
    perm_rep_factor_group = symmetry.make_perm_rep_factor_group(structure, tol)

    return MappingReport(_mapping.symmetry_preserving_mapping_report(mapping_report._pybind_value, pybind_factor_group, perm_rep_factor_group))


def symmetry_breaking_mapping_report(mapping_report, structure, tol):
    """Returns the symmetry breaking mapping report

    Parameters
    ----------
    mapping_report : MappingReport
    structure : xtal.Structure
    tol : float

    Returns
    -------
    MappingReport

    """
    pybind_factor_group = _xtal.make_factor_group(structure._pybind_value, tol)
    perm_rep_factor_group = symmetry.make_perm_rep_factor_group(structure, tol)

    return MappingReport(_mapping.symmetry_breaking_mapping_report(mapping_report._pybind_value, pybind_factor_group, perm_rep_factor_group))
