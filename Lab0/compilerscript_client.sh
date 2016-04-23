#!/bin/bash
gcc catclient.c -o catclient
./catclient 172.17.0.2 8087 /data/string.txt
