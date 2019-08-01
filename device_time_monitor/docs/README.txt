This device just connects to a phy and prints the time as it passes
(for example if you want to monitor the progress of long simulations)

How often the speed is measured and printed out is configured with
the command line option `-interval=<interval>`
where `interval` is a value in microseconds.
Note that monitoring adds overhead to the simulation, therefore do not set too
small values (i.e. not smaller than 50ms)
