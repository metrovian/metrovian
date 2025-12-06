# metrovian daemon
src_daemon = files(
  'server-micro/api.cpp',
  'server-micro/context.cpp',
  'state/abstract.cpp',
  'state/dummy.cpp',
  'state/performance.cpp',
  'state/setup.cpp',
  'state/synthesis.cpp',
  'main.cpp')

executable(
  'metroviand',
  src_daemon,
  include_directories: inc_core,
  dependencies: [dep_core],
  install : true, 
  install_dir : '/usr/local/bin/')