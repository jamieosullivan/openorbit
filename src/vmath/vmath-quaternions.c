/* 
    The contents of this file are subject to the Mozilla Public License
    Version 1.1 (the "License"); you may not use this file except in compliance
    with the License. You may obtain a copy of the License at
    http://www.mozilla.org/MPL/

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the
    License.

    The Original Code is the Open Orbit space flight simulator.

    The Initial Developer of the Original Code is Mattias Holm. Portions
    created by the Initial Developer are Copyright (C) 2006 the
    Initial Developer. All Rights Reserved.

    Contributor(s):
        Mattias Holm <mattias.holm(at)contra.nu>.

    Alternatively, the contents of this file may be used under the terms of
    either the GNU General Public License Version 2 or later (the "GPL"), or
    the GNU Lesser General Public License Version 2.1 or later (the "LGPL"), in
    which case the provisions of GPL or the LGPL License are applicable instead
    of those above. If you wish to allow use of your version of this file only
    under the terms of the GPL or the LGPL and not to allow others to use your
    version of this file under the MPL, indicate your decision by deleting the
    provisions above and replace  them with the notice and other provisions
    required by the GPL or the LGPL.  If you do not delete the provisions
    above, a recipient may use your version of this file under either the MPL,
    the GPL or the LGPL."
 */


#include <tgmath.h>

#include <vmath/vmath-types.h>
#include <vmath/vmath-constants.h>
#include <vmath/vmath-matvec.h>
#include <vmath/vmath-quaternions.h>
#include <assert.h>

 scalar_t
q_scalar(const quaternion_t q)
{
    return q.s.w;
}

vector_t
q_vector(const quaternion_t q)
{
    vector_t r;
    r.a[0] = q.s.x;
    r.a[1] = q.s.y;
    r.a[2] = q.s.z;
    r.a[3] = S_ZERO;
    return r;
}

void
q_m_convert(matrix_t *m, const quaternion_t q)
{
    scalar_t n = q_dot(q, q);
    scalar_t a = (n > S_ZERO) ? S_TWO / n : S_ZERO;
    
    scalar_t xa = QUAT_X(q.a)*a;
    scalar_t ya = QUAT_Y(q.a)*a;
    scalar_t za = QUAT_Z(q.a)*a;
    
    scalar_t xy = QUAT_X(q.a)*ya;
    scalar_t xz = QUAT_X(q.a)*za;
    scalar_t yz = QUAT_Y(q.a)*za;

    scalar_t wx = QUAT_W(q.a)*xa;
    scalar_t wy = QUAT_W(q.a)*ya;
    scalar_t wz = QUAT_W(q.a)*za;

    scalar_t xx = QUAT_X(q.a)*xa;
    scalar_t yy = QUAT_Y(q.a)*ya;
    scalar_t zz = QUAT_Z(q.a)*za;
    
    m->a[0][0] = S_ONE-(yy+zz); m->a[0][1] =        xy-wz;
    m->a[0][2] =        xz+wy;  m->a[0][3] = S_ZERO;
    
    m->a[1][0] =        xy+wz;  m->a[1][1] = S_ONE-(xx+zz);
    m->a[1][2] =        yz-wx;  m->a[1][3] = S_ZERO;
    
    m->a[2][0] =        xz-wy;  m->a[2][1] =        yz+wx;
    m->a[2][2] = S_ONE-(xx+yy); m->a[2][3] = S_ZERO;
    
    m->a[3][0] = S_ZERO;        m->a[3][1] = S_ZERO;
    m->a[3][2] = S_ZERO;        m->a[3][3] = S_ONE;
}
#if 0
void
q_m_convert(mat_arr_t m, const quat_arr_t q)
{
    scalar_t xy = QUAT_X(q)*QUAT_Y(q);
    scalar_t xz = QUAT_X(q)*QUAT_Z(q);
    scalar_t yz = QUAT_Y(q)*QUAT_Z(q);
    
    scalar_t wx = QUAT_W(q)*QUAT_X(q);
    scalar_t wy = QUAT_W(q)*QUAT_Y(q);
    scalar_t wz = QUAT_W(q)*QUAT_Z(q);
    
    scalar_t xx = QUAT_X(q)*QUAT_X(q);
    scalar_t yy = QUAT_Y(q)*QUAT_Y(q);
    scalar_t zz = QUAT_Z(q)*QUAT_Z(q);
    scalar_t ww = QUAT_W(q)*QUAT_W(q);

    
    m[0][0] = ww+xx-yy-zz;  m[0][1] = 2.0*(xy-wz);
    m[0][2] = 2.0*(xz+wy);  m[0][3] = 0.0;
    
    m[1][0] = 2.0*(xy+wz);  m[1][1] = ww-xx+yy-zz;
    m[1][2] = 2.0*(yz-wx);  m[1][3] = 0.0;
    
    m[2][0] = 2.0*(xz-wy);  m[2][1] = 2.0*(yz+wx);
    m[2][2] = ww-xx-yy+zz;  m[2][3] = 0.0;
    
    m[3][0] = 0.0;        m[3][1] = 0.0;
    m[3][2] = 0.0;        m[3][3] = ww+xx+yy+zz;
}
#endif


