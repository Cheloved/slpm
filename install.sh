#!/bin/bash

# Create necessary folders
[ ! -d /var/slpm ] && mkdir /var/slpm
[ ! -d /var/slpm/cache ] && mkdir /var/slpm/cache
[ ! -d /var/slpm/installed ] && mkdir /var/slpm/installed
[ ! -d /var/slpm/tmp ] && mkdir /var/slpm/tmp

# Copy executable
[ -f /usr/bin/slpm ] && rm /usr/bin/slpm
cp ./slpm /usr/bin/slpm

# Copy default config if not exists
[ ! -f /etc/slpm.conf ] && cp ./slpm.conf /etc/slpm.conf
