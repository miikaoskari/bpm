project('bpmdisp', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

cc = meson.get_compiler('c')

# Declare dependencies
pipewire_dep = dependency('libpipewire-0.3')
m_dep = cc.find_library('m', required: true)

# Subdirectories
subdir('src')