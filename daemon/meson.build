# metrovian daemon
src_daemon = files(
  'hardware/abstract.cpp',
  'hardware/development.cpp',
  'hardware/development/indicator.cpp',
  'hardware/development/knob.cpp',
  'hardware/raspi.cpp',
  'hardware/raspi/indicator.cpp',
  'hardware/raspi/knob.cpp',
  'hardware/raspi/segment.cpp',
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