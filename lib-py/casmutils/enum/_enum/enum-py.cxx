#include "casmutils/enum/strain_enum.hpp"
#include "casmutils/xtal/structure.hpp"

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
PYBIND11_MODULE(_enum, m)
{
    using namespace pybind11;

    m.doc() = "Testsss";

    {
        class_<enumerator::StrainEnumOptions>(m, "StrainEnumOptions")
            .def(init<const Eigen::VectorXd&, const Eigen::VectorXd&>())
            .def_readwrite("strain_metric", &enumerator::StrainEnumOptions::strain_metric)
            .def_readwrite("axes", &enumerator::StrainEnumOptions::axes)
            .def_readwrite("min_val", &enumerator::StrainEnumOptions::min_val)
            .def_readwrite("max_val", &enumerator::StrainEnumOptions::max_val)
            .def_readwrite("inc_val", &enumerator::StrainEnumOptions::inc_val)
            .def_readwrite("trim_corners", &enumerator::StrainEnumOptions::trim_corners)
            .def_readwrite("sym_axes", &enumerator::StrainEnumOptions::sym_axes);
    }

    m.def("enumerate_strain", &enumerator::enumerate_strain);
}
} // namespace wrappy
