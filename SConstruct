# -*- python -*-
python = ARGUMENTS.get("py", "2.6")
env = Environment(CXX="g++-4.6")
env.Append(CCFLAGS = "-std=c++0x")
env.Append(CCFLAGS = "-g")
env.Append(CPPPATH = ["#include"])
Export('env')
Export('python')
env.SConscript('SConscript', variant_dir='build/%s' % python)
