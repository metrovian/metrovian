# metrovian daemon
src_daemon = files(
  'server-micro/parser/read/state.cpp',
  'server-micro/parser/read/waveform.cpp',
  'server-micro/parser/write/waveform.cpp',
  'server-micro/parser/abstract.cpp',
  'server-micro/parser/router.cpp',
  'server-micro/api.cpp',
  'server-micro/context.cpp',
  'server-micro/response.cpp',
  'state/abstract.cpp',
  'state/none.cpp',
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