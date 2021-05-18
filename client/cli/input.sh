#!/usr/bin/env bash

while true; do
    read -p "input msg: " msg && echo $msg > client-pipe
done
