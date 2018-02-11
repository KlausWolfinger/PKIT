# func_unify.inc 
function menu1 () { 
	# osv menu 
	let LOOPCNT++
	MenuLevel=1
	MenuName="OSV"
	PROMPT_2="@$HOSTNAME[$MenuName] : "	
	clear 
	menu_header 
	echo -en "\n--------------------------------------------------
--- NO WARRANTY --- BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE,
THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY
APPLICABLE LAW. EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT
HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM \"AS IS\" WITHOUT
WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF
THE PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME
THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
---------------------------------------------------------------------
!!! Usage on OpenScape Voice Servers may lead to an outage !!! !!! This
tool is not supported or released by Unify !!! \n" 

	echo -en "\tN - Draw IP Configuration 
	\tS - Check SRX PARAMS SSL/TLS Config 
	\tb - Backup OpenScape SSL/TLS Config 
	\tC - compile srx command file 
	\tD - restore to defaults 
	\tM - Merge OpenScape rootca with other ca pem files 
	\tI - Install certificates to OSV and modify SRX Parameters 
	\tR - Replicate pemfiles to second node 
	
	\t99 - Return\n\n"
	read -p "--> $MenuLevel$PROMPT_2" userchoice 
	keypress=$(echo $userchoice | tr -d "\n") 
	Lastaction=$keypress 
	case $keypress in 
		[Nn] ) # draw8k 
			draw8k; 
		;;
		[Ss] ) #Check SSL/TLS Config 
			backup_srxtlsconf; 
		;; [Bb] ) #backup OSV TLS Config 
			ssldirbackup 
		;; 
		# new option 
		[Mm] ) # Merge root Certificates
			mergerootca; 
		;; 
		[C] ) #compile SRX PARAMS Commands 
			compile_srxcommands
		;; 
		[D] ) #restore to defaults
			restoreToDefaults 
		;; [Vv] ) # edit and reload PKI Tool Config 
			editConfig; 
			checkConfig; 
		;; 
		[I] ) #execute installer 
			execute_installer 
		;; 
		[R] ) #execute installer
			execute_pemreplic 
		;; 
		99 ) # main menu 
			echo "Back to main"
			menu0 
		;; 
		esac 
} 
function readConfig() { 
	# try to find the system type 
	if [ -e /opt/solid/bin/solcon ]; then 
		LearnDBAuth SysType=`/opt/solid/bin/solcon -e"hsb state" "tcp 127.0.0.1 16760" dba $DBPASS` 
		SysState=$(echo $SysType | sed -r 's#^.+ ([^ ]) *#\1#') 
		#echo $SysState 
	fi
	case $SysState in 
		STANDALONE) 
			osvServerType="simplex" 
			RootCAFilename=rootCA_$HOSTNAME.pem 
			ServerPEMFilename=server_$HOSTNAME.pem 
			ClientPEMFilename=client_$HOSTNAME.pem 
			;; 
		ACTIVE) 
			osvServerType="duplex"
			# clustername in /etc/hiq8000/node.cfg 
			ClusterName="Cluster"
			RootCAFilename=rootCA_$ClusterName.pem
			ServerPEMFilename=server_$ClusterName.pem
			ClientPEMFilename=client_$ClusterName.pem 
			;; 
		* ) 
			osvServerType="Linux"
			RootCAFilename=rootCA_$HOSTNAME.pem
			ServerPEMFilename=server_$HOSTNAME.pem
			ClientPEMFilename=client_$HOSTNAME.pem 
			;; 
		esac 
		if [ ! -e $PKIconfig ]; then 
			FTPUSER=sysad 
			FTPPASS=1clENtk= 
			FTPDIR="" 
			FTPIP="192.168.111.1" 
			ToolNIC="eth0"
			# todo: fqdn support 
			else 
			. $PKIconfig 
			BANNER="--- Project: $PKINAME $Version --- \n \ --- $custbanner ---" 
			ToolIP=$(hostname -I | sed 's/[ \t]*$//')
		fi
		return 
} 


