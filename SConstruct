# -*- python -*-

import os
import sys
import re

def CheckPython(context):
    python_info_file = """
from __future__ import print_function
import sys
import distutils.sysconfig
target = sys.argv[1]
with open(target, 'w') as f:
    print("py{0}.{1}".format(*sys.version_info[0:2]), file=f)
    print(distutils.sysconfig.get_python_inc(), file=f)
    print(distutils.sysconfig.get_config_var("LIBDIR"), file=f)
    print(distutils.sysconfig.get_config_var("LIBRARY"), file=f)
    print(distutils.sysconfig.get_config_var("SHLIBS"), file=f)
"""
    python_source_file = """
#include "Python.h"
int main()
{
  Py_Initialize();
  Py_Finalize();
  return 0;
}
"""
    context.Message('Configuring Python... ')
    action = "{0} $SOURCES $TARGETS".format(GetOption("python"))
    result, output = context.TryAction(action, python_info_file, "py")
    if not result:
        context.Result(False)
        print("Error looking up Python configuration with distutils (see config.log).")
        Exit(1)
    variant, include, libdir, libfile, shlibs = output.strip().split("\n")
    context.env["variant"] = os.path.join("build", variant)
    context.env.AppendUnique(CPPPATH=[include])
    context.env.AppendUnique(LIBPATH=[libdir])
    match = re.search("(python.*)\.(a|so|dylib)", libfile)
    if match:
        context.env.AppendUnique(LIBS=[match.group(1)])
    shflags = [f for f in shlibs.split() if f != "-L"]
    context.env.MergeFlags(" ".join(shflags))
    result, output = context.TryRun(python_source_file,'.cpp')
    if not result and sys.platform == 'darwin':
        # Sometimes we need some extra stuff on Mac OS
        framework = libdir       # search up the libDir tree for the proper home for frameworks
        while framework and framework != "/":
            framework, d2 = os.path.split(framework)
            if d2 == "Python.framework":
                if not "Python" in os.listdir(os.path.join(framework, d2)):
                    context.Result(False)
                    print("Python not found in framework directory {0}".format(framework))
                    Exit(1)
                break
        context.env.AppendUnique(LDFLAGS="-F{0}".format(framework))
        result, output = context.TryRun(python_source_file,'.cpp')
    if not result:
        context.Result(False)
        print "Cannot run program built with Python."
        Exit(1)    
    context.Result(True)
    return True

AddOption("--with-python", dest="python", default="python",
          help="name of Python executable to build against")

env = Environment(CXX="g++-4.6")

for key in ("PATH", "PYTHONPATH", "LD_LIBRARY_PATH", "DYLD_LIBRARY_PATH"):
    if os.environ.has_key(key):
        env["ENV"][key] = os.environ[key]

if not GetOption("help") and not GetOption("clean"):
    config = env.Configure(
        custom_tests = {
            "CheckPython": CheckPython,
        }
    )
    config.CheckPython()
    env = config.Finish()

env.Append(CCFLAGS = "-std=c++0x")
env.Append(CCFLAGS = "-g")
env.Append(CCFLAGS = "-Wall")
env.Append(CPPPATH = ["#include", "#internal"])
env.SConscript('SConscript', variant_dir=env["variant"], exports='env', duplicate=0)
