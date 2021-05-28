from . import _xtal
from ..sym.cart import CartOp
from .lattice import *
from .structure import *
from .site import SitePeriodicEquals


def make_point_group(lattice, tol):
    """Calculate all symmetry operations that map the given
    lattice onto itself

    Parameters
    ----------
    lattice : Lattice
    tol : float

    Returns
    -------
    list(cu.sym.CartOp)

    """
    return [CartOp(op) for op in _xtal.make_point_group(lattice, tol)]


def make_factor_group(structure, tol):
    """Calculate all symmetry operations that map the given
    structure onto itself

    Parameters
    ----------
    structure : Structure
    tol : float

    Returns
    -------
    list(cu.sym.CartOp)

    """
    return [
        CartOp(op)
        for op in _xtal.make_factor_group(structure._pybind_value, tol)
    ]


def make_permutation_representation_factor_group(structure, tol):
    """Calculates all the symmetry operations in their permutation
    representation that map the given structure onto itself

    Example: Consider a structure with 3 sites (named 0,1,2) and a symmetry
    operation that permutates site 0 with site 1, and site 2 remains
    the same. Then this function returns a list [1,0,2].
    Interpretation of [1,0,2]: The index of the array are the actual sites.
    And the value of the array is the site number at which the actual site is now at.

    Parameters
    ----------
    structure : cu.xtal.Structure
    tol : float

    Returns
    -------
    list[list[int]]

    """
    factor_group = make_factor_group(structure, tol)
    basis_sites = structure.basis_sites()
    perm_rep_factor_group = []

    for sym_op in factor_group:
        perm_rep_sym_op = [0] * len(basis_sites)
        transformed_sites = [sym_op * site for site in basis_sites]

        for i, site in enumerate(basis_sites):
            for j, transformed_site in enumerate(transformed_sites):
                site.set_compare_method(SitePeriodicEquals,
                                        structure.lattice(), tol)
                if site == transformed_site:
                    perm_rep_sym_op[i] = j
                    break
                if j == len(transformed_sites) - 1:
                    raise Exception(
                        "Oopsie! No permutation represention found. Check the sym_op and structure again"
                    )

        perm_rep_factor_group.append(perm_rep_sym_op)
    return perm_rep_factor_group


def symmetrize(lattice_or_structure, enforced_group):
    """Gives a symmetrized version of the input lattice/structure such 
    that it obeys the given enforced symmetry group

    :lattice_or_structure: Lattice or Structure
    :enforced_group: list(cu.sym.CartOp)
    :returns: Lattice or Structure

    """
    if isinstance(lattice_or_structure, Lattice):
        return _xtal._symmetrize_lattice(lattice_or_structure, enforced_group)
    elif isinstance(lattice_or_structure, Structure):
        return Structure._from_pybind(
            _xtal._symmetrize_structure(lattice_or_structure._pybind_value,
                                        enforced_group))
    else:
        raise ValueError("symmetrize only works on Structure or Lattice types")
