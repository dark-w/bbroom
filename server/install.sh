#!/usr/bin/env bash

if [ $UID != 0 ]; then
    echo 'pls run as root!'
    exit 1
fi

rm -vrf /opt/bbroom-server
mkdir -p /opt/bbroom-server
cp -v bbroom-server *.service /opt/bbroom-server

cp -v /opt/bbroom-server/bbroom-server.service /etc/systemd/system
systemctl enable --now bbroom-server.service

echo install done

