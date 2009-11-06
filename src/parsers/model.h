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

#ifndef MODEL_H
#define MODEL_H

#include <stdint.h>
#include <stdlib.h>


typedef struct model_t {
  float trans[3];
  float rot[4][4];

  size_t childCount;
  struct model_t **children;

  uint32_t vertexCount;
  float *vertices;
  float *texCoords;
  float *normals;
  float *colours;
  uint32_t *faces;
} model_t;

model_t * model_load(const char * restrict fileName);
int model_dispose(model_t * restrict model);


#endif /* end of include guard: MODEL_H */

