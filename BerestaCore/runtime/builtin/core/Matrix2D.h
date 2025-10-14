//
// Created by Denis on 16.07.2025.
//

#ifndef BERESTALANGUAGE_MATRIX2D_H
#define BERESTALANGUAGE_MATRIX2D_H

#pragma once
#include <array>
#include <ostream>

struct Matrix2D
{
    std::array<double, 6> data;
    static Matrix2D identity();
    static Matrix2D build(double scale_x, double scale_y, double rotation_angle, double translate_x, double translate_y);
    [[nodiscard]] Matrix2D multiply(const Matrix2D& other) const;
    [[nodiscard]] Matrix2D inverse() const;
    [[nodiscard]] std::pair<double, double> transform_vertex(double x, double y) const;
};


#endif //BERESTALANGUAGE_MATRIX2D_H
