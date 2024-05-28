# L4D2-Cheat

### Tested on
- [Fedora (amd64)](https://getfedora.org/)
- [Gentoo (amd64)](https://www.gentoo.org/)
- [Mint (amd64)](https://linuxmint.com/)

### Features
- Bhop (+ Bhop delay to try throwing off anticheats, which is optional)
- Glow hack
- Removing boomer effects

### How to use
- Commands
  - bhopdelay <on/off>
  - glow <on/off>
  - noboom <on/off>
- Bhop
  - Up/Down arrows to toggle on and off
  - To Bhop, hold LCTRL and strafe

You'll need to be root to run L4D2-Cheat

### Requirements
- GCC - see config.mk if you want to change this
- Make

#### Building
```
$ git clone https://github.com/VMpc/L4D2-Cheat
$ cd L4D2-Cheat
$ make all
```

#### Installing L4D2-Cheat
```
# make install
```

#### Uninstalling
```
# make uninstall
```

##### Make options

Don't add these unless you know what you're doing.

Add CC='cc' to change the compiler.

Add DIR='/usr/local/sbin' to change the install location. (uninstall must use the same thing)

#### Troubleshooting
If you run into

```
/usr/bin/ld: skipping incompatible /usr/lib/x86_64-linux-gnu/libXtst.so when searching for -lXtst
/usr/bin/ld: skipping incompatible /usr/lib/x86_64-linux-gnu/libXtst.a when searching for -lXtst
/usr/bin/ld: cannot find -lXtst: No such file or directory
/usr/bin/ld: skipping incompatible /usr/lib/x86_64-linux-gnu/libXtst.so when searching for -lXtst
```
Fix it by running

```
sudo apt-get install libxtst-dev:i386
```

## Thanks to
- [Helped with offsets](https://github.com/StrafeTx)
- [Moral support / Helped figure out the hellscape of ptrace](https://github.com/StrafeReaver)

## License
L4D2-Cheat is licensed under GPL version 3 or later.
