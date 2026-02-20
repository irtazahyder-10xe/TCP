# TCP

## Environment
Run the following commands to create the required enviroment.
``` bash
    git clone https://gitlab.com/qemu-project/qemu.git -b stable-8.2
    build_env QEMU_PATH=<QEMU repo>
```

## RISC-V VM
``` bash
    make run_vm
```

## GDB
Currently using the QEMU -s flag to open gdbserver on tcp:1234
``` bash
    # Make sure the bin/boot.dbg file is built
    make build
    make gdb
```
