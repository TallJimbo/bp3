env = Environment(CXX="g++-4.6")
env.Append(CCFLAGS = "-std=c++0x")
env.Append(CPPPATH = ["#include"])
env.Append(CPPPATH = ["/usr/include/python2.6"])
env.Append(LIBS = ["python2.6"])

env.SharedLibrary("lib/bp3", env.Glob("#src/*/*.cpp"))

env.SharedLibrary("scratch/example", "scratch/example.cc", SHLIBPREFIX="")
