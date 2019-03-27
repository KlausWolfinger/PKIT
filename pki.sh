#!/bin/bash
# multilevelpki tool
# use with care, no warranty
# written for Training
# main program
# (c) 2015 K.D. Wolfinger
# www.itconsulting-wolfinger.de
#
#
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


function init(){
PKIHOME="$(echo $0 | sed 's,/[^/]\+$,,')"
PKIHOME_START="$(echo $0 | sed 's,/[^/]\+$,,')"

# PKIHOME verbessern !!
## PKIHOME_START=$(dirs)

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
CERTSCACHEDIR=$PKIHOME/certscache
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
if [ ! -e $PKIHOME/sbin/func_restore.inc ]; then
echo "PKI include file func_restore.inc does not exist !"
exit 1
else
source $PKIHOME/sbin/func_restore.inc
# echo "functions func_restore.inc loaded...."
# read
fi

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


if [ $# -eq 0 ] && [ $USER != "pki" ];then
disclaimer;
read
	echo     "######################"
	echo -en "# Enter your PIN: "
	read -s PIN
	if [ $PIN -ne "0815" ]; then
	    echo -ne "\n!!!!!!!!!!!!!!!!!!!!!\n"
		echo "! Wrong PIN, sorry !!"
		exit 1
	fi
	else
	echo "# ##### enabled user $USER"
fi

startlastproject;

# jump into menu loop if no ext params are given
	while [ $# -eq 0 ]; do
	#echo "test3"
	menu$MenuLevel
	done;
batcher;
	echo
