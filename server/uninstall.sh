#!/usr/bin/env bash

if [ $UID != 0 ]; then
    echo 'pls run as root'
    exit 1
fi

systemctl stop bbroom-server.service
rm -vf /etc/systemd/system/bbroom-server.service
rm -vrf /opt/bbroom-server
systemctl daemon-reload

echo uninstall done

