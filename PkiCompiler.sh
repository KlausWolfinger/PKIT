#!/bin/bash
clear
echo "pki script compiler"

#Der Shellcompiler auf der Kali 
# -r relax security, redistributable


CFLAGS=-static; shc -r -m "contact: www.itconsulting-wolfinger.de" -r -f pki.sh 

#CFLAGS=-static; shc -r -e 31/08/2015 -m "contact: www.itconsulting-wolfinger.de" -r -f pki.sh 
mv pki.sh.x pki
chmod u+x pki
echo "done ...."
file pki


 