#pragma once

#include "math3d.h"

struct Transform
{
    vec3 scale;
    vec3 translate;
    quat rotate;

    mat4 compose() {
        mat4 result;
        result.setQuat(rotate);
        result.transpose();
        result.row(0) *= scale.x;
        result.row(1) *= scale.y;
        result.row(2) *= scale.z;
        result.row(3)  = vec4(translate, 1.0f);
        return result;
    }
    void decompose(mat4 const& m) {
        scale.x = m.row(0).length();
        scale.y = m.row(1).length();
        scale.z = m.row(2).length();
        translate = m.row(3).xyz();

        mat3 rmat = mat3( m.row(0).xyz().normalized(),
                          m.row(1).xyz().normalized(),
                          m.row(2).xyz().normalized() ).transposed();
        rotate.fromMat(rmat);
    }
};

