# vim:ts=4:sw=4:expandtab
import os
import glob
import platform

SOURCES = glob.glob('*.c')

env = Environment(ENV=os.environ)

env['CC'] = "gcc"

env.Append(CPPDEFINES="DEBUG")
env.Append(CCFLAGS = ['-ggdb', '-Wall' ])
env.Append(LINKFLAGS = [ '-rdynamic' ])

env['FRAMEWORKS'] = ['CoreFoundation', 'IOKit', 'System']

env.Program(target = 'GamePad', source = SOURCES)
