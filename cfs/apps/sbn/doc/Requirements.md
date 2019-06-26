---
title: Core Flight Software - Software Bus Networking Requirements
tags: cFE SBN
abstract: Software Requirements for the Core Flight System Software Bus
          Networking (SBN) application
---

# Software Bus Networking Requirements (cFE SBN) {#SBN}

## SBN: Application Requirements {#SBN10}

### SBN1000

SBN shall be a Core Flight Software application and comply with cFS
application interface requirements.

### SBN1001

SBN shall provide one or more network modules, each of which shall interface
with a network technology over which SBN shall transmit and receive messages
from configured SBN peers.

### SBN1001.1

SBN shall support zero or more networks, up to a configured maximum number of
networks. Each network may use any one of the network modules and SBN
shall allow multiple networks, each utilizing a different network module.

### SBN1001.2

For each configured network, SBN shall communicate with zero or more peers,
up to a configured maximum number of peers per network, per the compile-time
limit.

### SBN1002

SBN shall automatically establish and maintain connections to peers.

*Rationale: Networks may be unreliable or otherwise experience radiation-
induced errors that may interrupt the connection.*

### SBN1003

SBN shall continue to relay messages to connected peers even when some
peers in the network are not connected.

*Rationale: Networks may be unreliable, or peers may be brought online or
offline separately from this cFS instance.*

### SBN1004

SBN shall use a minimum of network and processor resources.

*Rationale: Resources are limited and valuable.*

### SBN1004.1

SBN shall interface with cFS to track all subscriptions made by local
applications and shall communicate those subscriptions to all peers.

*Rationale: Communicating the subscription information ensures that only
messages relevant to local applications are received and only messages
relevant to remote applications are sent.*

### SBN1004.2

SBN shall only send messages for peer connection awareness when required
for the network technology.

*Rationale: Some network technologies lack an infrastructure to track
whether a connection is currently/still established. In order to communicate
subscription state information, SBN needs to be aware of the state of
connections so that if a peer connection is lost and re-established,
all local subscriptions must be retransmitted. However, network resources
are limited, so only the minimum amount of traffic shall be sent to
maintain awareness.*

### SBN1004.3

When configured to do so, SBN shall filter out messages with particular
identifiers that would normally be sent to a peer.

*Rationale: By default, SBN will send all subscribed-to messages to a peer.
There may be some configurations where this would result in a great deal of
unwanted traffic.*

### SBN1005

When configured to do so, SBN shall translate local message identifiers
prior to sending to a peer. This translation shall be configurable per peer.

*Rationale: Complex spacecraft configurations may involve multiple subsystems
managed by different organizations and may have complex configuration
management. Translations will reduce the impact of subsystem changes.*

### SBN1006

All SBN headers shall be in network-byte (big-endian) order.

*Rationale: SBN may be used in an environment where some peers are of a
different byte order.*

### SBN1007

All SBN-generated protocol messages shall be in network-byte (big-endian) order.

*Rationale: SBN may be used in an environment where some peers are of a
different byte order.*

### SBN1008

SBN shall not modify messages it receives from peers and shall inject those
messages onto the local Software Bus.

*Rationale: Until EDS is implemented in cFS, understanding the complexity
of the message structures for every message would be burdensome and, once
EDS is implemented, would become redundant.*

### SBN1009

SBN shall not modify Software Bus messages it sends to peers.

*Rationale: Until EDS is implemented in cFS, understanding the complexity
of the message structures for every message would be burdensome and, once
EDS is implemented, would become redundant.*

### SBN1010

SBN shall track housekeeping information and, when commanded to do so,
transmit that information as housekeeping messages on the local Software
Bus (to be downlinked by other cFS components.)

Housekeeping statistics to be tracked:

- Valid commands received by this application.
- Invalid commands received by this application.
- Per-peer, the number of messages sent to a particular peer.
- Per-peer, the number of messages received from a particular peer.

## SBN: Operational Interface {#SBN20}

### SBN2000

SBN shall be configured by a cFS-standard compile-time configuration
header file, defining relevant upper and lower limits and other sizing
parameters.

### SBN2001

SBN network configuration shall be described in standard cFS Tables.

### SBN2002

SBN remapping/filtering configuration shall be described in standard cFS Tables.

### SBN2003

SBN shall comply with Core Flight Software application interface requirements.

### SBN2004

Upon receipt of a Command, SBN application shall generate a NO-OP event message.

*Rationale: This command is useful as a general sub-system aliveness test.*

### SBN2005

Upon receipt of a valid Command, SBN shall increment a valid Command 
counter.

*Rationale: The ground needs the capability to verify that the command was
received and accepted by SBN.*

### SBN2006

Upon receipt of an invalid Command, SBN shall increment the invalid Command 
counter and generate an event message.

*Rationale: The ground needs an indicator if a command is rejected by SBN.*

### SBN2007

Upon receipt of a Command, SBN shall send housekeeping telemetry indicating
the operating status of the application.

*Rationale: The ground needs an indicator of the current running state of SBN.*

### SBN2008

Upon receipt of a Command, SBN shall send housekeeping telemetry indicating
the current operating status regarding a particular peer.

*Rationale: The ground needs an indicator of the current running state of SBN.*

### SBN2009

Upon receipt of a Command, SBN shall set to zero the housekeeping statistics.

*Rationale: This command is a common feature in heritage sub-system software
design.*
