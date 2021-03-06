/*
 Copyright 2009 Mattias Holm <mattias.holm(at)openorbit.org>

 This file is part of Open Orbit.

 Open Orbit is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Open Orbit is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Open Orbit.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>
#include "mass.h"
#if 0
void
testMassFuncs(void)
{
  pl_mass_t m;
  plMassSet(&m, 5.0,
            0.0, 0.0, 0.0,
            10.0, 20.0, 30.0,
            1.0, 2.0, 3.0);

  dMass dm;
  dMassSetParameters(&dm, 5.0,
                     0.0, 0.0, 0.0,
                     10.0, 20.0, 30.0,
                     1.0, 2.0, 3.0);

  printf("lm: %f : %f %f %f : %f %f %f\n",
         m.m,
         m.I[0][0], m.I[1][1], m.I[2][2],
         m.I[0][1], m.I[0][2], m.I[1][2]);
  printf("om: %f : %f %f %f : %f %f %f\n",
         dm.mass,
         dm.I[0*4+0], dm.I[1*4+1], dm.I[2*4+2],
         dm.I[0*4+1], dm.I[0*4+2], dm.I[1*4+2]);

  plMassTranslate(&m, 5.0, 10.0, 15.0);
  dMassTranslate(&dm, 5.0, 10.0, 15.0);

  printf("lmt: %f : %f %f %f : %f %f %f\n",
         m.m,
         m.I[0][0], m.I[1][1], m.I[2][2],
         m.I[0][1], m.I[0][2], m.I[1][2]);
  printf("omt: %f : %f %f %f : %f %f %f\n",
         dm.mass,
         dm.I[0*4+0], dm.I[1*4+1], dm.I[2*4+2],
         dm.I[0*4+1], dm.I[0*4+2], dm.I[1*4+2]);

  float3x3 mrot;
  mf3_rot(mrot, 1.0, 0.0, 0.0, 0.5);

  dReal rdr[16];
  rdr[0] = mrot[0].x;
  rdr[1] = mrot[0].y;
  rdr[2] = mrot[0].z;
  rdr[3] = 0.0;

  rdr[4] = mrot[1].x;
  rdr[5] = mrot[1].y;
  rdr[6] = mrot[1].z;
  rdr[7] = 0.0;

  rdr[8] = mrot[2].x;
  rdr[9] = mrot[2].y;
  rdr[10] = mrot[2].z;
  rdr[11] = 0.0;

  rdr[12] = mrot[3].x;
  rdr[13] = mrot[3].y;
  rdr[14] = mrot[3].z;
  rdr[15] = 0.0;

  dMassRotate(&dm, rdr);
  plMassRotateM(&m, mrot);

  printf("lmr: %f : %f %f %f : %f %f %f : %f %f %f\n",
         m.m,
         m.I[0][0], m.I[1][1], m.I[2][2],
         m.I[0][1], m.I[0][2], m.I[1][2],
         m.cog.x, m.cog.y, m.cog.z);
  printf("omr: %f : %f %f %f : %f %f %f : %f %f %f\n",
         dm.mass,
         dm.I[0*4+0], dm.I[1*4+1], dm.I[2*4+2],
         dm.I[0*4+1], dm.I[0*4+2], dm.I[1*4+2],
         dm.c[0], dm.c[1], dm.c[2]);

}
#endif
void
pl_mass_set(pl_mass_t *mo, float m,
          float cox, float coy, float coz,
          float ixx, float iyy, float izz,
          float ixy, float ixz, float iyz)
{
  memset(mo, 0, sizeof(pl_mass_t));

  mo->m = m;
  mo->In[0][0] = ixx;
  mo->In[1][1] = iyy;
  mo->In[2][2] = izz;
  mo->In[0][1] = ixy;
  mo->In[1][0] = ixy;
  mo->In[0][2] = ixz;
  mo->In[2][0] = ixz;
  mo->In[1][2] = iyz;
  mo->In[2][1] = iyz;

  mo->cog = vd3_set(cox, coy, coz);

  md3_inv2(mo->I_inv, mo->In);

  mo->minMass = 0.0f;
}

void
pl_mass_hollow_cylinder(pl_mass_t *mo, float m, float r)
{
  float i = m * r * r;
  pl_mass_set(mo, m,
            0.0f, 0.0f, 0.0f,
            i, i, i,
            0.0f, 0.0f, 0.0f);
}

void
pl_mass_solid_cylinder(pl_mass_t *mo, float m, float r, float h)
{
  float ixy = 1.0f / 12.0f * m * (3.0f * r * r + h * h);
  float iz = m * r * r / 2.0f;
  pl_mass_set(mo, m,
            0.0f, 0.0f, 0.0f,
            ixy, ixy, iz,
            0.0f, 0.0f, 0.0f);
}

void
pl_mass_walled_cylinder(pl_mass_t *mo, float m, float out_r, float in_r, float h)
{
  float ixy = 1.0f / 12.0f * m * (3.0f * (out_r * out_r + in_r * in_r) + h * h);
  float iz = 0.5 * m * (in_r*in_r + out_r * out_r);
  pl_mass_set(mo, m,
            0.0f, 0.0f, 0.0f,
            ixy, ixy, iz,
            0.0f, 0.0f, 0.0f);
}

void
pl_mass_solid_sphere(pl_mass_t *mo, float m, float r)
{
  float ixyz = 2.0f * m * r * r / 5.0f;
  pl_mass_set(mo, m,
            0.0f, 0.0f, 0.0f,
            ixyz, ixyz, ixyz,
            0.0f, 0.0f, 0.0f);
}

void
pl_mass_hollow_sphere(pl_mass_t *mo, float m, float r)
{
  float ixyz = 2.0f * m * r * r / 3.0f;
  pl_mass_set(mo, m,
            0.0f, 0.0f, 0.0f,
            ixyz, ixyz, ixyz,
            0.0f, 0.0f, 0.0f);
}

void
pl_mass_solid_cone(pl_mass_t *mo, float m, float r, float h)
{
  float iz = 3.0f / 10.0f * m * r * r;
  float ixy = 3.0f / 5.0f * m * (r * r / 4.0f + h * h);
  pl_mass_set(mo, m,
            0.0f, 0.0f, 0.0f,
            ixy, ixy, iz,
            0.0f, 0.0f, 0.0f);
}



void
pl_mass_translate(pl_mass_t *m, float dx, float dy, float dz)
{
  double3x3 re;
  md3_ident(re);
  double3 r = vd3_set(dx, dy, dz);
  double rdot = vd3_dot(r, r);
  re[0][0] = rdot;
  re[1][1] = rdot;
  re[2][2] = rdot;
  double3x3 rout;
  vd3_outprod(rout, r, r);
  double3x3 madj;
  md3_sub(madj, re, rout);

  double3x3 mtmp;
  md3_s_mul(mtmp, madj, m->m);
  md3_add2(m->In, mtmp);

  m->cog = vd3_add(m->cog, r);
  md3_inv2(m->I_inv, m->In);
}

void
pl_mass_rotate_m(pl_mass_t *m, const double3x3 rm)
{
  // We want to rotate the inertia tensor with
  // Ir = M * I0 * M^-1
  // Please verify

  double3x3 rmi;
  md3_inv2(rmi, rm);

  double3x3 mtmp;
  memset(mtmp, 0, sizeof(float3x3));

  md3_mul3(mtmp, m->In, rmi);
  md3_mul3(m->In, rm, mtmp);

  m->cog = md3_v_mul(rm, m->cog);
  md3_inv2(m->I_inv, m->In);
}

void
pl_mass_rotate_q(pl_mass_t *m, quatd_t q)
{
  double3x3 mat;
  qd_md3_convert(mat, q);

  pl_mass_rotate_m(m, mat);
}


void
pl_mass_add(pl_mass_t * restrict md, const pl_mass_t * restrict ms)
{
  float recip = 1.0f / (md->m + ms->m);

  double3 a = vd3_s_mul(md->cog, md->m);
  double3 b = vd3_s_mul(ms->cog, ms->m);
  double3 c = vd3_add(a, b);
  md->cog = vd3_s_mul(c, recip);

  md->m += ms->m;

  for (int i = 0 ; i < 3 ; ++ i) {
    md->In[i] = vd3_add(md->In[i], ms->In[i]);
  }
  md3_inv2(md->I_inv, md->In);
}

void
pl_mass_mod(pl_mass_t *m, float newMass)
{
  double s = newMass / m->m;
  m->m = newMass;

  for (int i = 0 ; i < 3 ; ++ i) {
    m->In[i] = vd3_s_mul(m->In[i], s);
    m->I_inv[i] = vd3_s_mul(m->I_inv[i], 1.0/s);
  }
}

float
pl_mass_red(pl_mass_t *m, float deltaMass)
{
  float oldMass = m->m;
  float newMass = oldMass - deltaMass;
  if (newMass < m->minMass) {
    newMass = m->minMass;
  }

  pl_mass_mod(m, newMass);
  return oldMass - newMass;
}

void
pl_mass_set_min(pl_mass_t *m, float minMass)
{
  m->minMass = minMass;
}
