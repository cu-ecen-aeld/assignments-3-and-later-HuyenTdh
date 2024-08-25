# Analysis the kernel oops
Run ```./runqemu.sh``` to start QEMU machine. Connect via SSH and run ```echo hello_world > /dev/faulty```
<details>
<summary>Full log</summary>
$ ./runqemu.sh 
Booting Linux on physical CPU 0x0000000000 [0x410fd034]
Linux version 4.19.91 (huyendv@huyendv) (gcc version 8.4.0 (Buildroot 2020.02.12)) #1 SMP Sun Aug 25 16:01:42 +07 2024
Machine model: linux,dummy-virt
efi: Getting EFI parameters from FDT:
efi: UEFI not found.
psci: probing for conduit method from DT.
psci: PSCIv0.2 detected in firmware.
psci: Using standard PSCI v0.2 function IDs
psci: Trusted OS migration not required
random: get_random_bytes called from start_kernel+0x98/0x37c with crng_init=0
percpu: Embedded 21 pages/cpu s46552 r8192 d31272 u86016
Detected VIPT I-cache on CPU0
CPU features: enabling workaround for ARM erratum 843419
CPU features: enabling workaround for ARM erratum 845719
Built 1 zonelists, mobility grouping on.  Total pages: 32256
Kernel command line: rootwait root=/dev/vda console=ttyAMA0
Dentry cache hash table entries: 16384 (order: 5, 131072 bytes)
Inode-cache hash table entries: 8192 (order: 4, 65536 bytes)
Memory: 106228K/131072K available (4796K kernel code, 312K rwdata, 1044K rodata, 512K init, 265K bss, 24844K reserved, 0K cma-reserved)
SLUB: HWalign=64, Order=0-3, MinObjects=0, CPUs=1, Nodes=1
rcu: Hierarchical RCU implementation.
rcu: 	RCU restricting CPUs from NR_CPUS=64 to nr_cpu_ids=1.
rcu: Adjusting geometry for rcu_fanout_leaf=16, nr_cpu_ids=1
NR_IRQS: 64, nr_irqs: 64, preallocated irqs: 0
arch_timer: cp15 timer(s) running at 62.50MHz (virt).
clocksource: arch_sys_counter: mask: 0xffffffffffffff max_cycles: 0x1cd42e208c, max_idle_ns: 881590405314 ns
sched_clock: 56 bits at 62MHz, resolution 16ns, wraps every 4398046511096ns
Console: colour dummy device 80x25
Calibrating delay loop (skipped), value calculated using timer frequency.. 125.00 BogoMIPS (lpj=250000)
pid_max: default: 32768 minimum: 301
Mount-cache hash table entries: 512 (order: 0, 4096 bytes)
Mountpoint-cache hash table entries: 512 (order: 0, 4096 bytes)
ASID allocator initialised with 32768 entries
rcu: Hierarchical SRCU implementation.
EFI services will not be available.
smp: Bringing up secondary CPUs ...
smp: Brought up 1 node, 1 CPU
SMP: Total of 1 processors activated.
CPU features: detected: 32-bit EL0 Support
CPU: All CPU(s) started at EL1
alternatives: patching kernel code
devtmpfs: initialized
clocksource: jiffies: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 7645041785100000 ns
futex hash table entries: 256 (order: 2, 16384 bytes)
DMI not present or invalid.
NET: Registered protocol family 16
vdso: 2 pages (1 code @ (____ptrval____), 1 data @ (____ptrval____))
hw-breakpoint: found 6 breakpoint and 4 watchpoint registers.
DMA: preallocated 256 KiB pool for atomic allocations
Serial: AMBA PL011 UART driver
9000000.pl011: ttyAMA0 at MMIO 0x9000000 (irq = 39, base_baud = 0) is a PL011 rev1
console [ttyAMA0] enabled
SCSI subsystem initialized
clocksource: Switched to clocksource arch_sys_counter
NET: Registered protocol family 2
tcp_listen_portaddr_hash hash table entries: 256 (order: 0, 4096 bytes)
TCP established hash table entries: 1024 (order: 1, 8192 bytes)
TCP bind hash table entries: 1024 (order: 2, 16384 bytes)
TCP: Hash tables configured (established 1024 bind 1024)
UDP hash table entries: 256 (order: 1, 8192 bytes)
UDP-Lite hash table entries: 256 (order: 1, 8192 bytes)
NET: Registered protocol family 1
hw perfevents: enabled with armv8_pmuv3 PMU driver, 5 counters available
workingset: timestamp_bits=62 max_order=15 bucket_order=0
Block layer SCSI generic (bsg) driver version 0.4 loaded (major 252)
io scheduler noop registered
io scheduler deadline registered
io scheduler cfq registered (default)
io scheduler mq-deadline registered
io scheduler kyber registered
cacheinfo: Unable to detect cache hierarchy for CPU 0
virtio_blk virtio0: [vda] 122880 512-byte logical blocks (62.9 MB/60.0 MiB)
NET: Registered protocol family 10
Segment Routing with IPv6
sit: IPv6, IPv4 and MPLS over IPv4 tunneling driver
NET: Registered protocol family 17
NET: Registered protocol family 15
registered taskstats version 1
hctosys: unable to open rtc device (rtc0)
EXT4-fs (vda): mounted filesystem with ordered data mode. Opts: (null)
VFS: Mounted root (ext4 filesystem) readonly on device 254:0.
devtmpfs: mounted
Freeing unused kernel memory: 512K
Run /sbin/init as init process
EXT4-fs (vda): re-mounted. Opts: (null)
Starting syslogd: OK
Starting klogd: OK
Running sysctl: OK
Saving random seed: random: dd: uninitialized urandom read (512 bytes read)
OK
Starting network: IPv6: ADDRCONF(NETDEV_UP): eth0: link is not ready
udhcpc: started, v1.31.1
random: mktemp: uninitialized urandom read (6 bytes read)
udhcpc: sending discover
udhcpc: sending select for 10.0.2.15
udhcpc: lease of 10.0.2.15 obtained, lease time 86400
deleting routers
random: mktemp: uninitialized urandom read (6 bytes read)
IPv6: ADDRCONF(NETDEV_CHANGE): eth0: link becomes ready
adding dns 10.0.2.3
OK
Starting dropbear sshd: OK
Start S98lddmodules
scull: loading out-of-tree module taints kernel.
scullsingle registered at fb00008
sculluid registered at fb00009
scullwuid registered at fb0000a
scullpriv registered at fb0000b
Load our module, exit on failure
faulty: unknown parameter 'faulty' ignored
Get the major number (allocated with allocate_chrdev_region) from /proc/devices
Remove any existing /dev node for /dev/faulty
Add a node for our device at /dev/faulty using mknod
Change group owner to staff
Change access mode to 664
Hello, world
Starting simpleserver
Parent exit. Child's id: 156
Start deamons

