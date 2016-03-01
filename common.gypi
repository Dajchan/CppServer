{
  'make_global_settings': [
    ['CXX','/usr/bin/clang++'],
    ['LINK','/usr/bin/clang++'],
  ],
  'configurations': {
    'Debug': {
      'xcode_settings': {
        'GCC_OPTIMIZATION_LEVEL': '0',
        'ONLY_ACTIVE_ARCH': 'YES',
      },
    },
  },

  'target_defaults': {
    'default_configuration': 'Debug',
    'cflags': ['-Wall', '-Werror', '-Wno-deprecated-register', '-fvisibility=default', '-Wno-shorten-64-to-32', '-Wno-tautological-undefined-compare', '-Wno-unused-variable', '-Wno-unused-function'],
    'cflags_cc': ['<@(_cflags)', '-std=c++11', '-fexceptions', '-frtti'],
    'xcode_settings': {
      'OTHER_CFLAGS' : ['<@(_cflags)'],
      'OTHER_CPLUSPLUSFLAGS' : ['<@(_cflags_cc)'],
      'CLANG_CXX_LANGUAGE_STANDARD': 'c++11',
      'CLANG_CXX_LIBRARY': 'libc++',
      'CLANG_ENABLE_OBJC_ARC': 'YES',
    },
    'conditions': [
      ['OS=="darwin"',
        {
          'xcode_settings' : {
            'SDKROOT': 'macosx',
          },
        }
      ],
      ['OS=="linux"',
        {
          'cflags+': ['-fPIC'],
          'cflags_cc+': ['-stdlib=libc++'],
          'ldflags': ['-nodefaultlibs', '-lc++', '-lc++abi', '-lm', '-ld', '-lc', '-lcrt', '-lgcc_s', '-lgcc', '-rdynamic'],
        }
      ],
    ],
    'configurations': {
      'Debug': {
        'defines': [ 'DEBUG', 'TARGET_OS_<@(OS)=1' ],
        'cflags' : [ '-g', '-O0' ],
      },
      'Release': {
        'defines': [ 'NDEBUG', 'TARGET_OS_<@(OS)=1' ],
        'cflags': [
          '-Os',
          '-fomit-frame-pointer',
          '-fdata-sections',
          '-ffunction-sections',
        ],
        'xcode_settings': {
          'DEAD_CODE_STRIPPING': 'YES',
        },
      },
    },
  },
}
