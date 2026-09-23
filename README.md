# The Giant

A simple bare metal OS written in x86_64 assembly and C that (currently just) plays Sokoban

## Running

Running the project is as easy as executing the Makefile and running it with your favorite VM

```
make
qemu-system-x86_64 -cdrom giant-os.iso
```

Or just can build and run directly

```
make run
```

It is strongly recommended to use QEMU but you can use any software you prefer

## WARNING
Although it's technically possible to run it on real hardware, it has never been tested that way and has only been executed on virtual machines. RUN AT YOUR OWN RISK!