Welcome to Buildroot
buildroot login: root
Password: 
# 
# 
# 
# urandom_read: 1 callbacks suppressed
random: dropbear: uninitialized urandom read (32 bytes read)
random: dropbear: uninitialized urandom read (32 bytes read)
random: dropbear: uninitialized urandom read (32 bytes read)
random: dropbear: uninitialized urandom read (32 bytes read)
random: dropbear: uninitialized urandom read (32 bytes read)
Unable to handle kernel NULL pointer dereference at virtual address 0000000000000000
Mem abort info:
  ESR = 0x96000046
  Exception class = DABT (current EL), IL = 32 bits
  SET = 0, FnV = 0
  EA = 0, S1PTW = 0
Data abort info:
  ISV = 0, ISS = 0x00000046
  CM = 0, WnR = 1
user pgtable: 4k pages, 39-bit VAs, pgdp = (____ptrval____)
[0000000000000000] pgd=0000000045f91003, pud=0000000045f91003, pmd=0000000000000000
Internal error: Oops: 96000046 [#1] SMP
Modules linked in: hello(O) faulty(O) scull(O)
Process sh (pid: 162, stack limit = 0x(____ptrval____))
CPU: 0 PID: 162 Comm: sh Tainted: G           O      4.19.91 #1
Hardware name: linux,dummy-virt (DT)
pstate: 80000005 (Nzcv daif -PAN -UAO)
pc : faulty_write+0x8/0x10 [faulty]
lr : __vfs_write+0x30/0x160
sp : ffffff80088f3d30
x29: ffffff80088f3d30 x28: ffffffc005fab980 
x27: 0000000000000000 x26: 0000000000000000 
x25: 0000000056000000 x24: 0000000000000015 
x23: 00000000004c87f0 x22: 0000000000000000 
x21: ffffff80088f3e50 x20: ffffffc005f77600 
x19: ffffff80086c8548 x18: 0000000000000000 
x17: 0000000000000000 x16: 0000000000000000 
x15: 0000000000000000 x14: 0000000000000000 
x13: 0000000000000000 x12: 0000000000000000 
x11: 0000000000000000 x10: 0000000000000000 
x9 : 0000000000000000 x8 : 0000000000000000 
x7 : 0000000000000000 x6 : ffffff800053d000 
x5 : ffffff800053b000 x4 : 0000000000000000 
x3 : ffffff80088f3e50 x2 : 000000000000000c 
x1 : 0000000000000000 x0 : 0000000000000000 
Call trace:
 faulty_write+0x8/0x10 [faulty]
 vfs_write+0xa4/0x190
 ksys_write+0x64/0xe0
 __arm64_sys_write+0x18/0x20
 el0_svc_common+0x90/0x160
 el0_svc_handler+0x68/0x80
 el0_svc+0x8/0xc
Code: bad PC value
---[ end trace 91932cc74ceb433d ]---
</details>

The error ```Unable to handle kernel NULL pointer dereference at virtual address 0000000000000000``` show that we try do dereference a NULL pointer at address 0x00, which is not allow. Stack trace show that lastest call from: ```faulty_write+0x14/0x20``` An error occurred on faulty_write function. ```[faulty]``` indicates faulty_write function in ```faulty``` module. ```0x14``` represents the position within the function's code (20 bytes from the start) where the fault was triggered. ```0x20``` indicates the total size of the faulty_write function in bytes, meaning that the entire function is 16 bytes long.

```bash
Call trace:
 faulty_write+0x8/0x10 [faulty]
 vfs_write+0xa4/0x190
 ksys_write+0x64/0xe0
 __arm64_sys_write+0x18/0x20
 el0_svc_common+0x90/0x160
 el0_svc_handler+0x68/0x80
 el0_svc+0x8/0xc
Code: bad PC value
---[ end trace 91932cc74ceb433d ]---
```