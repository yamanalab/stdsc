#!/bin/bash

BINDIR=`pwd`/build/examples/add_server

xterm -T "server"      -e "/bin/bash -c 'cd ${BINDIR}/server && ./demo_server; exec /bin/bash -i'"&
xterm -T "client"      -e "/bin/bash -c 'cd ${BINDIR}/client && ./demo_client; exec /bin/bash -i'"&
