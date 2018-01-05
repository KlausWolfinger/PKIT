#!/bin/bash
# multilevelpki tool 
# use with care, no warranty
# written for Training
# main program
# (c) 2015 K.D. Wolfinger
# www.itconsulting-wolfinger.de
# testet: 20.11.2015
#
Version='2.05'
#-------------------NO WARRANTY ----------
#BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY
#FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT WHEN
#OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES
#PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED
#OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
#MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS
#TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE
#PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING,
#REPAIR OR CORRECTION.

#--------------------changelog
# version 1.94
# add common PassPhrase variable
# define dirs for UC (use alias)
# implement certificate creation for Facade and OSMO
# version 1.91
# correct permission for certificate files in /usr/local/ssl/
# server and client pem must have 640
# dh_keys must have 640
# cleanup menu structure
# version 1.952
# SSL srx params for SOAP and CSTA based in PKI
# draw8k in OSV Menu
#--------------------changelog
# version 1.953
# write crl
# version 1.954
# revoke certificates
# UNrevoke certificates 
# version 1.955
# bugfixing, root.der
# version 1.995a
# srxdefaults parameter errors fixed
# version 1.995c
# conf Menu , pkitool.conf Infos
# READMEs
# version change 1.996 after classroomtraining
# version change 1.97d sha2 (sha256)
# version change 1.98 several improvements, code more flexible
# version change 2.01 include files seperated, new menu structure, 
# 	auto template generation, without osv mode if include does not exist
# version 2.05, debugging, new menus, rootca.p12 possible


#-------------------- todo
# prepare tool for webservice
# delete candidate
# copy file for private .key *.privatekey.pem
# copy file for rootCA .crt in *.pem
# 1. zip and copy certs and keys to /tmp (without rootCA secrets)
# compile installer for UC / tomcat / apache
# reissue certs
# delete certs from list

