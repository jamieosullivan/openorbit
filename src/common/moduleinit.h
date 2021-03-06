/*
 Copyright 2010 Mattias Holm <mattias.holm(at)openorbit.org>

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

#ifndef COMMON__MODULE_INIT_H
#define COMMON__MODULE_INIT_H
#include <stddef.h>
/*!
  Module initalisation macros.

  The module initialisation macros define constructor functions used for
  initialising global owned by a module. The module init functions are executed
  before main, and will be executed in the order of PRIMARY, SECONDARY and
  TERTIARY. With no order defined within the specific classes.

  They depend on GCC extensions, but if it is necessary to build the system with
  compilers that are not GCC compatible, then you need add the module
  initialisation functions to the top of main.
 */

/// Used when initialising modules that do not depend on any specific order
//#define INIT_PRIMARY_MODULE \
//  static void __attribute__((constructor(101))) Init(void)

/// Used for initialiser of secondary modules. That is modules that depend on other
/// modules such as a module that depend on the settings module is available
//#define INIT_SECONDARY_MODULE \
//  static void __attribute__((constructor(102))) Init(void)

/// Used for initialiser in a module that depends on a secondary module
//#define INIT_TERTIARY_MODULE \
//static void __attribute__((constructor(103))) Init(void)

/// Used for registering IO handlers in a module. This is guaranteed to be executed
/// after the IO manager has been initialised.
//#define INIT_IO \
//  static void __attribute__((constructor(102))) IOInit(void)

//#define INIT_MFD \
//  static void __attribute__((constructor(102))) MFDInit(void)

//#define INIT_SIM_MODULE\
//  static void __attribute__((constructor(103))) SIMInit(void)

//#define INIT_STATIC_SC_PLUGIN \
//  static void __attribute__((constructor(104))) PluginInit(void)

typedef void (*module_init_t)(void);

#define MODULE_INIT(m, ...)\
  static void m##_init(void);\
  static void __attribute__((constructor)) m##_construct(void)\
  {\
    char *arg[] = {__VA_ARGS__};\
    module_register(#m, m##_init, arg);\
  }\
  \
  static void m ##_init()


void module_register(const char *module_name, module_init_t module_init,
                     char **dependencies);
void module_initialize(void);

#endif /*  !COMMON__MODULE_INIT_H */
