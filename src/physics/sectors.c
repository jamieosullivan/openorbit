/*
  Copyright 2009 Mattias Holm <mattias.holm(at)openorbit.org>

  This file is part of Open Orbit.

  Open Orbit is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Open Orbit is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with Open Orbit.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "physics.h"

#define OFFS_X(c) (((float*)&(c)->offs)[0])
#define OFFS_Y(c) (((float*)&(c)->offs)[1])
#define OFFS_Z(c) (((float*)&(c)->offs)[2])

#define SEG_X(c) (((int32_t*)&(c)->seg)[0])
#define SEG_Y(c) (((int32_t*)&(c)->seg)[1])
#define SEG_Z(c) (((int32_t*)&(c)->seg)[2])


PLocttree*
plOcttree(double width, int levels)
{
  PLocttree *octtree = malloc(sizeof(PLocttree));
  octtree->level = levels;
  return octtree;
}

void
plLwcNormalise(PLlwcoord *coord)
{
  if (fabsf(OFFS_X(coord)) >= PL_SEGMENT_LEN) {
    SEG_X(coord) += (int16_t) (OFFS_X(coord) / PL_SEGMENT_LEN);
    OFFS_X(coord) = fmodf(OFFS_X(coord), PL_SEGMENT_LEN);
  }
  if (fabsf(OFFS_Y(coord)) >= PL_SEGMENT_LEN) {
    SEG_Y(coord) += (int16_t) (OFFS_Y(coord) / PL_SEGMENT_LEN);
    OFFS_Y(coord) = fmodf(OFFS_Y(coord), PL_SEGMENT_LEN);
  }
  if (fabsf(OFFS_Z(coord)) >= PL_SEGMENT_LEN) {
    SEG_Z(coord) += (int16_t) (OFFS_Z(coord) / PL_SEGMENT_LEN);
    OFFS_Z(coord) = fmodf(OFFS_Z(coord), PL_SEGMENT_LEN);
  }
}

void
plLwcTranslate(PLlwcoord *coord, PLfloat3 offs)
{
  coord->offs = vf3_add(coord->offs, offs);
  plLwcNormalise(coord);
}

PLfloat3
plLwcGlobal(const PLlwcoord *coord)
{
  PLfloat3 p = coord->offs;
  return vf3_add(p, vf3_s_mul(p, PL_SEGMENT_LEN));
}

PLfloat3
plLwcRelVec(const PLlwcoord *coord, PLint3 seg)
{
  PLfloat3 r = coord->offs;
  PLint3 segdiff = coord->seg - seg;
  PLfloat3 segdiffr = vf3_set((float)((int32_t*)&segdiff)[0],
                               (float)((int32_t*)&segdiff)[1],
                               (float)((int32_t*)&segdiff)[2]);
  r = vf3_add(r, vf3_s_mul(segdiffr, PL_SEGMENT_LEN));
  return r;
}

PLfloat3
plLwcDist(const PLlwcoord *a, const PLlwcoord * b)
{
  PLfloat3 diff = vf3_sub(a->offs, b->offs);
  PLint3 segdiff = a->seg - b->seg;
  PLfloat3 segdiffr = vf3_set((float)((int32_t*)&segdiff)[0],
                              (float)((int32_t*)&segdiff)[1],
                              (float)((int32_t*)&segdiff)[2]);

  return vf3_add(diff, vf3_s_mul(segdiffr, PL_SEGMENT_LEN));
}

PLfloat3
plGetObjGlobal3d(PLobject2 *obj)
{
  return plLwcGlobal(&obj->p);
}

PLfloat3
plObjectDistance(PLobject2 *a, PLobject2 *b)
{
  return plLwcDist(&a->p, &b->p);
}

PLobject2*
plObject3d(double x, double y, double z)
{
  PLobject2 *obj = malloc(sizeof(PLobject2));
  obj->p.offs = vf3_set(x, y, z);
  obj->p.seg = vi3_set(0, 0, 0);
  plLwcNormalise(&obj->p);
  return obj;
}

void
plObjectDelete(PLobject2 *obj)
{
  free(obj);
}

void
plTranslateObject3fv(PLobject2 *obj, PLfloat3 dp)
{
  plLwcTranslate(&obj->p, dp);
}

PLfloat3
plObjectGlobal(PLobject2 *obj)
{
  return plLwcGlobal(&obj->p);
}

