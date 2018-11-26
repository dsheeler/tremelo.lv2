project(
  'tremelo', 'c',
  meson_version: '>= 0.47.0',
  license: 'GPL-2.0-only',
  version: '1.0.0',
)

# Versioning
package_name = meson.project_name()
package_version = meson.project_version()

# Paths
prefix = get_option('prefix')
libdir = join_paths(prefix, get_option('libdir'))
lv2dir = join_paths(libdir, 'lv2')

# Dependencies
cc = meson.get_compiler('c')

lv2_req = '>= 1.12.0'

lv2_dep = dependency('lv2', version: lv2_req)

libm_dep = cc.find_library('m', required: true)

# Configuration
bundle_name = get_option('bundle')

host_system = host_machine.system()
if host_system == 'darwin'
  sha_extension = '.dylib'
elif host_system == 'windows'
  sha_extension = '.dll'
else
  sha_extension = '.so'
endif

if get_option('buildtype') == 'release'
  add_project_arguments([
    '-ffast-math',
    '-fno-finite-math-only',
    '-fomit-frame-pointer',
  ], language: 'c')
endif

# Build
tremelo_sources = files([
  'tremelo.c',
])

tremelo_deps = [
  libm_dep,
  lv2_dep,
]

tremelo_sha = shared_library(
  'tremelo', tremelo_sources,
  dependencies: tremelo_deps,
  name_prefix: '',
  install: true,
  install_dir: join_paths(
    lv2dir,
    bundle_name,
  )
)

# Data
config_ttl = configuration_data()
config_ttl.set('LIB_EXT', sha_extension)
config_ttl.set('VERSION', package_version)

manifest_ttl = configure_file(
  input: 'manifest.ttl.in',
  output: 'manifest.ttl',
  configuration: config_ttl,
  install: true,
  install_dir: join_paths(
    lv2dir,
    bundle_name,
  )
)

tremelo_ttl = configure_file(
  input: 'tremelo.ttl.in',
  output: 'tremelo.ttl',
  configuration: config_ttl,
  install: true,
  install_dir: join_paths(
    lv2dir,
    bundle_name,
  )
)

# Message
summary = [
  '',
  '------',
  '@0@.lv2 @1@'.format(package_name, package_version),
  '',
  '  Bundle: @0@'.format(bundle_name),
  '',
  'Directories:',
  '  prefix: @0@'.format(prefix),
  '  libdir: @0@'.format(libdir),
  '  lv2dir: @0@'.format(lv2dir),
  '------',
]

message('\n'.join(summary))
