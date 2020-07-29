#include <casmutils/strain/strain_enum.hpp>
#include <casmutils/xtal/structure.hpp>

namespace cu = casmutils;

int main(int argc, char* argv[])
{
    cu::xtal::Structure test_struc = cu::xtal::Structure::from_poscar("PRIM");
    Eigen::VectorXd inc_val(6), max_val(6), min_val(6);
    inc_val << 0.05, 0.05, 0.05, 0.05, 0.05, 0.05;
    max_val << 0.1, 0.1, 0.1, 0.1, 0.1, 0.1;
    min_val << -0.05, -0.05, -0.05, -0.05, -0.05, -0.05;

    cu::strain::StrainEnumOptions input_options(max_val, inc_val);
    input_options.sym_axes = true;
    input_options.min_val = min_val;

    cu::strain::enumerate_strain(test_struc, input_options);

    return 0;
}
