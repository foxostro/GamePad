# vim:ts=4:sw=4:expandtab
import os
import glob
import platform

SOURCES = glob.glob('*.c') + glob.glob('*.cpp')

env = Environment(ENV=os.environ)

env['CC'] = "gcc"
env['CXX'] = "g++"

env.Append(CPPDEFINES="DEBUG")
env.Append(CCFLAGS = ['-ggdb', '-Wall' ])
env.Append(LINKFLAGS = [ '-rdynamic' ])

env['FRAMEWORKS'] = ['CoreFoundation', 'IOKit', 'System']

env.Program(target = 'GamePad', source = SOURCES)
