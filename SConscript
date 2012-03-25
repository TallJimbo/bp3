# -*- python -*-
Import('env')

env.Append(RPATH = Dir("lib").abspath)

# Workaround for SCons bug: The above doesn't always work correctly.
# RPATH clashes with LINKFLAGS.  So if LINKFLAGS is already set, we need this workaround.
# See: http://scons.tigris.org/issues/show_bug.cgi?id=1644
# Fixed in version 2.1, but leave this next line until scons 2.1 is the version in fink.
env.Append( LINKFLAGS = ["$__RPATH"] )

lib = env.SharedLibrary("lib/bp3", env.Glob("src/*.cpp") + env.Glob("src/*/*.cpp"))

env.SharedLibrary("scratch/example", "scratch/example.cc", SHLIBPREFIX="")
env.AlwaysBuild("scratch/run.py")
env.Program("scratch/typeid", "scratch/typeid.cpp")

test_env = env.Clone()
test_env.Append(LIBPATH=["lib"])
test_env.Append(LIBS=["bp3"])
test_env.Program("tests/keywords", "tests/keywords.cpp")
test_env.Program("tests/from_python", "tests/from_python.cpp")
