#!/usr/bin/env bash

make clean && make
scp bbroom-server bbroom-server.service install.sh uninstall.sh dark@darkone.top:~/bbroom-server
