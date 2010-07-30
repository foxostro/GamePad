# vim:ts=4:sw=4:expandtab
import os
import glob
import platform

SOURCES = glob.glob('src/GamePad/*.c') + ['src/main.c']

env = Environment(ENV=os.environ)

# The compiler options below were chosen for use with gcc, so make sure to build with gcc.
env['CC'] = "gcc"

env.Append(CPPDEFINES="DEBUG")
env.Append(CPPPATH='inc')
env.Append(CCFLAGS = ['-ggdb', '-Wall' ])
env.Append(LINKFLAGS = [ '-rdynamic' ])

env['FRAMEWORKS'] = ['CoreFoundation', 'IOKit', 'System']

env.Program(target = 'GamePad', source = SOURCES)
