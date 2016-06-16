solution('bw-asset-parser')
location('build')
targetdir('build')
configurations({'Debug', 'Release'})
configuration('Debug')
flags({'Symbols'})
configuration('')

project('pugixml')
kind('StaticLib')
language('C++')
files({'pugiconfig.hpp', 'pugixml.hpp', 'pugixml.cpp'})

project('VisualTest')
kind('ConsoleApp')
language('C++')
files({'visual.h','visual.cpp'})
defines({'TEST_VISUAL'})
links('pugixml')



