# -*- python -*-
Import('env')
Import('python')

env.Append(CPPPATH = ["/usr/include/python%s" % python])
env.Append(LIBS = ["python%s" % python])

env.SharedLibrary("lib/bp3", env.Glob("src/*/*.cpp"))

env.SharedLibrary("scratch/example", "scratch/example.cc", SHLIBPREFIX="")
