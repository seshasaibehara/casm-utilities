from __future__ import absolute_import

from . import coordinate
from .coordinate import Coordinate
from .coordinate import MutableCoordinate
from .lattice import *
from .site import *
from .structure import *
from .symmetry import *
from .globaldef import *
from ._xtal import make_niggli as _make_niggli
from ._xtal import make_superstructure as _make_superstructure
from ._xtal import make_primitive as _make_primitive
from ._xtal import _deformation_tensor_to_metric

# from .single_block_wadsley_roth import *


def extra_function(self):
    print("I'm the extra function!")


def make_niggli(input_value):
    """Returns the niggli version of input_value. Type checks the argument
    to return a value with the same type.

    :input_value: casmutils.xtal.lattice.Lattice or casmutils.xtal.structure.Structure
    :returns: casmutils.xtal.lattice.Lattice or casmutils.xtal.structure.Structure 

    """
    if isinstance(input_value, Structure):
        return Structure._from_pybind(_make_niggli(input_value._pybind_value))
    elif isinstance(input_value, Lattice):
        return Lattice._from_pybind(_make_niggli(input_value))
    else:
        raise ValueError


def make_superstructure(structure, transformation_matrix):
    """Returns the superstructure of the given structure,
    scaling the lattice by the given transformation matrix

    :structure: casmutils.xtal.structure.Structure
    :transformation_matrix: np.array(int32[3,3])
    :returns: casmutils.xtal.structure.Structure

    """
    return Structure._from_pybind(
        _make_superstructure(structure._pybind_value, transformation_matrix))


def make_primitive(structure):
    """Returns the primitive version of the given structure.

    :structure: casmutils.xtal.structure.Structure
    :returns: casmutils.xtal.structure.Structure

    """
    return Structure._from_pybind(_make_primitive(structure._pybind_value))


def deformation_tensor_to_metric(metric, deformation_tensor):
    """Converts the given deformation tensor to a strain metric of your choice

    Parameters
    ----------
    metric : string
        Allowed metric values are 'GL' (Green-Lagrange), 'B' (BIOT), 'H'(HENCKY) and 'EA' (Euler-Almansi).
        Metric values can either be capital letters or small letters. Only the first letter in the provided metric 
        string is considered to compare it with the allowed values. For example, you can provide 'gree' as the string 
        and it selects Green-Lagrange

    deformation_tensor : np.array(float32[3,3])
        Deformation tensor that needs to be converted to a metric tensor of your choice
    
    Returns
    -------
    np.array(float32[3,3])
        Correspoding metric tensor

    """
    return _deformation_tensor_to_metric(metric, deformation_tensor)

Coordinate.extra_function = extra_function
