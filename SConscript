# -*- python -*-
Import('env')
Import('python')

env.Append(CPPPATH = ["/usr/include/python%s" % python])
env.Append(LIBS = ["python%s" % python])

lib = env.SharedLibrary("lib/bp3", env.Glob("src/*.cpp") + env.Glob("src/*/*.cpp"))

env.SharedLibrary("scratch/example", "scratch/example.cc", SHLIBPREFIX="")
env.AlwaysBuild("scratch/run.py")
env.Program("scratch/typeid", "scratch/typeid.cpp")

env.Program("tests/keywords", "tests/keywords.cpp", 
            LIBPATH = ["lib"],
            LIBS = env["LIBS"] + ["bp3"])
