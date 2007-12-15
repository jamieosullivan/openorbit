#   The contents of this file are subject to the Mozilla Public License
#   Version 1.1 (the "License"); you may not use this file except in compliance
#   with the License. You may obtain a copy of the License at
#   http://www.mozilla.org/MPL/
#   
#   Software distributed under the License is distributed on an "AS IS" basis,
#   WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
#   for the specific language governing rights and limitations under the
#   License.
#   
#   The Original Code is the Open Orbit space flight simulator.
#   
#   The Initial Developer of the Original Code is Mattias Holm. Portions
#   created by the Initial Developer are Copyright (C) 2006 the
#   Initial Developer. All Rights Reserved.
#   
#   Contributor(s):
#       Mattias Holm <mattias.holm(at)contra.nu>.
#   
#   Alternatively, the contents of this file may be used under the terms of
#   either the GNU General Public License Version 2 or later (the "GPL"), or
#   the GNU Lesser General Public License Version 2.1 or later (the "LGPL"), in
#   which case the provisions of GPL or the LGPL License are applicable instead
#   of those above. If you wish to allow use of your version of this file only
#   under the terms of the GPL or the LGPL and not to allow others to use your
#   version of this file under the MPL, indicate your decision by deleting the
#   provisions above and replace  them with the notice and other provisions
#   required by the GPL or the LGPL.  If you do not delete the provisions
#   above, a recipient may use your version of this file under either the MPL,
#   the GPL or the LGPL."


# This is a sample configuration file for Open Orbit. The configfile is actually
# written in Python, so although this one is pretty booring, it is possible to
# write a fully fledged program here that will be executed during boot of Open
# Orbit. The following defined variables have a special meaning though as they
# are the ones extracted after the script has been executed. Changing them later
# on is not guaranteed to have any effect.
# More about the config file can be read in the documentation.

# For the moment there is a slight problem with running this, since the
# graphics system is not initialised, this will change in the future however

print "Loading config..."

import io       # I/O module, allowing the binding of key handlers
import config   # config, allows one to set config values
import ConfigParser
import os
from os import environ

home = environ["HOME"]
resSearchPaths = [home + "/Library/Preferences/", home + "/.openorbit/"]
prefsSearchPaths = [home + "/Library/Application Support/Open Orbit/", home + "/.openorbit/"]
# Find path that contains the openorbit.conf file, this path is our default dir
# for preferences, we should create the directory if it does not exist
for path in prefsSearchPaths:
    prefsPath = None
    if os.path.isfile(os.path.join(path, "openorbit.conf"):
        prefsPath = path
        break

if prefsPath == None:
    # not found
    pass

# Find path with open orbit resources
for path in resSearchPaths:
    resPath = None
    if os.path.isfile(os.path.join(path, ""):
        resPath = path
        break

if resPath == None:
    # not found
    pass

    
# A few default values for the config parser
defaultValues = {'HOME' : home, 'RES': resPath}

prefs = ConfigParser(defaultValues)
prefs.read([path + "openorbit.conf" for path in searchPaths])

# Populate the default preferences
if not prefs.has_section("VIDEO"):
    prefs.set("VIDEO", "WIDTH", 800)
    prefs.set("VIDEO", "HEIGHT", 600)
    prefs.set("VIDEO", "FULLSCREEN", False)

if not prefs.has_section("AUDIO"):
    pass

if not prefs.has_section("KEYBOARD"):
    prefs.set("KEYBOARD", "cam-fwd",        "e")
    prefs.set("KEYBOARD", "cam-back",       "d")
    prefs.set("KEYBOARD", "cam-left",       "s")
    prefs.set("KEYBOARD", "cam-right",      "f")
    prefs.set("KEYBOARD", "cam-up",         "a")
    prefs.set("KEYBOARD", "cam-down",       "z")
    prefs.set("KEYBOARD", "cam-pitch-up",   "g")
    prefs.set("KEYBOARD", "cam-pitch-down", "t")
    prefs.set("KEYBOARD", "cam-yaw-right",  "v")
    prefs.set("KEYBOARD", "cam-yaw-left",   "x")
    prefs.set("KEYBOARD", "cam-roll-left",  "w")
    prefs.set("KEYBOARD", "cam-roll-right", "r")
    
if not prefs.has_section("MOUSE"):
    pass


# Write all defaults to disk (maybe we should not write anything if there have
# been no modifications?)
fp = open(os.path.join(prefsPath, "openorbit.conf"), "w")
prefs.write(fp)
fp.close()

try:
    videoWidth = prefs.getint("VIDEO", "width")
    videoHeight = prefs.getint("VIDEO", "height")
    videoFullscreen = prefs.getboolean("VIDEO", "fullscreen")
    videoDepth = prefs.getboolean("VIDEO", "depth")
except NoOptionError:
    print "Options width, height or fullscreen missing in section VIDEO"
    sys.exit(1)
except NoSectionError:
    print "VIDEO section missing"
    sys.exit(1)
    
config.setScreenSize(videoWidth, videoHeight)
config.setFullscreen(videoFullscreen)
config.setScreenDepth(videoDepth)

camControlActionKeys = ["cam-fwd", "cam-back", "cam-left", "cam-right",
                        "cam-up", "cam-down", "cam-pitch-up", "cam-pitch-down",
                        "cam-yaw-right", "cam-yaw-left", "cam-roll-left",
                        "cam-roll-right"]

for key in camControlActionKeys:
    try:
        keyCode = prefs.get("KEYBOARD", key)
        io.bindKeyDown(keyCode, 0, key)
    except NoOptionError:
        # ignore missing bindings...
        pass
    except NoSectionError:
        print "fatal error, no KEYBOARD section"
        sys.exit(1)


#def foo():
#    print "button pressed"

#def bar(a, b):
#    print a, b

#def drag(a, b):
#    print "drag" + str((a, b))

# Register the function above as an action handler
#io.registerButtonHandler("foo", foo)
#io.registerClickHandler("bar", bar)
#io.registerDragHandler("drag", drag)


# io.bindKeyUp can bind both C-function and Python functions to button presses
# This is why you have to register the button handler function, i.e. to make
# the interface identical, independent of whether you are using C or Python.    

#io.bindKeyDown("a", 0, "foo")
#io.bindMouseDown(io.LEFT, "bar")
#io.bindMouseDown(io.RIGHT, "foo")
#io.bindMouseDrag(io.LEFT, "drag")