function LearnDBAuth() { 
	if [ $(whoami) = 'srx' ]; then
		MY_PWD_LOC_RTP=/opt/solid/RtpDb/dba.secrets 
		if [ -e $MY_PWD_LOC_RTP ]; then 
			MY_PWD_LOC_RTP=/opt/solid/RtpDb/dba.secrets
			DBAPASSWD=`/unisphere/srx3000/callp/bin/db_crypt dec $MY_PWD_LOC_RTP`
			#DBPASS=`sed -r 's#^.+ ([^ ]) *#\1#' $DBAPASSWD` 
			DBPASS=$(echo $DBAPASSWD | sed -r 's#^.+ ([^ ]) *#\1#') 
		fi
		else 
			DBAPASSWD="Starting decryption File is binary, start decryption. 36 dba" 
		fi 
} 
function LoadSRXDefaults() {
	SSLCSTAMutualAuthServerCertificatesPath="/usr/local/ssl/certs"
	SSLCSTAMutualAuthServerDHPath="/usr/local/ssl/dh_keys"
	SSLCSTAMutualAuthServerKeys="server.pem"
	SSLCSTAMutualAuthServerKeysPath="/usr/local/ssl/private"
	SSLCSTAMutualAuthServerSupportedCertificates="root.pem"
	SSLCSTAMutualAuthServerSupportedCiphers="AES128-SHA:AES256-SHA:DES-CBC3-SHA"
	SSLCSTAMutualAuthServerSupportedDH="dh1024.pem"
	SSLSoapServerCertificatesPath="/usr/local/ssl/certs"
	SSLSoapServerKeys1="server.pem" SSLSoapServerKeys2="server.pem"
	SSLSoapServerKeys3="server.pem" SSLSoapServerKeys4="server.pem"
	SSLSoapServerKeysPath="/usr/local/ssl/private"
	SSLSoapServerSupportedCertificates="root.pem"
	SSLEndpointServerCertificatesPath="/usr/local/ssl/certs"
	SSLEndPointServerDHPath="/usr/local/ssl/dh_keys"
	SSLEndPointServerKeys="server.pem"
	SSLEndPointServerKeysPath="/usr/local/ssl/private"
	SSLEndPointServerSupportedCertificates="root.pem"
	SSLEndPointServerSupportedCiphers="AES128-SHA:AES256-SHA:DES-CBC3-SHA"
	SSLEndPointServerSupportedDH="dh1024.pem"
	SSLIdentityVerificationEnabled="RtpFalse"
	SSLMutualAuthClientCertificatesPath="/usr/local/ssl/certs"
	SSLMutualAuthClientDHPath="/usr/local/ssl/dh_keys"
	SSLMutualAuthClientKeys="client.pem"
	SSLMutualAuthClientKeysPath="/usr/local/ssl/private"
	SSLMutualAuthClientSupportedCertificates="root.pem"
	SSLMutualAuthClientSupportedCiphers="AES128-SHA:AES256-SHA:DES-CBC3-SHA"
	SSLMutualAuthClientSupportedDH="dh1024.pem"
	SSLMutualAuthServerCertificatesPath="/usr/local/ssl/certs"
	SSLMutualAuthServerDHPath="/usr/local/ssl/dh_keys"
	SSLMutualAuthServerKeys="server.pem"
	SSLMutualAuthServerKeysPath="/usr/local/ssl/private"
	SSLMutualAuthServerSupportedCertificates="root.pem"
	SSLMutualAuthServerSupportedCiphers="AES128-SHA:AES256-SHA:DES-CBC3-SHA"
	SSLMutualAuthServerSupportedDH="dh1024.pem" 
} 
function SetnewSRXParameter () {
	SSLCSTAMutualAuthServerCertificatesPath="/usr/local/ssl/certs"
	SSLCSTAMutualAuthServerDHPath="/usr/local/ssl/dh_keys"
	SSLCSTAMutualAuthServerKeys=$ServerPEMFilename
	SSLCSTAMutualAuthServerKeysPath="/usr/local/ssl/private"
	SSLCSTAMutualAuthServerSupportedCertificates=$RootCAFilename
	SSLCSTAMutualAuthServerSupportedCiphers="AES128-SHA:AES256-SHA:DES-CBC3-SHA"
	SSLCSTAMutualAuthServerSupportedDH="dh$DH_LENGTH.pem"
	SSLSoapServerCertificatesPath="/usr/local/ssl/certs"
	SSLSoapServerKeys1=$ServerPEMFilename
	SSLSoapServerKeys2=$ServerPEMFilename
	SSLSoapServerKeys3=$ServerPEMFilename
	SSLSoapServerKeys4=$ServerPEMFilename
	SSLSoapServerKeysPath="/usr/local/ssl/private"
	SSLSoapServerSupportedCertificates=$RootCAFilename
	SSLMutualAuthServerSupportedDH="dh$DH_LENGTH.pem"
	SSLEndpointServerCertificatesPath="/usr/local/ssl/certs"
	SSLEndPointServerDHPath="/usr/local/ssl/dh_keys"
	SSLEndPointServerKeys=$ServerPEMFilename
	SSLEndPointServerKeysPath="/usr/local/ssl/private"
	SSLEndPointServerSupportedCertificates=$RootCAFilename
	SSLEndPointServerSupportedCiphers="AES128-SHA:AES256-SHA:DES-CBC3-SHA"
	SSLEndPointServerSupportedDH="dh$DH_LENGTH.pem"
	SSLIdentityVerificationEnabled="RtpFalse"
	SSLMutualAuthClientCertificatesPath="/usr/local/ssl/certs"
	SSLMutualAuthClientDHPath="/usr/local/ssl/dh_keys"
	SSLMutualAuthClientKeys=$ClientPEMFilename
	SSLMutualAuthClientKeysPath="/usr/local/ssl/private"
	SSLMutualAuthClientSupportedCertificates=$RootCAFilename
	SSLMutualAuthClientSupportedCiphers="AES128-SHA:AES256-SHA:DES-CBC3-SHA"
	SSLMutualAuthClientSupportedDH="dh$DH_LENGTH.pem"
	SSLMutualAuthServerCertificatesPath="/usr/local/ssl/certs"
	SSLMutualAuthServerDHPath="/usr/local/ssl/dh_keys"
	SSLMutualAuthServerKeys=$ServerPEMFilename
	SSLMutualAuthServerKeysPath="/usr/local/ssl/private"
	SSLMutualAuthServerSupportedCertificates=$RootCAFilename
	SSLMutualAuthServerSupportedCiphers="AES128-SHA:AES256-SHA:DES-CBC3-SHA"
	SSLMutualAuthServerSupportedDH="dh$DH_LENGTH.pem" 
} 
function compile_installer(){ 
	echo -en "\t Compiles installer script to ..filename..\n" 
	echo -en "\t--> Press ENTER to continue";read 
} 
function compile_srxcommands(){ 
	if [ $InitSRX = 0 ]; then 
		echo -en "\n\t -- Preparing SRX Soap File for new settings\n" 
		SetnewSRXParameter
	else 
		echo -en "\n\t -- Preparing SRX Soap File for restore to defaults\n"
		LoadSRXDefaults 
	fi 
	dhfile=dh$DH_LENGTH.pem 
	rm -f $SRXInstaller 
	rm -f $SRXconfig 
	touch $SRXInstaller 
	touch $SRXconfig 
	echo -en "\t Compiles sxr command file..\n" 
	echo -en "\t--> Press ENTER to continue"; read 
	echo "confModifyParameter \"SSL/CSTAMutualAuth/Server/CertificatesPath\" \"$SSLCSTAMutualAuthServerCertificatesPath\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/CSTAMutualAuth/Server/DHPath\" \"$SSLCSTAMutualAuthServerDHPath\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/CSTAMutualAuth/Server/Keys\" \"$SSLCSTAMutualAuthServerKeys\"" >> $SRXconfig
	echo "confModifyParameter \"SSL/CSTAMutualAuth/Server/KeysPath\" \"$SSLCSTAMutualAuthServerKeysPath\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/CSTAMutualAuth/Server/SupportedCertificates\" \"$SSLCSTAMutualAuthServerSupportedCertificates\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/CSTAMutualAuth/Server/SupportedCiphers\" \"$SSLCSTAMutualAuthServerSupportedCiphers\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/CSTAMutualAuth/Server/SupportedDH\" \"$SSLCSTAMutualAuthServerSupportedDH\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/Soap/Server/CertificatesPath\" \"$SSLSoapServerCertificatesPath\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/Soap/Server/Keys1\" \"$SSLSoapServerKeys1\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/Soap/Server/Keys2\" \"$SSLSoapServerKeys2\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/Soap/Server/Keys3\" \"$SSLSoapServerKeys3\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/Soap/Server/Keys4\" \"$SSLSoapServerKeys4\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/Soap/Server/KeysPath\" \"$SSLSoapServerKeysPath\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/Soap/Server/SupportedCertificates\" \"$SSLSoapServerSupportedCertificates\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/EndPoint/Server/CertificatesPath\" \"$SSLEndpointServerCertificatesPath\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/EndPoint/Server/DHPath\" \"$SSLEndPointServerDHPath\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/EndPoint/Server/Keys\" \"$SSLEndPointServerKeys\"" >> $SRXconfig
	echo "confModifyParameter \"SSL/EndPoint/Server/KeysPath\" \"$SSLEndPointServerKeysPath\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/EndPoint/Server/SupportedCertificates\" \"$SSLEndPointServerSupportedCertificates\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/EndPoint/Server/SupportedCiphers\" \"$SSLEndPointServerSupportedCiphers\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/EndPoint/Server/SupportedDH\" \"$SSLEndPointServerSupportedDH\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/IdentityVerificationEnabled\" \"$SSLIdentityVerificationEnabled\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/MutualAuth/Client/CertificatesPath\" \"$SSLMutualAuthClientCertificatesPath\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/MutualAuth/Client/DHPath\" \"$SSLMutualAuthClientDHPath\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/MutualAuth/Client/Keys\" \"$SSLMutualAuthClientKeys\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/MutualAuth/Client/KeysPath\" \"$SSLMutualAuthClientKeysPath\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/MutualAuth/Client/SupportedCertificates\" \"$SSLMutualAuthClientSupportedCertificates\"" >> $SRXconfig
	echo "confModifyParameter \"SSL/MutualAuth/Client/SupportedCiphers\" \"$SSLMutualAuthClientSupportedCiphers\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/MutualAuth/Client/SupportedDH\" \"$SSLMutualAuthClientSupportedDH\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/MutualAuth/Server/CertificatesPath\" \"$SSLMutualAuthServerCertificatesPath\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/MutualAuth/Server/DHPath\" \"$SSLMutualAuthServerDHPath\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/MutualAuth/Server/Keys\" \"$SSLMutualAuthServerKeys\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/MutualAuth/Server/KeysPath\" \"$SSLMutualAuthServerKeysPath\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/MutualAuth/Server/SupportedCertificates\" \"$SSLMutualAuthServerSupportedCertificates\"" >> $SRXconfig
	echo "confModifyParameter \"SSL/MutualAuth/Server/SupportedCiphers\" \"$SSLMutualAuthServerSupportedCiphers\"" >> $SRXconfig 
	echo "confModifyParameter \"SSL/MutualAuth/Server/SupportedDH\" \"$SSLMutualAuthServerSupportedDH\"" >> $SRXconfig 
	echo -en "\n\t--> SRX Parameter config prepared in $SRXconfig\n" 
	echo -en "\t ENTER to continue\n"; read 
} 
function view_installer(){ 
	echo -en "\t -- Installer Script --\n" 
	echo -en "\t--> Press ENTER to continue"; read 
} 
function restoreToDefaults(){ 
	InitSRX=1 
	LoadSRXDefaults 
	now=$(date +"%Y-%m-%d-%S") 
	filename="$SoapLogFile.$now.log" 
	compile_srxcommands
	echo -en "\n\t --- Wrote srxlog to $filename ---\n"; 
	echo -en "\n\t Restore TLS to default-->\n" 
	echo -en "\t Are you sure ?"
	if (yes_no); then 
		RtpAdmCli -p 40 -l sysad -x >> $filename 2>&1 <<-!
		batch "$SRXconfig" exit 
		!
		echo -en "\n\t OSV with default PKI supplied\n" 
		echo -en "\t Reinit the RTP with stxctrl 3 x -> srxctrl 4 x\n" 
		echo -en "\t ENTER to continue\n"; read 
	fi 
	InitSRX=0 
} 
function execute_pemreplic(){
	echo "Copying pem files to neighbour node" 
	echo -n "Enter neighbour node management IP address: ";read IP_Node2; 
	echo -en "\n\t Which root certificate do you want to copy ?\n" 
	echo -en "\t ------- choose one from the list --------\n" 
	fileArray=($(find $OSVSSLDIR/certs -name *.pem -type f)) 
	tLen=${#fileArray[@]} 
	for (( i=0; i<${tLen}; i++ )); do 
		echo -en "$i --> \t${fileArray[$i]}\n" 
	done 
	echo echo -en "\t --> Enter Number from list: "; read filenum; 
	pemfile=${fileArray[$filenum]} 
	echo -en "\n\t Your choice for root cert pemfile: $filenum: $pemfile : [Yy]es or [Nn]o ?" 
	if (yes_no); then 
		echo -en "\n\t--> Copying $pemfile to neighbor node" 
		echo -en "\n\t--> scp $pemfile srx@$IP_Node2:$pemfile"
		scp $pemfile srx@$IP_Node2:$pemfile 
	fi 
	echo -en "\n\t Which server certificate do you want to copy ?\n" 
	echo -en "\t ------- choose one from the list --------\n" 
	fileArray=($(find $OSVSSLDIR/private -name *.pem -type f)) 
	tLen=${#fileArray[@]} 
	for (( i=0; i<${tLen}; i++ )); do
		echo -en "$i --> \t${fileArray[$i]}\n" 
	done 
	echo echo -en "\t --> Enter Number from list: "; read filenum; 
	pemfile=${fileArray[$filenum]} 
	echo -en "\n\t Your choice for server pemfile: $filenum: $pemfile : [Yy]es or [Nn]o ?" 
	if (yes_no); then 
		echo -en "\n\t--> Copying $pemfile to neighbor\n" 
		echo -en "\n\t--> scp $pemfile srx@$IP_Node2:$pemfile" 
		scp $pemfile srx@$IP_Node2:$pemfile 
	fi 
	echo -en "\n\t Which client certificate do you want to copy ?\n" 
	echo -en "\t ------- choose one from the list --------\n" 
	fileArray=($(find $OSVSSLDIR/private -name *.pem -type f)) 
	tLen=${#fileArray[@]} 
	for (( i=0; i<${tLen}; i++ )); do 
		echo -en "$i --> \t${fileArray[$i]}\n" 
	done 
	echo 
	echo -en "\t --> Enter Number from list: "; read filenum;
	pemfile=${fileArray[$filenum]} 
	echo -en "\n\t Your choice for client pemfile: $filenum: $pemfile : [Yy]es or [Nn]o ?" 
	if (yes_no); then 
		echo -en "\n\t--> Copying $pemfile to neighbor\n" 
		echo -en "\n\t--> scp $pemfile srx@$IP_Node2:$pemfile" 
		scp $pemfile srx@$IP_Node2:$pemfile
		echo -en "\n\t--> ENTER to continue"; read 
	fi
	echo -en "\n\t Which dh pemfile do you want to copy ?\n" 
	echo -en "\t ------- choose one from the list --------\n" 
	fileArray=($(find $OSVSSLDIR/dh_keys -name *.pem -type f)) 
	tLen=${#fileArray[@]} 
	for (( i=0; i<${tLen}; i++ )); do
		echo -en "$i --> \t${fileArray[$i]}\n" 
	done 
	echo echo -en "\t --> Enter Number from list: "; read filenum; 
	pemfile=${fileArray[$filenum]} 
	echo -en "\n\t Your choice for dh pemfile: $filenum: $pemfile : [Yy]es or [Nn]o ?" 
	if (yes_no); then 
		echo -en "\n\t--> Copying $pemfile to neighbor\n" 
		echo -en "\n\t--> scp $pemfile srx@$IP_Node2:$pemfile" 
		scp $pemfile srx@$IP_Node2:$pemfile 
		echo -en "\n\t--> ENTER to continue";read 
	fi 
} 
function execute_installer(){ 
	# todo some tests first 
	# check for writeable in osvdirs 
	# check for tool files if OSV certs exist 
	# select OSVCERTSDIR only for pem disitibution to OSV 
	# ----- 
	now=$(date +"%Y-%m-%d-%S") 
	filename="$SoapLogFile.$now.log" 
	echo -en "\n\t Execute Installer -->\n" 
	ssldirbackup; 
	echo -en "\t Execute SRX PARAMS BACKUP -->\n" 
	backup_srxtlsconf; 
	echo -en "\t--> Now copying certificates: Press ENTER to continue\n" 
	echo -en "\t--> !! for escape press <CTRL> C !!\n";read 
	echo -en "\t--> Copying $ROOTCADIR/root.crt to $OSVSSLDIR/certs/$RootCAFilename\n" 
	cp $OSVSSLDIR/certs/root.pem $OSVSSLDIR/certs/root.pem.$now.save 
	cp $ROOTCADIR/root.crt $OSVSSLDIR/certs/$RootCAFilename 
	if [ $(whoami) = 'root' ]; then 
		chown srx.rtpgrp $OSVSSLDIR/certs/$RootCAFilename 
	fi 
	chmod 640 $OSVSSLDIR/certs/$RootCAFilename 
	echo -en "\t--> Copying $DHDIR/dh$DH_LENGTH.pem to $OSVSSLDIR/dh_keys/dh$DH_LENGTH.pem\n" 
	cp $DHDIR/dh$DH_LENGTH.pem $OSVSSLDIR/dh_keys/dh$DH_LENGTH.pem 
	if [ $(whoami) = 'root' ]; then 
		chown srx.rtpgrp $OSVSSLDIR/dh_keys/dh$DH_LENGTH.pem 
	fi 
	chmod 644 $OSVSSLDIR/dh_keys/dh$DH_LENGTH.pem 
	echo -en "\n\t Which server certificate do you want to install to OSV ?\n" 
	echo -en "\t ------- choose one from the list --------\n" 
	echo -en "\t -----choose the *chain.pem if duplex ----\n" 
	OSVCERTSDIR=$CERTSDIR/osvserver/
	fileArray=($(find $OSVCERTSDIR -name *.pem -type f))
	tLen=${#fileArray[@]} 
	for (( i=0; i<${tLen}; i++ )); do 
		echo -en "$i --> \t${fileArray[$i]}\n" 
	done 
	# ls $CERTSDIR/*.pem 
	echo 
	echo -en "\t --> Enter Number from list: "; read filenum; 
	pemfile=${fileArray[$filenum]}
	echo -en "\n\t Your choice for server pemfile: $filenum: $pemfile : [Yy]es or [Nn]o ?"
	if (yes_no); then 
		echo -en "\n\t--> Copying $pemfile to $OSVSSLDIR/private/$ServerPEMFilename\n" 
		cp $pemfile $OSVSSLDIR/private/$ServerPEMFilename 
		#cp $OSVCERTSDIR/$pemfile $OSVSSLDIR/private/$ServerPEMFilename 
		#chown root.rtpgrp $OSVSSLDIR/private/$ServerPEMFilename 
		chmod 600 $OSVSSLDIR/private/$ServerPEMFilename 
	fi 
	echo -en "\n\t Which client certificate do you want to install to OSV ?\n"
	echo -en "\t ------- choose one from the list --------\n" 
	OSVCERTSDIR=$CERTSDIR/osvclient/
	fileArray=($(find $OSVCERTSDIR -name *.pem -type f))
	tLen=${#fileArray[@]} 
	for (( i=0; i<${tLen}; i++ )); do 
		echo -en "$i --> \t${fileArray[$i]}\n" 
	done 
	# ls $CERTSDIR/*.pem 
	echo 
	echo -en "\t --> Enter Number from list: "; read filenum; 
	pemfile=${fileArray[$filenum]}
	echo -en "\n\t Your choice for client pemfile: $filenum: $pemfile : [Yy]es or [Nn]o ?" 
	if (yes_no); then 
		echo -en "\n\t--> Copying $pemfile to $OSVSSLDIR/private/$ClientPEMFilename\n" 
		cp $pemfile $OSVSSLDIR/private/$ClientPEMFilename 
		#cp $OSVCERTSDIR/$pemfile $OSVSSLDIR/private/$ClientPEMFilename 
		#chown root.rtpgrp $OSVSSLDIR/private/$ClientPEMFilename 
		chmod 600 $OSVSSLDIR/private/$ClientPEMFilename 
	fi 
	echo -en "\n\t...please wait ... writing new SRX parameters ...\n\n" 
	echo $(date) >> $filename; 
	### SRX PARAMS to be set again ?? 
	compile_srxcommands 
	RtpAdmCli -p 40 -l sysad -x >> $filename 2>&1 <<-!
	batch "$SRXconfig" exit 
	!
	cd $OSVSSLDIR/private 
	c_rehash $PWD 
	cd $OSVSSLDIR/certs 
	c_rehash $PWD 
	cd 
	echo -en "\n\t --- Wrote srxlog to $filename ---\n"; 
	echo -en "\n\t OSV with new PKI supplied\n" 
	echo -en "\n\t --- Exit the Tool now with <CTRL>-C --- and " 
	echo -en "\t Reinit the RTP with srxctrl 3 x -> srxctrl 4 x\n"
	echo -en "\t <ENTER> to continue or <CTRL>-D to exit\n"; read
} 

function draw8k(){ 
	/usr/bin/draw8k; 
	echo "done... Press [ENTER]";
	read 
} 

function ssldirbackup(){ 
	# todo list 
	# only dirs with rtpgrp read perms 
	now=$(date +"%Y-%m-%d-%S")
	filename="$SSLbackupdir/sslbackup.$now.tar.gz" 
	# $SSLbackupdir 
	tar -czvpf $filename /usr/local/ssl 
	echo -en "\n\t--- SSL/TLS Backup ---\n"
	echo -en "\t Output saved in $filename\n" 
	echo -en "\t--> Press ENTER to continue" 
	read 
} 

function backup_srxtlsconf(){ 
	now=$(date +"%Y-%m-%d-%S") 
	filename="$SoapLogFile.$now.log" 
	echo -en "...please wait ... invoking StartCli ...\n\n" 
	RtpAdmCli -p 40 -l sysad -x > $filename 2>&1 <<-!
	batch "$CONFIGDIR/osv_read_ssl_rtp_parameters.txt"
	exit
	!
	grep 'name:\|value:' $filename 
	echo -en "\n\t--- SSL/TLS Systemcheck ---\n" 
	echo -en "\t Output saved in $filename\n" 
	echo -en "\t--> Press ENTER to continue" 
	read 
}

#shell script compiler for OSV	
function osvscripter () {
echo "#!/bin/bash" >$CERTSDIR/$extension/osvpkiactivater.sh
echo "# su -srx" >>$CERTSDIR/$extension/osvpkiactivater.sh
echo "scp $pkiAccount"$ROOTCADIR"/rootCA.pem /tmp/rootCA.pem &&" >>$CERTSDIR/$extension/osvpkiactivater.sh
echo "scp $pkiAccount"$CERTSDIR"/osvserver/osv_cluster_s.pem /tmp/server.pem &&" >>$CERTSDIR/$extension/osvpkiactivater.sh
echo "scp $pkiAccount"$CERTSDIR"/osvclient/osv_cluster_c.pem /tmp/client.pem &&" >>$CERTSDIR/$extension/osvpkiactivater.sh
echo "scp $pkiAccount"$DHDIR"/dh2048.pem /tmp/dh2048.pem;" >>$CERTSDIR/$extension/osvpkiactivater.sh
echo "read "ENTER"" >>$SBINdir/osvpkiactivater.sh
echo "cp -p "$OSVCertsDir"/root.pem "$OSVCertsDir"/root.pem.save" >>$CERTSDIR/$extension/osvpkiactivater.sh
echo "cat /tmp/rootCA.pem >"$OSVCertsDir"/root.pem" >>$CERTSDIR/$extension/osvpkiactivater.sh
echo "cp -p "$OSVPrivateDir"/server.pem "$OSVPrivateDir"/server.pem.save" >>$CERTSDIR/$extension/osvpkiactivater.sh
echo "cat /tmp/server.pem >"$OSVPrivateDir"/server.pem" >>$CERTSDIR/$extension/osvpkiactivater.sh
echo "cp -p "$OSVPrivateDir"/client.pem "$OSVPrivateDir"/client.pem.save" >>$CERTSDIR/$extension/osvpkiactivater.sh
echo "cat /tmp/client.pem >"$OSVPrivateDir"/client.pem" >>$CERTSDIR/$extension/osvpkiactivater.sh
echo "cp -p "$OSVdhDir"/dh1024.pem "$OSBdhDir"/dh2048.pem" >>$CERTSDIR/$extension/osvpkiactivater.sh
echo "cat /tmp/dh2048.pem >"$OSVdhDir"/dh2048.pem" >>$CERTSDIR/$extension/osvpkiactivater.sh
echo "echo repeat this on node 2" >>$CERTSDIR/$extension/osvpkiactivater.sh
echo "echo change srx params for dh key file" >>$CERTSDIR/$extension/osvpkiactivater.sh
}

# keystore changer for CMP Server
function keystorechanger () {
# create script
echo "#!/bin/bash" >$CERTSDIR/$extension/cmpkeyhashreplacer.sh
echo "P12PassPhrase="$PassPhrase >>$CERTSDIR/$extension/cmpkeyhashreplacer.sh
echo "CMPserverXMLFile=/opt/siemens/share/tomcat/conf/server.xml " >>$CERTSDIR/$extension/cmpkeyhashreplacer.sh
echo "CMPtrustStorePropertiersFile=/opt/siemens/common/conf/application-external/trust-store.properties " >>$CERTSDIR/$extension/cmpkeyhashreplacer.sh
	# --- first ----
echo "Line=\$(egrep -n tomcat-keystore.p12" "$CMPserverXMLFile | cut -d\":\" -f1)" >>$CERTSDIR/$extension/cmpkeyhashreplacer.sh
echo "LineNr=\$(( $Line + 1 )) " >>$CERTSDIR/$extension/cmpkeyhashreplacer.sh
echo "PassHash=\$(/opt/siemens/servicetools/cmp/encodePassphrase.sh $P12PassPhrase) " >>$CERTSDIR/$extension/cmpkeyhashreplacer.sh
echo "NewLine='keystorePass="'$PassHash'"' " >>$CERTSDIR/$extension/cmpkeyhashreplacer.sh
echo "sed -i -e \$(echo $LineNr)c"$NewLine " $CMPserverXMLFile " >>$CERTSDIR/$extension/cmpkeyhashreplacer.sh
	# --- second ---
echo "LineNr=\$(egrep -n \"keystorePass\" $CMPtrustStorePropertiersFile | cut -d\":\" -f1) " >>$CERTSDIR/$extension/cmpkeyhashreplacer.sh
echo "sed -i -e \$(echo $LineNr)c"$NewLine" $CMPtrustStorePropertiersFile " >>$CERTSDIR/$extension/cmpkeyhashreplacer.sh


}
			
