//
// Created by Denis on 16.07.2025.
//

#include "Matrix2D.h"
#include <cmath>

Matrix2D Matrix2D::identity() {return Matrix2D{{1, 0, 0, 1, 0, 0}};}

Matrix2D Matrix2D::build(double sx, double sy, double rot_deg, double tx, double ty)
{
    double rad = rot_deg * (M_PI / 180.0);
    double cos_r = std::cos(rad);
    double sin_r = std::sin(rad);
    return Matrix2D{{
                            cos_r * sx, sin_r * sx,
                            -sin_r * sy, cos_r * sy,
                            tx, ty
                    }};
}

Matrix2D Matrix2D::multiply(const Matrix2D& o) const
{
    const auto& m1 = data;
    const auto& m2 = o.data;

    return Matrix2D{{
                            m1[0] * m2[0] + m1[2] * m2[1],
                            m1[1] * m2[0] + m1[3] * m2[1],

                            m1[0] * m2[2] + m1[2] * m2[3],
                            m1[1] * m2[2] + m1[3] * m2[3],

                            m1[0] * m2[4] + m1[2] * m2[5] + m1[4],
                            m1[1] * m2[4] + m1[3] * m2[5] + m1[5]
                    }};
}

Matrix2D Matrix2D::inverse() const
{
    const auto& m = data;
    double det = m[0] * m[3] - m[1] * m[2];
    if(std::abs(det) < 1e-8) {throw std::runtime_error("Matrix is not invertible");}
    double inv_det = 1.0 / det;
    return Matrix2D{{
                            m[3] * inv_det,
                            -m[1] * inv_det,
                            -m[2] * inv_det,
                            m[0] * inv_det,
                            (m[2] * m[5] - m[3] * m[4]) * inv_det,
                            (m[1] * m[4] - m[0] * m[5]) * inv_det
                    }};
}

std::pair<double, double> Matrix2D::transform_vertex(double x, double y) const
{
    const auto& m = data;
    return {
            x * m[0] + y * m[2] + m[4],
            x * m[1] + y * m[3] + m[5]
    };
}

Matrix2D current_matrix = Matrix2D::identity();
