/*
 Copyright 2010 Mattias Holm <mattias.holm(at)openorbit.org>

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

#ifndef SG_DRAWABLE_H
#define SG_DRAWABLE_H

#include <stdlib.h>
#include <stdbool.h>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#else
#include <gl/gl.h>
#include <gl/glu.h>
#endif


#include <vmath/vmath.h>
#include "rendering/reftypes.h"
#include "parsers/model.h"
#include "rendering/material.h"
#include "common/lwcoord.h"
#include <gencds/list.h>

struct SGdrawable {
  LIST_ENTRY(SGdrawable) siblings;
  LIST_HEAD(SGdrawable) children;
  struct SGdrawable *parent;
  const char *name;
  SGscene *scene;
  bool enabled;
  float3 p;  // Position
  float3 dp; // delta pos per time step
  float3 dr; // delta rot per time step
  quaternion_t q; // Quaternion
                  //float s; // Scale
  GLfloat R[16];
  SGdrawfunc draw; // Draw function for this object
};

typedef struct SGellipsis {
  SGdrawable super;
  double semiMajor;
  double semiMinor;
  double ecc;
  float colour[3];
  size_t vertCount;
  float verts[];
} SGellipsis;

typedef struct SGmodel {
  SGdrawable super;
  model_t *modelData;
} SGmodel;

struct SGsphere {
  SGdrawable super;
  SGmaterial mat;
  GLuint texId;
  GLUquadricObj *quadratic;

  GLuint dayTexId;
  GLuint nightTexId;
  GLuint cloudTexId;
  GLuint bumpTexId;

  GLuint northPoleIdx;
  GLuint northPoleCount;
  GLuint southPoleIdx;
  GLuint southPoleCount;

  GLuint sliceCount;
  GLuint stackCount;
  GLuint stripIdx;

  GLfloat *vertices;
  GLfloat *normals;
  GLfloat *texCoords;

  GLfloat radius;
};


struct SGcylinder {
  SGdrawable super;
  //  GLuint texId;
  GLbyte col[3];
  GLUquadricObj *quadratic;
  GLfloat bottonRadius;
  GLfloat topRadius;
  GLfloat height;
};


SGdrawable* sgNewDrawable(SGdrawable *drawable, const char *name, SGdrawfunc df);

void sgSetObjectQuat4f(SGdrawable *obj, float x, float y, float z, float w);
void sgSetObjectQuatv(SGdrawable *obj, quaternion_t q);
void sgSetObjectPosLWAndOffset(SGdrawable *obj, const OOlwcoord *lw, float3 offset);
void sgSetObjectPosLW(SGdrawable *obj, const OOlwcoord *lw);
void sgSetObjectPos3f(SGdrawable *obj, float x, float y, float z);
//void ooSgSetObjectScale(SGdrawable *obj, float s);
void sgSetObjectSpeed(SGdrawable *obj, float dx, float dy, float dz);
void sgSetObjectAngularSpeed(SGdrawable *obj, float drx, float dry, float drz);

SGmaterial* sgSphereGetMaterial(SGsphere *sphere);


SGdrawable* sgNewSphere(const char *name, float radius, const char *tex);
SGdrawable* sgNewEllipsis(const char *name,
                          double semiMajor, double semiMinor,
                          double longAsc, double inc, double argPeri,
                          float r, float g, float b,
                          size_t vertCount);

SGdrawable* sgLoadModel(const char *file);

void sgDrawableAddChild(SGdrawable * restrict parent, SGdrawable * restrict child,
                        float3 t, quaternion_t q);
void sgPaintDrawable(SGdrawable *drawable);


#endif /* !SG_DRAWABLE_H */
