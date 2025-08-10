project('imgview', 'cpp',
  version : '1.1',
  default_options : [
    'warning_level=3',
    'c++_std=c20',
    'cpp_std=c20',
    'cxx_std=c20',
    'optimization=3',
  ]
)

pname = 'imgview'
# imlib2 = dependency('imlib2', method: 'pkg-config')
sdl3 = dependency('sdl3', method: 'pkg-config')
deps = [
  sdl3
  ]

infile = [
  'src/main.cpp', 'src/window.cpp',
  'src/argparse.cpp', 'src/state.cpp'
  ]

executable(
  pname,
  infile,
  dependencies: deps,
  install : true,
  link_args : ['-lm']
)
