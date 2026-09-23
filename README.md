# The Giant

A simple bare metal OS written in x86_64 assembly and C that (currently just) plays Sokoban

## Running

Running the project is as easy as executing the Makefile and running it with your favorite VM

```
make
qemu-system-x86_64 -cdrom giant-os.iso
```