# ssl/tls online check
# options and vars for OSV V8
# detect osv version
# help page, howto install certs on facade, OSMo, etc..
# pki installer script
# pki distribution
# function create_keys_certs -> check for root ca first
# check fileperms in /usr/local/ssl/
# second level CA
function init(){
PKIHOME="$(echo $0 | sed 's,/[^/]\+$,,')"
PKIHOME_START="$(echo $0 | sed 's,/[^/]\+$,,')"
PKINAME=$(basename $(pwd))
# DIRS and Vars ---
PKINAME="RootCA"
BANNER="--- Project: $PKINAME V$Version --- \n \
 --- www.itconsulting-wolfinger.de ---"
DEBUGMODE=0

THISHOST=$HOSTNAME
LOGDIR=$PKIHOME/log
TMPDIR=$PKIHOME/tmp
CONFIGDIR=$PKIHOME/pkiconf
ROOTCADIR=$PKIHOME/rootca
SUBCADIR=$PKIHOME/subca
CERTSDIR=$PKIHOME/certs
OSVCERTSDIR=$PKIHOME/OSV/certs
CRLDIR=$PKIHOME/crl
KEYSDIR=$PKIHOME/keys
DHDIR=$PKIHOME/dh
HOSTDIR=$PKIHOME/host
SBINdir=$PKIHOME/sbin
CANDdir=$PKIHOME/candidates
CANDKEYSDIR=$CANDdir/CandKeys
CANDCERTSDIR=$CANDdir/CandCerts
# mkdir -m 700 -p $CANDKEYSDIR
# mkdir -m 700 -p $CANDCERTSDIR
# mkdir -m 700 -p $CandCERTSDI
SSLbackupdir=$PKIHOME/sslbackups
OSVSSLDIR="/usr/local/ssl"
# FILES ---
LOGFILE=$TMPDIR/pkitool.log
SoapLogFile=$LOGDIR/Soaplog.log
SRXInstaller=$SBINdir/srxinstaller
SRXconfig=$SBINdir/srxconfig
HOSTfile=$HOSTDIR/hosts
CandFile=$HOSTDIR/candreq
X509file=$$CONFIGDIR/x509.inf
PKIconfig=$CONFIGDIR/pkitool.conf
SubCATemplate=$CONFIGDIR/subcatemplate.inf
DATABASE=$ROOTCADIR/root-index.txt

PROMPT_1="--> $USER@$HOSTNAME : "
PROMPT_2="@$HOSTNAME[$MenuName] : "
# if config file does not exist
PassPhrase="12345678"

dos2unix $PKIHOME/sbin/*.inc > /dev/null 2>&1
dos2unix $PKIHOME/pkiconf/*.* > /dev/null 2>&1
}
init;
function libloader(){

if [ ! -e $PKIHOME/sbin/func_batcher.inc ]; then
echo "PKI include file func_batcher.inc does not exist !"
exit 1
else 
source $PKIHOME/sbin/func_batcher.inc
fi

if [ ! -e $PKIHOME/sbin/func_stuptest.inc ]; then
echo "PKI include file func_stuptest.inc does not exist !"
exit 1
else 
source $PKIHOME/sbin/func_stuptest.inc
fi

if [ ! -e $PKIHOME/sbin/functions.inc ]; then
echo "PKI include file functions.inc does not exist !"
exit 1
else 
source $PKIHOME/sbin/functions.inc
fi

if [ ! -e $PKIHOME/sbin/func_menues.inc ]; then
	echo "PKI include file func_menues.inc does not exist !"
	exit 1
else 
source $PKIHOME/sbin/func_menues.inc
fi
 
# func_unify.inc
if [ ! -e $PKIHOME/sbin/func_unify.inc ]; then
echo "PKI include file func_unify.inc does not exist !"
ToolMode="Linux"
else 
source $PKIHOME/sbin/func_unify.inc
# echo "functions func_unify.inc loaded...."
ToolMode="OSV"
# read
fi

if [ ! -e $PKIHOME/sbin/func_ca.inc ]; then
echo "PKI include file func_ca.inc does not exist !"
exit 1
else 
source $PKIHOME/sbin/func_ca.inc
# echo "functions func_ca.inc loaded...."
# read
fi

if [ ! -e $PKIHOME/sbin/func_subca.inc ]; then
echo "PKI include file func_subca.inc does not exist !"
exit 1
else 
source $PKIHOME/sbin/func_subca.inc
# echo "functions func_subca.inc loaded...."
# read
fi
 
if [ ! -e $PKIHOME/sbin/func_ra.inc ]; then
echo "PKI include file func_requests.inc does not exist !"
exit 1
else 
source $PKIHOME/sbin/func_ra.inc
# echo "functions func_requests.inc loaded...."
# read
fi
if [ ! -e $PKIHOME/sbin/func_reissuesigned.inc ]; then
echo "PKI include file func_reissuesigned.inc does not exist !"
exit 1
else 
source $PKIHOME/sbin/func_reissuesigned.inc
# echo "functions func_createtls.inc loaded...."
# read
fi

#echo "functions loaded...."
#read
}
libloader;
# init parameters
umask 0077
InitSRX=0
MenuLevel=0
let LOOPCNT=0
SysState='noosv'
# ---- MAIN ------
#echo -en "\t ---- Initializing Parameters -----\n"
InitDirs;
InitConfig;
readConfig;
read_x509config;
reInitVars;
#clear

Param1=$1
Param2=$2
Param3=$3
Param4=$4
Param5=$5

if [ $# -eq 0 ];then
	echo -en "Enter your PIN: "
	read -s PIN
	if [ $PIN -ne "0815" ]; then
		echo "Wrong PIN !!"
		exit 1
	fi
fi
# test for batch mode parameters
# 

# jump into menu loop if no ext params are given
	while [ $# -eq 0 ]; do
	menu$MenuLevel
	done;
batcher;
	echo

	
