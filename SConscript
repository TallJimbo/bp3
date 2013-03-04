# -*- python -*-
Import('env')

env.Append(RPATH = Dir("lib").abspath)

# Workaround for SCons bug: The above doesn't always work correctly.
# RPATH clashes with LINKFLAGS.  So if LINKFLAGS is already set, we need this workaround.
# See: http://scons.tigris.org/issues/show_bug.cgi?id=1644
# Fixed in version 2.1, but leave this next line until scons 2.1 is the version in fink.
env.Append( LINKFLAGS = ["$__RPATH"] )

lib = env.SharedLibrary("lib/bp3", env.Glob("src/*.cpp") + env.Glob("src/*/*.cpp"))


test_env = env.Clone()
test_env.Append(LIBPATH=["lib"])
test_env.Append(LIBS=["bp3"])
test_env.Program("tests/from_python", "tests/from_python.cpp")
test_env.SharedLibrary("scratch/example", "scratch/example.cc", SHLIBPREFIX="")
test_env.AlwaysBuild("scratch/run.py")
test_env.Program("scratch/typeid", "scratch/typeid.cpp")

def writeEupsTable(target, source, env):
    filename = target[0].abspath
    with open(filename, 'w') as f:
        f.write("setupRequired(python)\n")
        f.write("envPrepend(LD_LIBRARY_PATH, ${PRODUCT_DIR}/lib)\n")
        f.write("envPrepend(PYTHONPATH, ${PRODUCT_DIR}/scratch)\n")
    return None
env.Command("ups/bp3.table", None, writeEupsTable)
