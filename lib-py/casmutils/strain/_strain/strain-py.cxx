#include <casmutils/strain/strain.hpp>

#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>

//******************************************************************************************************//
//******************************************************************************************************//

namespace casmutils
{
}

namespace wrappy
{

using namespace casmutils;
PYBIND11_MODULE(_strain, m)
{
    using namespace pybind11;

    m.doc() = "Testsss";

    m.def("tdp", &strain::deformation_tensor_to_strain_metric);
}
} // namespace wrappy
