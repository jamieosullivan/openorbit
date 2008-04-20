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
/*! 
    Resource manager header
 
    The resource manager is not a manager per see at the moment, but might
    become one in the future. At the moment it supplies a number of functions
    that can be used to locate resource files in a platform independent manner.
    
    Resource location is heavily dependant on the target platform. On the Mac
    for example, resources are expected to be located in the application bundle
    and we use the core foundataion API to locate these. On normal posix
    systems (not including MacOS X), we will probably just look through
    $PREFIX/share/openorbit/resources and $HOME/.openorbit/resources. On the Mac
    the user directories will be
    "~/Library/Application Support/Open Orbit/Resources".
 */
#ifndef _RES_MANAGER_H_
#define _RES_MANAGER_H_
#include <stdio.h>

char* res_get_path(const char *file_name);
FILE* res_get_file(const char *file_name);
int res_get_fd(const char *file_name);

char* plugin_get_path(const char *file_name);
FILE* plugin_get_file(const char *file_name);
int plugin_get_fd(const char *file_name);

#endif /* _RES_MANAGER_H_ */