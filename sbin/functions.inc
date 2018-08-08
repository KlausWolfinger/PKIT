# pki functions 
# use with care 
# developed for training only 
# K.D.Wolfinger 
# klaus@itconsulting-wolfinger.de 
# 
#
# ca, rootca, base and create_keys_certs 
# -------------------NO WARRANTY ---------- 
# BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY 
# FOR THEPROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW. EXCEPT WHEN 
# OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES 
# PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED 
# OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK AS 
# TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU. SHOULD THE 
# PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, 
# REPAIR OR CORRECTION. 


function disclaimer() {

echo -en "\n----- DISCLAIMER --- 
BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE,
THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY
APPLICABLE LAW. EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT
HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM \"AS IS\" WITHOUT
WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF
THE PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME
THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
------- www.itconsulting-wolfinger.de -------
!!!   DO NOT RUN THE MULTILEVELPKI SOFTWARE AS ROOT USER   !!! \n\n" 

}

function changePKILevel() { 
	# change PKIHOME clear 
	#echo -en "" 
	echo -en "----- Change the PKI project -----\n" 
	echo -en "----- This will change the tool-root directory -----\n" 
	echo -en " Switch to any project, with '99' back to root \n" 
	DirArray=($(find $PKIHOME_START -name \PKIclone\* -type d)) tLen=${#DirArray[@]} 
	echo -e "----> Found $tLen project(s) ...\n" 
	echo -en "| 99 --> \tBack to RootCA Level\n" 
	#echo -en "1 --> \t${DirArray[0]}\n" 
	DirArray[99]=$PKIHOME_START 
	for (( i=0; i<${tLen}; i++ )); do 
	echo -en "| $i --> \t${DirArray[$i]}\n" 
	done 
	# ls $CERTSDIR/*.pem 
	echo "------------------------------" 
	echo -en "\t --> Enter Number from list: " 
	read dirnum; 
	pkidir=${DirArray[$dirnum]} 
	echo -en "\n\t Your choice :$dirnum: $pkidir : [Yy]es or [Nn]o ?\n" 
	if (yes_no); then 
		#cd $pkidir; 
		PKIHOME=$pkidir; 
		#echo $PKIHOME 
		reInitVars;
		BANNER="--- Project: $PKINAME $Version --- \n--- $custbanner ---" 
		#echo -e $BANNER; 
		#echo $CONFIGDIR 
		echo " PKINAME: $PKINAME" 
		echo -e "\nChanged PKI to $pkidir \n" 
		echo $PKINAME >$PKIHOME/project 
	fi
	read 
}

function getMyIP() {
    my_ip=$(ip route get 8.8.8.8 | awk 'NR==1 {print $NF}')
	echo $my_ip
}

function reInitVars() { 
    PKIUSERHOME=$(basename $(pwd))
	if [ $PKIHOME == $PKIHOME_START ]; then 
	# echo "Debugging -- $PKIHOME" 
	PKINAME="RootCA Level 0" 
	PKITprojectName='root'
	else 
	# echo "not equal"
	PKINAME=$(basename $PKIHOME) 
	fi
	BANNER="--- $PKINAME Management $Version on $HOSTNAME ---" 
	ARCHIVEDIR=$PKIHOME_START/archives 
	EXPORTDIR=$PKIHOME_START/exports
	LOGDIR=$PKIHOME/log 
	TMPDIR=$PKIHOME/tmp 
	CONFIGDIR=$PKIHOME/pkiconf 
	ROOTCADIR=$PKIHOME/rootca
	SUBCADIR=$PKIHOME/subca	
	
	CRLDIR=$PKIHOME/crl 
	CERTSDIR=$PKIHOME/certs
	CERTSCACHEDIR=$PKIHOME/certscache
	OSVCERTSDIR=$OSVSSLDIR/certs 
	KEYSDIR=$PKIHOME/keys 
	DHDIR=$PKIHOME/dh
	HOSTDIR=$PKIHOME/host 
	SBINdir=$PKIHOME_START/sbin
	SSLbackupdir=$PKIHOME/sslbackups 
	WWWCADIR=$PKIHOME_START/www-public/roots
	WWWCERTSDIR=$PKIHOME_START/www-public/roots
	WWWDIR=$PKIHOME_START/www
	OSVSSLDIR="/usr/local/ssl"
	pkiUser=$(whoami)
	PKISHARE=/home/pkishare
	PKISHAREUSER=pkishare
	PKISHAREGROUP=pkishare
	PKISHAREFOLDER=exports
	
	
	
	
	PKIFOLDER=$(echo $PKIUSERHOME | awk '{print substr($0,1)}') 
	
	ToolIP=$(getMyIP)
    pkiAccount="$pkiUser@$ToolIP:$PKIFOLDER"
	pkiShareAccount="$PKISHAREUSER@$ToolIP:$PKISHAREFOLDER"
    OSVCertsDir=$OSVSSLDIR/certs
	OSVPrivateDir=$OSVSSLDIR/private
    OSVdhDir=$OSVSSLDIR/dh_keys
	
	CRLWEBDIR="/tmp" 
	# FILES --- LOGFILE=$TMPDIR/pkitool.log
	SoapLogFile=$LOGDIR/Soaplog.log 
	SRXInstaller=$SBINdir/srxinstaller
	SRXconfig=$SBINdir/srxconfig 
	HOSTfile=$HOSTDIR/hosts
	X509file=$CONFIGDIR/x509.inf 
	PKIconfig=$CONFIGDIR/pkitool.conf
	SubCATemplate=$CONFIGDIR/subcatemplate.inf
	DATABASE=$ROOTCADIR/root-index.txt
	. $PKIHOME_START/pkiconf/version.inf
	. $CONFIGDIR/projectname.inf
}

function ListVars() {

VarsArray=(PKIUSERHOME \
PKINAME \
PKITprojectName \
BANNER \
ARCHIVEDIR \
EXPORTDIR \
LOGDIR \
TMPDIR \
CONFIGDIR \
ROOTCADIR \
SUBCADIR \
CRLDIR \
CERTSDIR \
CERTSCACHEDIR \
OSVCERTSDIR \
KEYSDIR \
DHDIR \
HOSTDIR \
SBINdir \
SSLbackupdir \
WWWCADIR \
WWWCERTSDIR \
WWWDIR \
OSVSSLDIR \
pkiUser \
PKISHARE \
PKISHAREUSER \
PKISHAREGROUP \
PKISHAREFOLDER \
PKIFOLDER \
ToolIP \
pkiAccount \
pkiShareAccount \
OSVCertsDir \
OSVPrivateDir \
OSVdhDir \
CRLWEBDIR \
SoapLogFile \
SRXInstaller \
SRXconfig \
HOSTfile \
X509file \
PKIconfig \
SubCATemplate \
THISSCRIPT \
database \
DATABASE )
rm -f $PKIHOME_START/log/vars.log
for i in ${VarsArray[@]}; do
 cmd="echo \$$i"
 echo "$i = $(eval $cmd)" >>$PKIHOME_START/log/vars.log 
done
less $PKIHOME_START/log/vars.log

}
 
function readIP () { 
	#sudo -u root /sbin/ifconfig 
	# /sbin/ifconfig | grep "inet" | head -n2
	echo $ToolIP
	echo -en " ENTER to cont ...";read
}	
function read_fcts () { 
	# reload functions 
	source $PKIHOME/sbin/functions.inc 
} 
function ToggleDebugMode() { 
	if [ $DEBUGMODE == '0' ];then 
		DEBUGMODE=1 
		else DEBUGMODE=0
	fi
} 

function InitDirs() { 
	 
	# echo "initialize dirs files ----" 
	if [ ! -d $LOGDIR ]; then 
		mkdir $LOGDIR 
	fi
	if [ ! -d $TMPDIR ]; then 
		mkdir $TMPDIR 
	fi
	if [ ! -d $CONFIGDIR ]; then 
		mkdir $CONFIGDIR 
	fi
	# if [ ! -r $CONFIGDIR/version.inf ]; then
	# timestamp filechange is problem for git checkouts
	# touch $CONFIGDIR/version.inf
	# fi
	if [ ! -d $ROOTCADIR ]; then
		mkdir $ROOTCADIR 
	fi
	
	if [ ! -d $SUBCADIR ]; then
		mkdir $SUBCADIR
	fi
	if [ ! -d $CERTSDIR ]; then 
		mkdir $CERTSDIR 
	fi
	if [ ! -d $DHDIR ]; then 
		mkdir $DHDIR 
	fi
	if [ ! -d $KEYSDIR ]; then 
		mkdir $KEYSDIR 
	fi
	if [ ! -d $HOSTDIR ]; then 
		mkdir $HOSTDIR 
	fi
	if [ ! -d $CRLDIR ]; then 
		mkdir $CRLDIR 
	fi
	if [ ! -d $SBINDIR ]; then 
		mkdir $SBINDIR 
	fi
	if [ ! -d $SSLbackupdir ]; then 
		mkdir $SSLbackupdir 
	fi
	if [ ! -d $WWWDIR ]; then 
		mkdir -p $WWWDIR 
	fi
		#echo $WWWCADIR 
		#echo $WWWCERTSDIR 
	if [ ! -d $WWWCADIR ]; then 
		mkdir -p $WWWCADIR 
	fi
	
	if [ ! -d $WWWCERTSDIR ]; then 
		#echo "erzeuge" $WWWCADIR
		mkdir -p $WWWCADIR 
	fi
	if [ ! -d $ARCHIVEDIR ]; then 
		mkdir -p $ARCHIVEDIR
	fi
	if [ ! -d $EXPORTDIR ]; then
		mkdir -p $EXPORTDIR
	fi
	if [ ! -d $CERTSCACHEDIR ]; then
		mkdir -p $CERTSCACHEDIR
	fi
	reInitVars;
	
} 

function InitConfig() { 
	reInitVars; # init config files from templates if not exist 
	# tool does not deliever config files ! 
	if [ ! -r $CONFIGDIR/x509.inf ]; then 
		cp -p $CONFIGDIR/x509template.inf	$CONFIGDIR/x509.inf 
	fi
	if [ ! -r $CONFIGDIR/pkitool.conf ]; then 
		cp -p $CONFIGDIR/pkitooltemplate.conf $CONFIGDIR/pkitool.conf 
	fi
	if [ ! -r $HOSTDIR/hosts ]; then 
		cp -p $HOSTDIR/hosts.template $HOSTDIR/hosts 
	fi
	reInitVars;
}

function PidLock() { 
	if [ ! -d /tmp/pkitool ]; then 
		mkdir /tmp/pkitool
	fi 
	# PID and locking 
	if [ -f /tmp/pkitool/pki-tool.pid ]; then 
		echo -n "Script already running - PID :" ; 
		cat /tmp/pkitool/pki-tool.pid 
		echo "*** WARNING: unable to start new job as previous job is not finished. ***" 
		exit 1; 
		else 
		echo $$ >/tmp/pkitool/pki-tool.pid 
	fi
}
function RemoveLock() { 
	[ `cat /tmp/pkitool/pki-tool.pid` = $$ ] && rm /tmp/pkitool/pki-tool.pid ;
trap RemoveLock 0 1 2 3 9 13 15 
} 
function yes_no () { 
	while read -s -t 120 -n 1 yes_no; do 
		if [[ $yes_no = [YyNn] ]]; then 
			[[ $yes_no = [Yy] ]] && retval=0 
			[[ $yes_no = [Nn] ]] && retval=1 
			break 
		fi
		done
		return $retval 
} 

function ftpget_pki(){ 
	clear
	echo "*** Get PKI files and Tool from FTP Server ***" 
	echo "Enter FTP Server IP or Name: $FTPIP" 
	read input 
	if [ ! $input = "" ]; then
		FTPIP=$input 
	fi
	echo "Enter FTP Server Directory: $FTPDIR" 
	read input 
	if [ ! $input = "" ]; then 
		FTPDIR=/$input 
	fi
	echo "Enter FTP Server Username: $FTPUSER" 
	read input 
	if [ ! $input = "" ]; then 
		FTPUSER=$input
	fi
	echo "Enter FTP Server Password: $FTPPASS" 
	read input 
	if [ ! $input = "" ]; then 
		FTPPASS=$input 
	fi
	# echo -en " Do you want to get current PKI-Tool ? [Y|y] or [N|n]:" 
	# if (yes_no); then # PKIGET="Yes" 
	# fi echo -en "\n Your input:\n FTP Server: $FTPIP FTP Directory: $FTPDIR FTP Username: $FTPUSER" 
	# echo -en "\nUpdate PKI-Tool: $PKIGET" 
	echo -en "\n Data valid ? : " 
	if (yes_no); then 
		# Get remote files now=$(date +"%Y-%m-%d-%S") 
		filename="$X509file.$now.save" 
		cp $X509file $filename
		ftpget 
		echo "weiter" 
		read 
	fi
} 

function ftpput_pki(){ 
	clear 
	$PKIHOME/pkiconf/pkitool.conf 
	echo -en "*** Zip PKI-Folder to FTP Server ***\n" 
	#echo -en " This Feature is not implemented , ENTER to cont..";
	echo -en "--> using: IP $FTPIP \n" echo -en "--> using: DIR $FTPDIR \n"
	echo -en "--> using: USER $FTPUSER \n" echo -en "--> using: PASS $FTPPASS \n" 
	#read input 
	#echo -n "Enter FTP Server : ";
	echo -en "$FTPIP\c";
	read input; 
	#if [ ! $input = "" ]; then 
		#FTPIP=$input 
	#fi
	#echo -en " Enter FTP Server Directory: [ $FTPDIR ]";
	read input 
	#if [ ! $input = "" ]; then 
		#FTPDIR=$input 
	#fi #echo -en " Enter FTP Server Username: [ $FTPUSER ]";
	read input 
	#if [ ! $input = "" ]; then 
	#FTPUSER=$input 
	#fi 
	#echo -en " Enter FTP Server Password: [ $FTPPASS ]";
	#read input 
	#if [ ! $input = "" ]; then 
		#FTPPASS=$input 
	#fi 
	ftpput
	echo "done... [ENTER]";read 
	# read 
	#fi 
} 
function ftpget() {
ftp -n -i -v $FTPIP <<End-Of-Session
	user $FTPUSER "$FTPPASS"
	binary
	mget $FTPDIR/certs $CERTSDIR
	mget $FTPDIR/rootca/*.crt $ROOTCADIR
	mget $FTPDIR/dh $DHDIR
	mget $FTPDIR/pkiconf $X509file

	bye
End-Of-Session
} 

function ftpput() {
	#now=$(date +"%Y-%m-%d-%S")
	# pkitool*.zip
	#	pki_archive=$PKIHOME/pki_archive_$now.zip
	fileArray=($(find $PKIHOME/ -name pkitool\*.zip -type f))
	tLen=${#fileArray[@]} 
	for (( i=0; i<${tLen}; i++ )); do 
		echo -en "$i --> \t${fileArray[$i]}\n" 
	done 
	# ls $CERTSDIR/*.pem 
	echo 
	echo -en "\t --> Enter Number from list: " 
	read filenum;
	pki_archive=${fileArray[$filenum]} 
	#zip -czvpf $pki_archive $PKIHOME
	filename=$(basename $pki_archive) 
	echo -en "\t--> Now connecting ftp server: Press ENTER to continue\n" 
	echo -en "\t--> !! for escape press <CTRL> C !!\n" 
	read ftp -inv $FTPIP <<End-Of-Session 
	user $FTPUSER $FTPPASS
	binary 
	put $pki_archive $pki_archive 
	bye
End-Of-Session
} 
function mergerootca() { 
	now=$(date +"%Y-%m-%d-%S") 
	# $IMEXPORTDIR 
	# use imexport dir to cat pem files 
	# pki tool can export pem files to this dir 
	# pki OSV tool can cat for OSV root CA 
	echo -en "not implemented yet [ENTER to cont....] \n" 
	read 
} 

function PrintActiveHosts() { 
	if [ -e $HOSTDIR/hosts ]; then 
		cat $HOSTDIR/hosts 
	else 
	echo "no pki candidate file found" 
	fi
	
} 

function PrintDefaultHosts() { 
	if [ -e $HOSTDIR/hosts.template ]; then 
		less $HOSTDIR/hosts.template 
		else 
		echo "no pki candidate template found" 
	fi
	echo "any key to continue ...." read 
} 

function print_x509config(){ 
	# Print X509 Attributes ################### 
	echo -en "--- X509 Setting ---\n 
	BASEDN=$BASEDN 
	SUBDN=$SUBDN 
	ROOTBITS = $ROOTBITS 
	ROOTDAYS = $ROOTDAYS 
	ROOTCN = $ROOTCN
	ROOTSHA="$ROOTSHA"	
	ROOTCERTURI = $ROOTCERTURI 
	ROOTCRLURI = $ROOTCRLURI 
	ROOTOCSPURI = $ROOTOCSPURI 
	CERTBITS = $CERTBITS 
	CERTDAYS = $CERTDAYS 
	CERTMAIL = $CERTMAIL 
	ca_extension = $ca_extension 
	WWWCADIR= $WWWCADIR
	WWWCERTSDIR = $WWWCERTSDIR
	--> Press ENTER to continue" 
	read 
} 
function read_x509config(){ 
	if [ ! -e $CONFIGDIR/x509.inf ]; then 
		# X509 Attributes ################### 
		# we support only a uniq dh length 
		BASEDN="C=DE/L=Germany/O=CertificationAuthority/OU=PKI Management" 
		SUBDN="C=DE/L=Germany/O=MyCompany/OU=Training Department" 
		# Change DH_LENGTH to 2048 
		# !!! Important: all keylengths must be equal !!! 
		DH_LENGTH=2048 
		ROOTBITS=2048
		ROOTSHA="sha256" 
		ROOTDAYS=3650 
		ROOTCN="PKITOOL Root CA"
		ROOTCERTURI="http://ca.mycompany.de/rootca.crt"
		ROOTCRLURI="http://ca.mycompany.de/rootca.crl"
		ROOTOCSPURI="http://ca.mycompany.de/rootca.ocsp" 
		CERTBITS=2048
		CERTDAYS=1500 
		ca_extension="" 
		SHA="sha256" 
		# Parameter for Revocation Lists 
		CRLDAYS=185 
		echo -en "\tINFO: file $PKIHOME/x509.inf not found\n
		\t Default X509 Parameters loaded\n 
		\t --> Press ENTER to continue" 
		read
	fi
	# IMPORT X509 Attributes File if exists 
	if [ -e $CONFIGDIR/x509.inf ]; then 
		# echo -en "\tINFO: READ X509 Attributes from $CONFIGDIR/x509.inf \n" 
		. $CONFIGDIR/x509.inf 
		#read 
	fi
	if [ -n "$CERTMAIL" ]; then
		CERTMAIL='/emailAddress='$CERTMAIL 
	fi
	return 
} 

function checkConfig() {
	if [ ! -e $PKIconfig ]; then 
		echo "PKI Config file $PKIconfig does not exist !" 
		exit 1 
		else 
		. $PKIconfig 
		cat $PKIconfig 
	fi
} 
function editConfig() { 
	if [ ! -e $PKIconfig ]; then 
		echo "PKI Config file $PKIconfig does not exist !" 
		exit 1 
		else 
		vi $PKIconfig 
	fi
} 
 
function changeto_subca() {
echo -en "!!!! This will change this CA level !!!!\n"
echo -en "!!!! Database MUST BE initially empty \n"
# ask_subCAName;
	
sed -i 's/CALevel.*$/CALevel=$subCAName/' $X509file

}
  
function UpdateCRL() {
	. $CONFIGDIR/projectname.inf
	local crlfileext='.crl'
	local crlFileName=$PKITprojectName$crlfileext
	# re-read config 
	. $PKIconfig
	# echo -en "--- Updating CRL ...\n" 
	cat $HOSTDIR/hosts \
	| grep -v "^[ ]*#" \
	| grep -v "^[ ]*$" \
	| grep -v "^[ ]*!" \
	| while read cert; do 
	# extension is "keyword like tlsserver, tlsboth etc 
	extension=$(echo "$cert" | cut -f1 -d:) 
	CN=$(echo "$cert" | cut -f2- -d: | cut -f1 -d,) 
	#echo -en "\n--> Extension: $extension\n\n" 
	perl $SBINdir/update_index.pl $CERTSDIR/$extension/"$CN".crt $ROOTCADIR/root-index.txt
	# write crl 
	#ca | openssl -gencrl -out $CRLDIR/rootca.crl -config /dev/stdin 
	crl $ca_extension "" | openssl ca -gencrl -out $CRLDIR/$crlFileName -config /dev/stdin 
	done 
		if [ ! -d $CRLWEBDIR ]; then 
			mkdir -p $CRLWEBDIR
		fi
	cp $CRLDIR/$crlFileName $CRLWEBDIR
	
	chmod 644 $CRLWEBDIR/$crlFileName 
	echo 
	echo "*****************************************************" 
	echo "* NOTE: YOU HAVE TO DISTRIBUTE THE UPDATED CRL NOW. *" 
	echo "*****************************************************" 
	echo "**** Get the new crl file from $CRLWEBDIR/$crlFileName  ****" 
	# echo -en "\n\t--> Press ENTER to continue" 
	# read 
} 
function FastRevoke(){

crl $ca_extension "" | openssl ca -revoke $CERTSDIR/$extension/"$CN".crt -config /dev/stdin 
}
function SubCARevoke(){
SUBCADIRNAME=$SUBCADIR/$SUBCANAME
#echo "ca_extension : $ca_extension";
#echo "Cert to revoke : $SUBCADIRNAME/subca.crt"; read
crl $ca_extension "" | openssl ca -revoke $SUBCADIRNAME/subca.crt -config /dev/stdin 
}
function UnrevokeCRL() { 
	echo -en "--- UNrevoking certificates ...\n" 
	echo -en "\t ------- choose one from the list --------\n" 
	crlArray=($(cat $DATABASE \ | grep "^[ ]*R" ))
	tLen=${#crlArray[@]} 
	for (( i=0; i<${tLen}; i++ )); do 
		echo -en "$i --> \t${crlArray[$i]}\n" 
	done 
	echo 
	echo -en "\t --> Enter Number from list:" 
	read crlnum; 
	crl=${crlArray[$crlnum]}
	echo -en "\n\t Your choice for crl: $crlnum: $crl : [Yy]es or [Nn]o ?" 
	if (yes_no); then 
		echo -en "\n\t--> UNRevoking $extension from Cert store" 
		echo -en "\n\t--> writing CRL ....." 
		# start awk to change line in root-index.txt ($DATABASE) 
		echo 
		echo "*****************************************************" 
		echo "* NOTE: YOU HAVE TO DISTRIBUTE THE UPDATED CRL NOW. *" 
		echo "*****************************************************" 
		echo -en "\n ENTER to continue ..." 
		read 
	fi
} 

function RevokeCRL(){ 
	# update and revoke crl 
	echo -en "--- revoking certificates and keys from hosts file...\n" 
	echo -en "\t ------- choose one from the list --------\n"
	certArray=($(cat $HOSTDIR/hosts \
		| grep -v "^[ ]*#" \
		| grep -v "^[ ]*$" \
		| grep -v "^[ ]*!" \
		)) 
	tLen=${#certArray[@]}
	for (( i=0;i<${tLen}; i++ )); do
		echo -en "$i --> \t${certArray[$i]}\n" 
		done 
		echo
		echo -en "\t --> Enter Number from list: " 
		read certnum;
		cert=${certArray[$certnum]} 
		CN=$(echo "$cert" | cut -f2- -d: | cut -f1 -d,) 
		SALTN=$(echo "$cert" | cut -f2- -d: | grep , | cut -f2- -d,)
		extension=$(echo "$cert" | cut -f1 -d:) 
		echo -en "\n\t Your choice for cert: $certnum: $extension : [Yy]es or [Nn]o ?" 
		if (yes_no); then 
			echo -en "\n\t--> Revoking $extension from Cert store" 
			echo -en "\n\t--> writing CRL ....." 
			crl $ca_extension "" | openssl ca -revoke $CERTSDIR/$extension/"$CN".crt -config /dev/stdin 
			echo -en "\n ENTER to continue ..." 
			read 
		fi
} 

function Revoke_extcandCRL(){
	# update and revoke crl external certs only 
	echo -en "--- revoking all external certificates...\n"
	echo -en " ?? are you sure ??"
	if (yes_no); then
	certArray=($(cat $HOSTDIR/candreq \
		| grep -v "^[ ]*#" \
		| grep -v "^[ ]*$" \
		| grep -v "^[ ]*!" \
		)) 
	tLen=${#certArray[@]}
	for (( i=0;i<${tLen}; i++ )); do
		#echo -en "$i --> \t${certArray[$i]}\n" 
		cert=${certArray[$i]} 
		CN=$(echo "$cert" | cut -f2- -d: | cut -f1 -d,) 
		SALTN=$(echo "$cert" | cut -f2- -d: | grep , | cut -f2- -d,)
		extension=$(echo "$cert" | cut -f1 -d:)
		echo -en "\n\t--> Revoking $extension from Cert store" 
		echo -en "\n\t--> writing CRL ....." 
		crl $ca_extension "" | openssl ca -revoke $CANDCERTSDIR/$extension/"$CN".crt -config /dev/stdin
		done 
		fi
} 
function deleteDHFiles() { 
	echo -en "\tWARNING: This will delete DH Files\n 
	\tNOTE: You must recreate RootCA !\n 
	\tAre you sure ? [Yy|Nn]\n 
	\t--> Press Y for Yes or N for No\n" 
	if (yes_no); then
		rm -f $PKIHOME/dh/*.pem 
		echo -en "\t !!! All DH Files deleted !!!\n"
		echo -en "\t ENTER to continue\n"; read 
		else 
			echo -en "\t +++ nothing deleted or changed +++\n" 
			echo -en "\t ENTER to continue \n"; read 
	fi
}
function destroypki() { 
	echo -en "\tWARNING: This will destroy the entire PKI\n
	\tNOTE: DiffiHellman will NOT be deleted !\n 
	\tAre you sure ? [Yy|Nn]\n 
	\t--> Press Y for Yes or N for No\n" 
	if (yes_no); then 
		# mkdir -m 700 -p $ROOTCADIR 
		rm -f $ROOTCADIR/* 
		find $ROOTCADIR -type l -exec test ! -e {} \; -delete 
		rm -R -f $SUBCADIR/* 
		
		
		rm -R -f $CERTSDIR/* 
		rm -R -f $KEYSDIR/* 
		rm -f $PKIHOME/*.pem
		rm -f $PKIHOME/tmp/* 
		rm -f $PKIHOME/params/* 
		rm -f $PKIHOME/crl/*
		rm -R -f $CANDCERTSDIR/*
		rm -R -f $PKIHOME_START/restore/
		rm -R -f $PKIHOME_START/log/
		# rm -f $PKIHOME/dh/*.pem 
		rm -R -f $WWWCADIR/$RootFileName 
		# rm -R -f $WWWCERTSDIR/*
		#mkdir -m 700 -p $KEYSDIR 
		#mkdir -m 700 -p $PKIHOME/certs 
		echo -en "\t !!! All pki files deleted !!!\n" 
		echo -en "\t ENTER to continue\n"; read
		else 
			echo -en "\t +++ nothing deleted or changed +++\n" 
			echo -en "\t ENTER to continue \n"; read 
	fi
} 
function remove_archives() { 
	echo -en "\tWARNING: This will remove ALL backups\n 
	\tAre you sure ? [Yy|Nn]\n
	\t--> Press Y for Yes or N for No\n" 
	if (yes_no); then 
		echo -en "\t !!! All archives, exports and backups will be removed !!!\n" 
		echo -en "\t If you are sure, ENTER to continue\n"; read 
		rm -f $PKIHOME/*.zip 
		rm -f $ARCHIVEDIR/*.zip
		rm -f $EXPORTDIR/*.zip
		rm -f $PKIHOME/sslbackups/*.* 
			else echo -en "\t +++ nothing deleted or changed +++\n" 
			echo -en "\t ENTER to continue \n"; read 
	fi
}
function CleanupProject(){
		rm -R -f $CERTSDIR/* 
		rm -R -f $KEYSDIR/* 
		#rm -R -f $CANDCERTSDIR/*.crt
		#rm -R -f $CANDCERTSDIR/*.pem
		find $CANDCERTSDIR -name *.crt -type f -delete
		find $CANDCERTSDIR -name *.pem -type f -delete
		
		#rm -R -f $WWWCADIR/* 
		#rm -R -f $WWWCERTSDIR/* 
		rm -f $PKIHOME/crl/* 
		#rm -R -f $CANDCERTSDIR/*.crt
		rm -f $ROOTCADIR/root-index.txt 
		touch $ROOTCADIR/root-index.txt 
		echo "01" > $ROOTCADIR/root-serial 
		echo -en "\tINFO: Removed host certificates and keys, init rootca DB !!\n ";
}

function remove_allcerts() {
	#echo -en "--degugging --\n "
	echo -en "\tWARNING: This will delete ALL host certificates and keys\n 
	\tAre you sure ? [Yy|Nn]\n 
	\t--> Press Y for Yes or N for No\n" 
	if (yes_no); then 
		CleanupProject;
		#echo -en "\n\t--> Press ENTER to continue"; read 
			else 
			echo -en "\t +++ nothing deleted or changed +++\n" 
			echo -en "\t ENTER to continue \n"; read 
	fi
}
function remove_extcerts() {
	echo -en "\tWARNING: This will delete external certificates\n
	\tRequest files will not be deleted !  
	\tAre you sure ? [Yy|Nn]\n 
	\t--> Press Y for Yes or N for No\n" 
	if (yes_no); then 
		find $CANDCERTSDIR -name *.crt -type f -delete
		find $CANDCERTSDIR -name *.pem -type f -delete
		#echo -en "\n\t--> Press ENTER to continue"; read 
			else 
			echo -en "\t +++ nothing deleted or changed +++\n" 
			echo -en "\t ENTER to continue \n"; read 
	fi
}
 	
function edithosts() {
	now=$(date +"%Y-%m-%d-%S") 
	cp -p $HOSTfile $HOSTfile$now.backup 
	vi $HOSTfile 
	echo -en "\t ---- hosts file ----- \n" 
	# cat $HOSTfile read_x509config; 
	# echo -en "\n\t ENTER to continue \n" 
	# read 
	# check for alias duplicates
	checkhosts;
	
	
} 

function checkhosts() {
hostArray=($(cat $HOSTDIR/hosts \
		| grep -v "^[ ]*#" \
		| grep -v "^[ ]*$" \
		| grep -v "^[ ]*!" \
		)) 
	tLen=${#hostArray[@]}
	echo "# alias log, check for duplicates" >$HOSTDIR/hosts_alias.log
	for (( i=0;i<${tLen}; i++ )); do
		#echo -en "$i \t${hostArray[$i]}\n"
		hostalias=$(echo "${hostArray[$i]}" | cut -f2- -d: | cut -f1 -d,)
		
		echo -e "$hostalias" >>$HOSTDIR/hosts_alias.log
	done
	echo -e "\nfound $tLen candidates in file\n"
	duplicatesExist=$(sort $HOSTDIR/hosts_alias.log | uniq -cd)
	if test -n "$duplicatesExist";  then 
		echo "------- ALERT -----"
		echo "   found duplicates in alias : $duplicatesExist"
		#cat $HOSTDIR/hosts | grep "$duplicatesExist"
		echo "! CORRECT THIS FIRST, PLEASE !"
		read;
	else
		duplicatesExist=""
	fi
}

function addcand(){
# add line to candidates
echo "$newcand" >> $HOSTfile
cat $HOSTfile
}
function editcandidates() { 
	echo "filename: $CandFile" 
	vi $CandFile 
	echo -en "\t ---- candidate request file ----- \n" 
	cat $CandFile 
	echo -e "\n\t--end of file--" 
	echo -en "\n\t ENTER to continue \n"; read 
} 
function viewcandcert() { 
	clear 
	echo "View Candidate Certificate" 
	fileArray=($(find $CANDCERTSDIR -name *.pem -type f))
	tLen=${#fileArray[@]} 
	# echo "Array Length = $tLen or ${tLen}" 
	if [ ${tLen} = 0 ]; then 
		echo "no pem file found, ENTER to continue ..."; read 
			else 
				for (( i=0; i<${tLen}; i++ )); do 
					echo -en "$i --> \t${fileArray[$i]}\n" 
					done 
					echo -en "\t\n --> Enter Number from list: ";read filenum; 
					if [ $filenum -lt $tLen ]; then
						pemfile=${fileArray[$filenum]} 
						echo "========= copy from here=============" 
						cat $pemfile 
						echo "============== to here =================" 
						echo "ENTER to continue ....."; read 
						else
						echo -en "\n invalid choice ...";read;
						return 
					fi
	fi
} 


function editx509() { 
	vi $X509file 
	echo -en "\t ---- hosts file ----- \n" 
	#cat $X509file 
	#echo -en "\n\t ENTER to continue \n" ;read 
	
} 


function LessHelp() { 
	less $CONFIGDIR/help.txt 
} 
function ziptool() {
	InitDirs 
	now=$(date +"%Y-%m-%d-%S")
	cd $PKIHOME
	echo -en "\nTool will write a full archive from current project level!" 
	echo -en "\nEnter a new archivename, please:"; read ArchiveName 
	echo "$ARCHIVEDIR/pkiProject_$ArchiveName$now.zip";
	echo ".. [ENTER] to cont ..";read
	zip --encrypt -r $ARCHIVEDIR/pkiProject_$ArchiveName$now.zip . \
		-x "./*.zip" \
		-x "./sslbackups/*.*" \
		-x "./PKIclone*" \
		-x "./archives/*" \
		-x "$ARCHIVEDIR/*.*"\
		-x "./tmp*"\
		-x "./sbin*"\
		-x "./pkiconf/*"\
		-x "./sslbackups/*"\
		-x "./exports/*"\
		-x "./www-public/*"\
		-x "./www/*"\
		-x "./*.sh"\
		-x "./log/*"\
		-x "./.git/*"\
		-x "./.gitignore" \
		-x "$EXPORTDIR/*.*" >$LOGDIR/lastfullexport.log
		
	echo -en "\n ---> pkiProject archives on disk ....\n " 
	dir -all $ARCHIVEDIR/pki*.zip 
	echo -en "\ncont ... with any key ";read 
} 
function exportCAND() {
	InitDirs
	now=$(date +"%Y-%m-%d-%S")
	echo -en "\nTool will export keys and certs from this project" 
	echo -en "\nEnter a new exportname, please:"; read ExportName 
	echo "Will write $EXPORTDIR/pkiCAND_$ExportName$now.zip"
	echo "You will be asked to enter a strong password for archive encryption ";
	echo ".. [ENTER] to cont ..";
	read
	zip --encrypt -r $EXPORTDIR/pkiCAND_$ExportName$now.zip $PKIHOME/certs $PKIHOME/keys $PKIHOME/rootca/rootCA.pem
	echo -en "\n ---> pkiCAND exported to disk ....\n " 
	dir -all $EXPORTDIR/*.zip 
	if [ -d $PKISHARE ]; then 
	echo -en "\n ---> copy $EXPORTDIR to $PKISHARE"
	cp $EXPORTDIR/pkiCAND_$ExportName$now.zip $PKISHARE/exports/
	echo -en "\n You can now download the export by SFTP with user $PKISHAREUSER remotely "
	echo -en "\n The IP address of this system is $(getMyIP)"
	echo -en "\n continue ... with any key ";read
	
	else
	echo -en "\n Copied the encrypted archive to local folder $EXPORTDIR "
	fi
	# copy the zip to pkishare

}	

function clonepki() {
	clear 
	echo -e "--- PKI Tool Clone Utility ---\n" 
	echo -e "This will clone the entite pki to a new project" 
	echo -e "After cloning, switch to the new project...\n"
	echo -en "\nEnter an new pkiname, please: ";read NewPkiName
	if [ -d "$PKIHOME/PKIclone-$NewPkiName" ]; then
		echo "This clone name exists already, delete manually or creata a new one"
		echo "ENTER to continue ..."
		read
		return
	fi 
	
	# do not change order !!
	mkdir $PKIHOME/PKIclone-$NewPkiName
	mkdir -p $PKIHOME/PKIclone-$NewPkiName/keys
	cp -R -p $PKIHOME/keys $PKIHOME/PKIclone-$NewPkiName
	cp -R -p $PKIHOME/certs $PKIHOME/PKIclone-$NewPkiName
	cp -R -p $PKIHOME/host $PKIHOME/PKIclone-$NewPkiName
	cp -R -p $PKIHOME/crl $PKIHOME/PKIclone-$NewPkiName
	cp -R -p $PKIHOME/certscache $PKIHOME/PKIclone-$NewPkiName
	cp -R -p $PKIHOME/dh $PKIHOME/PKIclone-$NewPkiName
	cp -R -p $PKIHOME/pkiconf $PKIHOME/PKIclone-$NewPkiName
	echo "PKITprojectName=$NewPkiName" >$PKIHOME/PKIclone-$NewPkiName/pkiconf/projectname.inf
	mkdir -p $PKIHOME/PKIclone-$NewPkiName/rootca
	cp -R -p $PKIHOME/rootca $PKIHOME/PKIclone-$NewPkiName
	
}

function clonepki_deprecated() { 
	clear echo -e "--- PKI Tool Clone Utility ---\n" 
	echo -e "This will clone the entite pki to a new project" 
	echo -e "After cloning, switch to the new project...\n"
	echo  "Current Working DIR: "; pwd;
	echo -en "\nEnter an pkiname, please:";read NewPkiName
	mkdir $PKIHOME/PKIclone-$NewPkiName
	
	# zip -r $PKIHOME/PKIclone-$NewPkiName/PKIclone-$NewPkiName.zip $PKIHOME 
	zip -r $PKIHOME/PKIclone-$NewPkiName/PKIclone-$NewPkiName.zip . \
		-x "$PKIHOME/*.zip" \
		-x "$PKIHOME/sslbackups/*.*" \
		-x "$PKIHOME/PKIclone*/**\*" \
		-x "$PKIHOME/log/**\*" &>$LOGDIR/lastclonezip.log
	
	# Extract it all to a folder called D, but without directory structure (note this will extract files in 
	# subdirectories of D into the top-level output D folder)
	# unzip -d D -j f.zip */D/*  !! in variable $PKIHOME is a / 
	
	unzip -qq -d $PKIHOME/PKIclone-$NewPkiName/ $PKIHOME/PKIclone-$NewPkiName/PKIclone-$NewPkiName.zip  &>$LOGDIR/lastcloneunzip.log
	
	#unzip -qq -d $PKIHOME/PKIclone-$NewPkiName/ -j $PKIHOME/PKIclo#ne-$NewPkiName/PKIclone-$NewPkiName.zip *$PKIHOME/PKIclone-$NewPkiName/*  &>$LOGDIR/lastcloneunzip.log
	echo -en "Created clone successfully: in $PKIHOME/PKIclone-$NewPkiName\n" 
	echo -en "----------------------------\n"
	#dir -c $PKIHOME/PKIclone-$NewPkiName
	echo -en "\n\nENTER to continue ..." 
	read
} 
function CandAddReq() {
	clear 
	# directory: $CANDreq 
	# Add CandRequest: function CandAddReq() 
	# Parameters: 
	# - Vendor Name 
	# - Alias Name 
	# - FQDN 
	# - IP primary 
	# - IP alternative 
	# - cert usage 
}

function ListCertIndex() { 
	clear 
	echo "# --- Signed Certificates from local PKI Database:" 
	less $ROOTCADIR/root-index.txt 
	echo
	
}


