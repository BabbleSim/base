## Introduction

This library provides a basic set of functions for low level communication
between devices and a Phy.

Is also provides a (very rarely needed) API devices can use to communicate
directly in between them independently of the Phy.


## Basic IPC functions

Normal devices will not use this library alone, but together with one dedicated
for a particular Phy IPC, like for example the ext_2G4_libPhyComv1.

Only simple ancillary devices, like the handbrake or the time monitor, will
use this library alone as their operations are not linked to any particular Phy.

This library provides a basic API:

  * for devices to connect and disconnect from a Phy
  * for a Phy to connect to devices
  * for either to send and receive messages to the other
  * And for sending and responding to "wait" commands.
    A wait command being an indication from a device to
    the Phy, telling that it does not need to interact with
    the Phy until the point in time indicated in the command.
    And for which the Phy will respond when that point in time
    has been reached.
    For the device, both blocking and nonblocking calls to
    request a wait are provided.

Some more information can be found in the source files.


## Backchannels

This is an API simulated devices can use to communicate with each other
independently of the Phy, for example to exchange side information in between
test code.
More information can be found in the
[source file bs_pc_backchannel.c](../src/bs_pc_backchannel.c)

It is very rare a device will need to use this, as usually it will be easier
to write the device testcode without sharing status information with other
devices testcode.
