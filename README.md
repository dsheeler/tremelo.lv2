tremelo.lv2
==============

tremelo.lv2 is an audio-plugin in [LV2](http://lv2plug.in) format that
modulates the amplitude of the input signal.

The modulation is sinusoidal with adjustable amplitude and frequency.

Install
-------

Compiling this plugin requires the LV2 SDK, gnu-make and a c-compiler.

```bash
  git clone git://github.com/dsheeler/tremelo.lv2.git
  cd tremelo.lv2
  make
  sudo make install PREFIX=/usr
```

Note to packagers: The Makefile honors `PREFIX` and `DESTDIR` variables as well
as `CFLAGS`, `LDFLAGS` and `OPTIMIZATIONS` (additions to `CFLAGS`).
