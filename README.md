# Xenomai 3 Exercises

This repositiory contains exercises to familiarize with Xenomai 3.1.
The exercises are taken from [cs.ru.nl](http://www.cs.ru.nl/lab/xenomai/), solutions are then added.
They have been tested using a 4GB RaspberryPi 4B running Raspbian Buster with with a xenomai kernel 4.19.86 (setup instructions below).

## Exercise Topics

1. Getting Started
2. Multi Tasking
3. Semaphores
4. Preemptive Priority-Based Scheduling
5. Round Robin Scheduling
6. Priority Inversion
7. Interrupt Service Routines <sup>:spades:</sup>
8. Measuring Jitter and Latency <sup>:spades:</sup>
9. Programmable LED Clock and Rotating LEDs <sup>:spades:</sup>

:spades: : One of more exercises in those sets required hardware other than the Rhaspberry Pi which I didn't have at hand at the moment. so I couldn't provide solutions for those exercisies :(

## Preparing the RaspberryPi
The exercises have been tested using a RaspberryPi 4B running Raspbian Buster with with a xenomai kernel 4.19.86. 
A prepatched kernel and indepth installation instructions can be found at [simplerobot.net](http://www.simplerobot.net/2019/12/xenomai-3-for-raspberry-pi-4.html), but can be summarize to:

Download and deploy the prepatched kernel:
```bash
git clone https://github.com/thanhtam-h/rpi4-xeno3.git
cd rpi4-xeno3/prebuilt
chmod +x deploy.sh
./deploy.sh
```

Set up the headers:
```bash
cd /usr/src/linux-headers-4.19.86-v7l-ipipe
sudo make -i modules_prepare
```

Disable DWC features which may cause problem for ipipe kernel and modify CPU affinity. This can be adding the following somwhere at the start of `/boot/cmdline.txt` (everything containted in this file must be in a single line):
```bash
dwc_otg.fiq_enable=0 dwc_otg.fiq_fsm_enable=0 dwc_otg.nak_holdoff=0 isolcpus=0,1 xenomai.supported_cpus=0x3
```

**NOTE:** There is a big issue found on 4GB RAM version raspberry pi 4 when using ipipe kernel. A workaround is to limit usable memory to 3GB which can be done can be fixed by adding the following somwhere at the start of `/boot/config.txt`:
```bash
total_mem = 3072
```

In order to test whether your kernel is really patched with xenomai, run (after a reboot) the latency test from xenomai tool:
```bash
sudo /usr/xenomai/bin/latency
```