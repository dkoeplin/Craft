#include "Sphere.h"

#include <cmath>

#include "craft/support/matrix.h"
#include "craft/util/Util.h"

int _make_sphere(float *data, float r, int detail, float *a, float *b, float *c, float *ta, float *tb, float *tc) {
    if (detail == 0) {
        float *d = data;
        *(d++) = a[0] * r;
        *(d++) = a[1] * r;
        *(d++) = a[2] * r;
        *(d++) = a[0];
        *(d++) = a[1];
        *(d++) = a[2];
        *(d++) = ta[0];
        *(d++) = ta[1];
        *(d++) = b[0] * r;
        *(d++) = b[1] * r;
        *(d++) = b[2] * r;
        *(d++) = b[0];
        *(d++) = b[1];
        *(d++) = b[2];
        *(d++) = tb[0];
        *(d++) = tb[1];
        *(d++) = c[0] * r;
        *(d++) = c[1] * r;
        *(d++) = c[2] * r;
        *(d++) = c[0];
        *(d++) = c[1];
        *(d++) = c[2];
        *(d++) = tc[0];
        *(d++) = tc[1];
        return 1;
    } else {
        float ab[3], ac[3], bc[3];
        for (int i = 0; i < 3; i++) {
            ab[i] = (a[i] + b[i]) / 2;
            ac[i] = (a[i] + c[i]) / 2;
            bc[i] = (b[i] + c[i]) / 2;
        }
        normalize(ab + 0, ab + 1, ab + 2);
        normalize(ac + 0, ac + 1, ac + 2);
        normalize(bc + 0, bc + 1, bc + 2);
        float tab[2], tac[2], tbc[2];
        tab[0] = 0;
        tab[1] = 1 - acosf(ab[1]) / PI;
        tac[0] = 0;
        tac[1] = 1 - acosf(ac[1]) / PI;
        tbc[0] = 0;
        tbc[1] = 1 - acosf(bc[1]) / PI;
        int total = 0;
        int n;
        n = _make_sphere(data, r, detail - 1, a, ab, ac, ta, tab, tac);
        total += n;
        data += n * 24;
        n = _make_sphere(data, r, detail - 1, b, bc, ab, tb, tbc, tab);
        total += n;
        data += n * 24;
        n = _make_sphere(data, r, detail - 1, c, ac, bc, tc, tac, tbc);
        total += n;
        data += n * 24;
        n = _make_sphere(data, r, detail - 1, ab, bc, ac, tab, tbc, tac);
        total += n;
        data += n * 24;
        return total;
    }
}

void make_sphere(float *data, float r, int detail) {
    // detail, triangles, floats
    // 0, 8, 192
    // 1, 32, 768
    // 2, 128, 3072
    // 3, 512, 12288
    // 4, 2048, 49152
    // 5, 8192, 196608
    // 6, 32768, 786432
    // 7, 131072, 3145728
    static int indices[8][3] = {{4, 3, 0}, {1, 4, 0}, {3, 4, 5}, {4, 1, 5}, {0, 3, 2}, {0, 2, 1}, {5, 2, 3}, {5, 1, 2}};
    static float positions[6][3] = {{0, 0, -1}, {1, 0, 0}, {0, -1, 0}, {-1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
    static float uvs[6][3] = {{0, 0.5}, {0, 0.5}, {0, 0}, {0, 0.5}, {0, 1}, {0, 0.5}};
    int total = 0;
    for (auto &index : indices) {
        int n = _make_sphere(data, r, detail, positions[index[0]], positions[index[1]], positions[index[2]],
                             uvs[index[0]], uvs[index[1]], uvs[index[2]]);
        total += n;
        data += n * 24;
    }
}
