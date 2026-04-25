This device slows down a simulation close to a ratio of the real time 
By default it will slow it down to real time speed

The ratio between simulation time and real time can be set with 
-r=<real_time_ratio>

Note that this device actually _stalls_ the simulation every <poke_period>.

Note2: Poking incurres in quite a lot of overhead, it will be difficult 
to keep realtimeness if the poking is 2ms or less.

Note3: The simulation will run for one poke period before the handbrake starts.
So expect to see simulations to last approx. 1 poke period less than specified

Note4: If the host machine is too slow to keep up with the requested ratio,
the handbrake will not stall the simulation. In that case a warning will be
printed at verbosity level 4 indicating how far behind real time the
simulation is lagging.

Run with --help for more information