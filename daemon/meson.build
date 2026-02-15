# metrovian daemon
src_daemon = files(
  'api/read/mid.cpp',
  'api/read/mids.cpp',
  'api/read/notes.cpp',
  'api/read/progress.cpp',
  'api/read/state.cpp',
  'api/read/waveform.cpp',
  'api/read/waveforms.cpp',
  'api/write/play.cpp',
  'api/write/reboot.cpp',
  'api/write/volume.cpp',
  'api/write/waveform.cpp',
  'api/router.cpp',
  'api/response.cpp',
  'machine/state/performance.cpp',
  'machine/state/setup.cpp',
  'machine/state/synthesis.cpp',
  'machine/api.cpp',
  'machine/automata.cpp',
  'context.cpp',
  'main.cpp')

executable(
  'metroviand',
  src_daemon,
  include_directories: inc_core,
  dependencies: [dep_core],
  install : true, 
  install_dir : '/usr/local/bin/')