void
m_q_convert(quaternion_t q, matrix_t *m)
{
    scalar_t tr, s;
    tr = m->a[0][0] + m->a[1][1] + m->a[2][2];
    if (tr >= S_ZERO) {
        s = sqrt(tr+m->a[3][3]);
        q.s.w = s*S_POINT_FIVE;
        s = S_POINT_FIVE / s;
        q.s.x = (m->a[2][1] - m->a[1][2]) * s;
        q.s.y = (m->a[0][2] - m->a[2][0]) * s;
        q.s.z = (m->a[1][0] - m->a[0][1]) * s;
    } else {
        int h = 0;
        if (m->a[1][1] > m->a[0][0]) h = 1;
        if (m->a[2][2] > m->a[h][h]) h = 2;
        switch (h) {
#define CASE_MACRO(i,j,k,I,J,K)                                     \
        case I:                                                     \
            s = sqrt( (m->a[I][I] - (m->a[J][J]+m->a[K][K])) + m->a[3][3] );  \
            q.a[i] = s*S_POINT_FIVE;                                  \
            q.a[j] = (m->a[I][J] + m->a[J][I]) * s;                         \
            q.a[k] = (m->a[K][I] + m->a[I][K]) * s;                         \
            q.s.w = (m->a[K][J] - m->a[J][K]) * s;                       \
            break
        CASE_MACRO(Q_X, Q_Y, Q_Z, 0, 1, 2);
        CASE_MACRO(Q_Y, Q_Z, Q_X, 1, 2, 0);
        CASE_MACRO(Q_Z, Q_X, Q_Y, 2, 0, 1);
#undef CASE_MACRO
        default:
            assert(0);
        }
    }
    
    // QUERY: Is the last ref to z correct? 
    if (m->a[3][3] != S_ZERO) {
        s = S_ONE / sqrt(m->a[3][3]);
        q.s.x *= s; q.s.y *= s; q.s.z *= s; q.s.z *= s;
    }
}


quaternion_t
q_add(const quaternion_t a, const quaternion_t b)
{
 	return v_add(a, b);
}

quaternion_t
q_mul(const quaternion_t a, const quaternion_t b)
{
	quaternion_t r;
    QUAT_X(r.a) = QUAT_X(a.a)*QUAT_W(b.a) + QUAT_W(a.a)*QUAT_X(b.a)
   				+ QUAT_Y(a.a)*QUAT_Z(b.a) - QUAT_Z(a.a)*QUAT_Y(b.a);
    QUAT_Y(r.a) = QUAT_Y(a.a)*QUAT_W(b.a) + QUAT_W(a.a)*QUAT_Y(b.a)
				+ QUAT_Z(a.a)*QUAT_X(b.a) - QUAT_X(a.a)*QUAT_Z(b.a);
    QUAT_Z(r.a) = QUAT_Z(a.a)*QUAT_W(b.a) + QUAT_W(a.a)*QUAT_Z(b.a)
 				+ QUAT_X(a.a)*QUAT_Y(b.a) - QUAT_Y(a.a)*QUAT_X(b.a);
    QUAT_W(r.a) = QUAT_W(a.a)*QUAT_W(b.a) - QUAT_X(a.a)*QUAT_X(b.a)
				- QUAT_Y(a.a)*QUAT_Y(b.a) - QUAT_Z(a.a)*QUAT_Z(b.a);

    return r;
}

quaternion_t
q_s_div(const quaternion_t q, const scalar_t d)
{
	quaternion_t r;
    r.s.x = q.s.x / d;
    r.s.y = q.s.y / d;
    r.s.z = q.s.z / d;
    r.s.w = q.s.w / d;
    return r;
}

 scalar_t
q_dot(const quaternion_t a, const quaternion_t b)
{
    return v_dot(a, b);
}

vector_t
q_cross(const quaternion_t a, const quaternion_t b)
{
    return v_cross(a, b);
}

scalar_t
q_abs(const quaternion_t q)
{
    return v_abs(q);
}

quaternion_t
q_conj(const quaternion_t q)
{
	quaternion_t qp;
    QUAT_X(qp.a) = -QUAT_X(q.a);
    QUAT_Y(qp.a) = -QUAT_Y(q.a);
    QUAT_Z(qp.a) = -QUAT_Z(q.a);
    QUAT_W(qp.a) = QUAT_W(q.a);
}

quaternion_t
q_repr(const quaternion_t q)
{
	quaternion_t res;
    quaternion_t qp;
    qp = q_conj(q);
    scalar_t d = q_dot(q, q);
    res = q_s_div(qp, d);
	return res;
}

quaternion_t
q_div(const quaternion_t a, const quaternion_t b)
{
	quaternion_t res;
    quaternion_t br;
    br = q_repr(b);
    
	res = q_mul(a, br);
	return res;
}

quaternion_t
q_rotv(const vector_t axis, const angle_t alpha)
{
	quaternion_t q;
    scalar_t Omega = alpha * S_POINT_FIVE;
    scalar_t sin_Omega = sin(Omega);
    QUAT_X(q.a) = axis.a[0] * sin_Omega;
    QUAT_Y(q.a) = axis.a[1] * sin_Omega;
    QUAT_Z(q.a) = axis.a[2] * sin_Omega;
    QUAT_W(q.a) = cos(Omega);

	return q;
}
quaternion_t
q_rot(scalar_t x, scalar_t y, scalar_t z, scalar_t alpha)
{
	vector_t axis = {.s.x = x, .s.y = y, .s.z = z, .s.w = S_ONE};
	return q_rotv(axis, alpha);
}
quaternion_t
q_normalise(quaternion_t q)
{
    return v_normalise(q);
}