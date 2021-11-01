# production 1
function base() { 
	cat <<-EOF
	[ req ] 
	distinguished_name = req_distinguished_name 
	[ req_distinguished_name ] 
	EOF
} 
function rootca() { 
	echo "--- initializing x509 config file ...">>$LASTLOG
	reInitVars
	echo "--- creating rootca -- $(date)" >>$LASTLOG
	echo " commonName = $ROOTCN" >>$LASTLOG
	echo " emailAddress = $BASEMAIL" >>$LASTLOG
	echo " POLICYIDENTIFIER = $POLICYIDENTIFIER" >>$LASTLOG
	echo " ROOTCRLURI = $ROOTCRLURI " >>$LASTLOG
	
    cat <<-EOF
	# --- new 
	[ req ]
	default_md = $ROOTSHA
	basicConstraints=CA:TRUE,pathlen:0
	keyUsage = nonRepudiation, digitalSignature, keyEncipherment
	distinguished_name = req_distinguished_name
	attributes = req_attributes	
	
	[ req_distinguished_name ]
	commonName	= $ROOTCN # (e.g. server FQDN or YOUR name)
	commonName_default	= $ROOTCN
	commonName_max	= 64

	emailAddress = $BASEMAIL
	emailAddress_default  = $BASEMAIL
	emailAddress_max = 64
	
	[ req_attributes ]
	challengePassword = $ChallPassWD
	challengePassword_min = 4
	challengePassword_max = 20
	
	# ---- new end
	# nur dieser Block wird zur CA Erstellung verwendet	
	[ v3_ca ]
	#policy = policy_PKIT
	#randfile=$ROOTCADIR/root-random
	subjectKeyIdentifier=hash
	EOF
	
	if [ ! -z $POLICYIDENTIFIER ]; then
	echo "POLICYIDENTIFIER = $POLICYIDENTIFIER found ..." >>$LASTLOG 
	cat <<-EOF
	
	certificatePolicies=ia5org,@certpol0_sect
	
	EOF
	fi
	
	if [ ! -z $ROOTCRLURI ]; then
	echo "ROOTCRLURI = $ROOTCRLURI found ..." >>$LASTLOG 
	cat <<-EOF
	crlDistributionPoints=crlDistributionPoint0_sect
	EOF
	fi
	
	
	cat <<-EOF
	#authorityKeyIdentifier=keyid:always,issuer:always
	authorityKeyIdentifier=keyid:always 
	# -------------------
	# hier ein Problem mit OSV ??
	# cert policy full ??
	# test OSV default cert: no critical, no length, failed also for full
	basicConstraints=CA:TRUE,pathlen:0
	keyUsage = critical, cRLSign, keyCertSign
	# -------------------
	[certpol0_sect]
	policyIdentifier = $POLICYIDENTIFIER
	CPS.0 = $CERTPOL
	userNotice.1=@certpol0_sect_notice1_sect
	[certpol0_sect_notice1_sect]
	explicitText=Diese CA wurde mit dem PKITOOL V$Version (www.pkitool.de) erzeugt. 
	
	
	[crlDistributionPoint0_sect]
	fullname = URI:$ROOTCRLURI


	[ policy_PKIT ]
	countryName		= match
	stateOrProvinceName		= optional
	localityName			= optional
	organizationName		= supplied
	organizationalUnitName	= optional
	commonName				= supplied
	emailAddress			= optional
	
	EOF
} 

function subrootca() {
cat <<-EOF
	
[ new_oids ]
tsa_policy1 = 1.2.3.4.1
tsa_policy2 = 1.2.3.4.5.6
tsa_policy3 = 1.2.3.4.5.7

####################################################################


####################################################################
[ req ]
req_extensions = v3_req
distinguished_name	= req_distinguished_name

[ req_distinguished_name ]
[ v3_req ]
# Extensions to add to a certificate request
basicConstraints=CA:TRUE,pathlen:0
keyUsage = nonRepudiation, digitalSignature, keyEncipherment


[bsipol]
policyIdentifier=$POLICYIDENTIFIER
CPS=$CPSPOL

[verisignev]
policyIdentifier='2.16.840.1.113733.1.7.23.6'
CPS=$CPSEV
userNotice.1=@verisignevnotice

[verisignevnotice]
explicitText="Nur fuer den im Kunden-Netz internen Gebrauch! Von Symantec Class 3 EV SSL CA - G3 uebernommene extended validation OID."
organization="PKI Management Kunde"


[tlkatlscrls]
fullname=URI:http://pkit.kunde.de/pki/ca/ca.crl


[tlkatlsaia]
caIssuers;URI  = http://pkit.kunde.de/pki/ca/ca.cer
OCSP;URI      = http://pkitocsp.kunde.de/ca




	EOF
}
 
function crl() {  
	reInitVars
	base 
	cat <<-EOF
	[ ca ] 
	default_ca = CA_default 
	[ CA_default ] 
	private_key = $ROOTCADIR/root.key 
	certificate = $ROOTCADIR/root.crt 
	new_certs_dir = $CERTSCACHEDIR 
	database = $ROOTCADIR/root-index.txt 
	default_md = $CRLSHA
	policy = policy_anything 
	serial = $ROOTCADIR/root-serial 
	certs = $CERTSCACHEDIR 
	RANDFILE = $ROOTCADIR/root-random 
	#crl_dir = $CRLDIR 
	crl = $CRLDIR/rootca.crl 
	default_crl_days = $CRLDAYS 
	[ policy_anything ] 
	EOF

} 
function create_dh(){
	reInitVars
	# Diffie-Hellman-Parameter erstellen 
	for length in $ROOTBITS $CERTBITS; do 
		if [ ! -e $DHDIR/dh$length.pem ]; then
			openssl dhparam -out $DHDIR/dh$length.pem $length 
		fi
	done 
} 

function remove_rootcaKey_passphrase(){
# passphrase remover
if [ -e $ROOTCADIR/root.key ]; then
echo
	echo -en "--> $ROOTCADIR/root.key found...\n";
	echo -en " !!! You want to remove the passphrase ? !!!\n"
	echo -en "Sure ? [Y]es or [N]o  ...\n"
	if (yes_no);then
	
		_encrypted=$(grep ENCRYPTED $ROOTCADIR/root.key)
		if [ "$_encrypted" != '' ]; then
			echo "The rootCA key is encrypted, enter the passphrase, please:"
			echo
			cp -p $ROOTCADIR/root.key $ROOTCADIR/root.key.save.encrypted
			openssl rsa -in $ROOTCADIR/root.key.save.encrypted -out $ROOTCADIR/root.key.unencrypted
			echo "removed the passphrase from root keyfile sucessfully .."
			echo "Enter any key to continue ...."
			read
			cat $ROOTCADIR/root.key.unencrypted >$ROOTCADIR/root.key
			else
			echo "the rootca keyfile is NOT encryted ..."
			echo "nothing to do ...."
			echo "Any key to continue ..."
			read
			return
		fi
	else
		return
	fi
	
else
	echo
	echo -en "--> $ROOTCADIR/root.key check failed...\n";
	echo -en " !!! There is no keyfile !!!\n"
	echo -en " ... create or import the keys first \n"
	echo -en "ANY KEY to continue ...\n"
	read
fi

}

function set_rootcaKey_passphrase(){
# add a passphrase 
if [ -e $ROOTCADIR/root.key ]; then
echo
	echo -en "--> $ROOTCADIR/root.key found...\n";
	echo -en " !!! You want to create a passphrase? !!!\n"
	echo -en "Sure ? [Y]es or [N]o  ...\n"
	if (yes_no);then
	
		_encrypted=$(grep ENCRYPTED $ROOTCADIR/root.key)
		if [ "$_encrypted" = '' ]; then
			echo "The rootCA key is cleartext, enter the passphrase, please:"
			echo
			cp -p $ROOTCADIR/root.key $ROOTCADIR/root.key.save.unencrypted
			openssl rsa -aes128 -in $ROOTCADIR/root.key.save.unencrypted -out $ROOTCADIR/root.key.encrypted
			echo "added passphrase to root keyfile sucessfully .."
			echo "Enter any key to continue ...."
			read
			cat $ROOTCADIR/root.key.encrypted >$ROOTCADIR/root.key
			else
			echo "the rootca keyfile was ALREADY encryted ..."
			echo "nothing to do ...."
			echo "Any key to continue ..."
			read
			return
		fi
	else
		return
	fi
	
else
	echo
	echo -en "--> $ROOTCADIR/root.key check failed...\n";
	echo -en " !!! There is no keyfile !!!\n"
	echo -en " ... create or import the keys first \n"
	echo -en "ANY KEY to continue ...\n"
	read
fi

}




function recreate_dh(){
	reInitVars
	rm -f $DHDIR/*.pem
	# Diffie-Hellman-Parameter erstellen 
	for length in $ROOTBITS $CERTBITS; do 
		if [ ! -e $DHDIR/dh$length.pem ]; then
			openssl dhparam -out $DHDIR/dh$length.pem $length 
		fi
	done 
} 

function create_rootCAkeys(){
	# check rootCA and DB
	#
	echo -en "\n--> This will perform a rootCA check ...\n"
	if [ -e $ROOTCADIR/root.key ] || [ -e $ROOTCADIR/root.crt ]; then
		hash_rootcrt=$(sha1sum $ROOTCADIR/root.crt | awk '{print $1}')
		echo -en "--> $ROOTCADIR/root.crt check OK...\n"
		echo -en " !!! We don't want to destroy the RootCA !!!\n"
		echo -en " ... to replace the rootCA keys you have to \n"
		echo -en "     destroy the project first and create the keys\n"
		echo -en "ANY KEY to continue ...\n"
		read
		return
	else 
		echo -en "there is no rootCA, good, so we can continue\n"
		echo -en "Are you sure? [Y or N]"
	 	if (yes_no); then
			echo -en "... this is a beta feature under test ...\n"; 
			echo 
			echo -en "--> with the next step PKIT will open the file root.key\n"
			echo -en "    which is a base64 pem format. After opening with\n"
			echo -en "    vi editor, paste the new master key from your crypto box\n";
			echo -en "    into the file and save. Next step is to create the rootCA\n"
			echo -en "UNDERSTOOD [Y or N]?"
				if (yes_no); then
					vi $ROOTCADIR/root.key
				else 
					return
				fi
		else 
			return
		fi
	fi
}

function create_rootCA_DB() {
reInitVars

mkdir -m 700 -p $ROOTCADIR 
	export CN 
	
	if [ ! -e $ROOTCADIR/root-serial -o ! -e $ROOTCADIR/root-index.txt -o ! -e $ROOTCADIR/root.key ]; then 
		rm -f $ROOTCADIR/*.* 
		rm -f $CERTSDIR/*.*
		touch $ROOTCADIR/rootca.conf
		echo "## CA CONFIG " >>$ROOTCADIR/rootca.conf
		echo "## DO NOT CHANGE !!" >>$ROOTCADIR/rootca.conf
		echo "## CREATED AUTOMATED " >>$ROOTCADIR/rootca.conf
		echo "BSIMODE=$BSIMODE" >>$ROOTCADIR/rootca.conf
		
		# must generate a randon as initial serial
		touch $ROOTCADIR/root-serial
		# count for incremental sn at msb (left aligned)
		echo "100001" >$ROOTCADIR/ra-count
		openssl rand -hex $SERIALSTART >$ROOTCADIR/root-serial 
		echo -n >$ROOTCADIR/root-index.txt 
		echo -n >$ROOTCADIR/root-index.txt.attr
		#ln -s $ROOTCADIR/root-index.txt $ROOTCADIR/root-index.txt.attr
	fi
	if [ ! -e $ROOTCADIR/root.key ]; then 
	    # more secure is
		# genrsa -rand [RAND.BIN] -aes256 -out ....... 
		echo "Do you want to secure the rootkey with a passphrase ?"
		echo " [y]es or [N]o  ?"
		if (yes_no); then 
			echo "...Enter the rootCA password"
			echo "!!!! NEVER FORGET THIS PASSWORD !!!!"
			echo "ENTER the password now :"
			read ROOTCAPASSWORD
			echo "Creating RootKeys ....."
			openssl genrsa -aes128 -passout pass:$ROOTCAPASSWORD -out  $ROOTCADIR/root.key $ROOTBITS 
		else
			openssl genrsa -out $ROOTCADIR/root.key $ROOTBITS >/dev/null 2>&1
		fi
		
		 
	fi 
	if [ ! -e $ROOTCADIR/.rnd ]; then 
	# RANDFILE		= $ENV::HOME/.rnd
	    touch  $ROOTCADIR/.rnd
	fi
	
	
	

}
function create_rootCA_Req_IA() {
reInitVars
rm -f $ROOTCADIR/This_$HOSTNAME.csr
openssl req \
-key $ROOTCADIR/root.key \
-new \
-out $ROOTCADIR/This_$HOSTNAME.csr 

	
}

function create_rootCA_Req() {
reInitVars
rm -f $ROOTCADIR/root_$HOSTNAME.req  
		
		# so gehts manuell: rootca openssl req -new -key osv500.key -out MYReq.pem 
		echo "... post creating request from rootCA"
		### old version
		#
		#openssl x509 \
		#	-in $ROOTCADIR/root.crt \
		#	-set_serial 0x$(openssl rand -hex 19) \
		#	-signkey $ROOTCADIR/root.key \
		#	-x509toreq -out $ROOTCADIR/root_$HOSTNAME.req
			
		### new version
		subrootca | openssl req \
			-batch  \
			-new \
			-config /dev/stdin \
			-in $ROOTCADIR/root.crt \
			-set_serial 0x$(openssl rand -hex 19) \
			-key $ROOTCADIR/root.key \
			-subj '/'"$BASEDN"'/CN='"$ROOTCN"'/emailAddress='"$BASEMAIL" \
			-out $ROOTCADIR/root_$HOSTNAME.req
		echo "created root_$HOSTNAME.req ..."
		openssl req -text -in $ROOTCADIR/root_$HOSTNAME.req | less
		
}



function create_rootca(){ 
	reInitVars
	. $CONFIGDIR/projectname.inf
	create_rootCA_DB
	local rootfileext='CA.pem'
	local RootFileName=$PKITprojectName$rootfileext
	
	
	if [ -e $ROOTCADIR/root.key -a ! -e $ROOTCADIR/root.crt ]; then 
		echo -en "Creating now rootCA\n" 
		# read 
		### new sign rootca request
		#rootca $ca_extension "" | openssl  ca -selfsign\
		#		-days $ROOTDAYS \
		#		-key $ROOTCADIR/root.key \
		#		-out $ROOTCADIR/root.crt \
		#		-in $ROOTCADIR/root_$HOSTNAME.req \
		#		-extensions v3_ca \
		#		-config /dev/stdin
		#		-subj '/'"$BASEDN"'/CN='"$ROOTCN" >/dev/null 2>&1
		### new end
		
		###
		### works without challengePassword
		
		
		rootca $ca_extension "" | openssl req \
			-batch \
			-x509 \
			-set_serial 0x$(openssl rand -hex 19) \
			-days $ROOTDAYS \
			-new \
			-key $ROOTCADIR/root.key \
			-out $ROOTCADIR/root.crt \
			-extensions v3_ca \
			-config /dev/stdin \
			-subj '/'"$BASEDN"'/CN='"$ROOTCN"'/emailAddress='"$BASEMAIL" >/dev/null 2>&1 
		#ln -s root.crt $ROOTCADIR/$(openssl x509 -noout -hash -in #$ROOTCADIR/root.crt).0 
		########
		# optional: create_rootCA_Req
		###
		touch $ROOTCADIR/isSuBCA.cnf
		echo "0" >$ROOTCADIR/isSuBCA.cnf
	fi
	
	
	
	if [ -e $ROOTCADIR/root.crt -a ! -e $ROOTCADIR/root.cer ]; then 
		openssl x509 -in $ROOTCADIR/root.crt -outform DER -out $ROOTCADIR/root.cer
		cp -p  $ROOTCADIR/root.crt $ROOTCADIR/rootCA.pem
		mv  $ROOTCADIR/root.cer $ROOTCADIR/rootCA.der
		
		if [ -d $WWWDIR ]; then 
		cp 	$ROOTCADIR/root.crt $WWWDIR/$RootFileName
		echo "## lastlog ##" >$WWWDIR/lastlog.log
		echo "## RootFileName=$RootFileName ##" >>$LASTLOG
		chmod 644 $WWWDIR/$RootFileName
		fi
		 
	fi
	
	cat > $ROOTCADIR/WebBrowserREADME.txt <<-EOL
	If you want to avoid certificate warnings in your webbrowser, import the root.crt file in your browser's certificate store. 
	EOL
	cat > $ROOTCADIR/PKCSREADME.txt <<-EOL
	*.cer is DER format for publishing 
	
	EOL
	echo "################# SECURITY WARNING ##############"
	echo "#                                               #"
	echo "#    PKIT comes with predefined DH Keys !       #"
	echo "#     YOU HAVE TO RECREATE DH KEYS              #"
	echo "#       PLEASE PERFORM THIS NOW                 #"
	echo "#   MENU: CA Management --> Create New DH       #"
	echo "#################################################"
	echo
	echo "Yes, I did understand: to Continue press [ENTER]"
	read 
}

function replace_rootcert() {
	reInitVars

	if [ -e $ROOTCADIR/root.crt ]; then
		echo "*********** be prepared ******************"
		echo "* you need the new root pem block        *"
		echo "*            AND                         *"
		echo "*   the pem block of your TOP Level CA   *"
		echo "##########################################"
		echo "# vi will now open the existing root.crt #"
		echo "##########################################"
		echo
		echo "replace the existing pem block with the new one, signed from enterprise PKI"
		echo "[ENTER] for opening the cert file ...";read
		cp -p $ROOTCADIR/root.crt $ROOTCADIR/root.crt.save
		vi $ROOTCADIR/root.crt
		echo "###########################################"
		echo "#  now we put the TOPLevel Root CA pem    #"
		echo "[ENTER] for opening the TOP ROOTCA file ..";read
		vi $ROOTCADIR/trustchain.crt
		echo "#   system changed to SUBCA... finish the action [Yy|Nn]"
			if (yes_no); then 
				
					cat $ROOTCADIR/root.crt >>$ROOTCADIR/trustchain.crt
				
					rm -f $ROOTCADIR/rootCA.pem
					rm -f $ROOTCADIR/rootCA.der
					SkipRootCACheck=1
					create_rootca
					echo    "########### Certificate Replaced #########"
					
					read -p "[ENTER] to cont for final RootCA Check...."
					
					SkipRootCACheck=0
					check_rootCA
					if [ $rootCACheck -eq 0 ]; then
						echo "###########################################"
						echo "#   Sucessfully RootCA Change to SBCA"
						echo "###########################################"
						echo "#   This RootCA is now a SUB-CA"
						echo "1" >$ROOTCADIR/isSuBCA.cnf
						echo "###########################################"
						read -p "[ENTER] to cont ...."
					else
						echo "###########################################"
						echo "#   This SUB RootCA has errors !!"
						echo "###########################################"
						cp -p $ROOTCADIR/root.crt.save $ROOTCADIR/root.crt
						echo "# restored to the last version "
						echo "0" >$ROOTCADIR/isSuBCA.cnf
						echo "###########################################"
						read -p "[ENTER] to cont ...."
					
					fi	
				
					
				
				else
				echo "###########################################"
				cp -p $ROOTCADIR/root.crt.save $ROOTCADIR/root.crt
						echo "# restored to the last version "
						echo "0" >$ROOTCADIR/isSuBCA.cnf
				rm -f $ROOTCADIR/trustchain.crt		
				echo "#  Skipped all actions, no change"
				echo "###########################################"
				read -p "[ENTER] to cont ...."
			fi
		
		else
		echo "################################################"
		echo "# There is not root.crt, please create first ..."
		echo "[ENTER] to cont ....";read
	fi

}
function create_root_p12() {
reInitVars
# openssl pkcs12 -export -out rootCA.p12 -inkey root.key -in rootCA.pem
	if [ -e $ROOTCADIR/root.crt ]; then

	openssl pkcs12 -export -out $ROOTCADIR/rootCA.p12 -inkey $ROOTCADIR/root.key -in 	$ROOTCADIR/rootCA.pem
	echo "###### RootCA is now exported to file $ROOTCADIR/rootCA.p12"
	else
	echo -e $red"!!!!! There is no Root CA found. Create first, please !!!"$transparent
	echo "OK ? ENTER to continue ..."
	read
	fi
}

function check_rootCA() {
reInitVars
#
if [[ $SkipRootCACheck -eq 0 ]]; then
	rootCACheck=0
	echo -en "\n--> This will perform a rootCA check ...\n"
	
	if [ $(cat $ROOTCADIR/isSuBCA.cnf) == "1" ];then
		echo -e $red "This is a Intermediate CA !"$transparent
		# check for $ROOTCADIR/trustchain.crt
			if [ -e $ROOTCADIR/trustchain.crt ]; then
			echo -en "--> $ROOTCADIR/trustchain.crt check OK...\n"; else
			echo -en $red"--> $ROOTCADIR/trustchain.crt check NOK...\n"$transparent; ((rootCACheck=++));
			fi
		else
		echo -e $red "This is a Top Level CA !"$transparent
	fi	
	
	if [ ! -e $ROOTCADIR/ra-count ]; then
	echo -en $red"--> $ROOTCADIR/ra-count check NOK...\n"; ((rootCACheck++));
	echo -en "## reason could be imported backup from old versions ##\n"$transparent
	fi
	
	if [ -e $ROOTCADIR/root.key ]; then
	echo -en "--> $ROOTCADIR/root.key check OK...\n";  else 
	echo -en "--> $ROOTCADIR/root.key check NOK...\n"; ((rootCACheck++));
	fi
	if [ -e $ROOTCADIR/root.crt ]; then
	hash_rootcrt=$(sha1sum $ROOTCADIR/root.crt | awk '{print $1}')
	echo -en "--> $ROOTCADIR/root.crt check OK...\n";  else 
	echo -en "--> $ROOTCADIR/root.crt check NOK...\n"; ((rootCACheck++)); 
	fi
	if [ -e $ROOTCADIR/rootCA.pem ]; then
	hash_rootpem=$(sha1sum $ROOTCADIR/rootCA.pem | awk '{print $1}')
	echo -en "--> $ROOTCADIR/rootCA.pem check OK...\n";
	RootTestCmd="openssl x509 -fingerprint -noout -in "$ROOTCADIR/root.crt > /dev/null 2>&1
	if ( eval $($RootTestCmd | grep error)  ); then
		openssl x509 -text -noout -in $ROOTCADIR/root.crt | grep 'Issuer:\|Subject:' | sed 's/^[ \t]*/==> /'
		echo -en "--> $ROOTCADIR/root.crt fingerprint OK...\n";
		else
		echo -en "--> $ROOTCADIR/root.crt fingerprint NOK...\n"; ((rootCACheck++)); 
	fi
	#openssl x509 -text -noout -in $ROOTCADIR/rootCA.pem | grep 'Issuer:\|Subject:' | sed 's/^[ \t]*/==> /' # replace spaces and tabs with ==> 
	if [ $hash_rootcrt = $hash_rootpem ]; then 
	echo "--> Certificate Files are equal, that's good ..."
	else
	echo "--> Certificate Files are NOT equal, that's bad ..."; ((rootCACheck++))
	fi
	else 
	echo -en "--Y $ROOTCADIR/rootCA.pem check NOK...\n"; ((rootCACheck++));

	fi
	if [ -e $ROOTCADIR/rootCA.der ]; then
	echo -en "--> $ROOTCADIR/rootCA.der check OK...\n";  

	else 
	echo -en "--> $ROOTCADIR/rootCA.der check NOK...\n"; ((rootCACheck++));
	fi

	if [ -e $ROOTCADIR/root-serial ]; then
	echo -en "--> $ROOTCADIR/root-serial >>$(cat $ROOTCADIR/root-serial)<< check OK...\n";

	else 
	echo -en "--> $ROOTCADIR/root-serial does not exist NOK...\n"; ((rootCACheck++));
	fi

	if [ $rootCACheck -eq 0 ]; then
	echo -en "\nCHECK WAS SUCCESSFUL --- GOOD\n"
	else
	echo -en $red"\nCHECK FAILED !!!! Repair the RootCA first !!!\n"$transparent

	fi
	else
	echo
	echo "########### RootCA Check skipped this time ...\n"
	echo
fi
}

function ca() { 
reInitVars
	base
	cat <<-EOF
	[ req ] 
		default_md = $CERTSHA 
	[ ca ]
		default_ca = CA_default
		
	[ subca ]	
		private_key = $ROOTCADIR/root.key
		certificate = $ROOTCADIR/root.crt 
		new_certs_dir = $PKIHOME/subca
		database = $DATABASE 
		default_md = $CERTSHA 
		policy = policy_anything 
		serial = $ROOTCADIR/root-serial 
		certs = $PKIHOME/subca 
		RANDFILE = $ROOTCADIR/root-random 
		#crl_dir = $CRLDIR 
		crl = $CRLDIR/rootca.crl
	[ CA_default ] 
		private_key = $ROOTCADIR/root.key
		certificate = $ROOTCADIR/root.crt 
		new_certs_dir = $CERTSCACHEDIR
		database = $DATABASE 
		default_md = $CERTSHA 
		policy = policy_anything 
		serial = $ROOTCADIR/root-serial 
		certs = $CERTSCACHEDIR 
		RANDFILE = $ROOTCADIR/root-random 
		#crl_dir = $CRLDIR 
		crl = $CRLDIR/rootca.crl 
	[ ca4ext ]
		private_key = $ROOTCADIR/root.key
		certificate = $ROOTCADIR/root.crt 
		new_certs_dir = $CERTSCACHEDIR
		database = $DATABASE 
		default_md = $CERTSHA 
		policy = policy_4ext 
		serial = $ROOTCADIR/root-serial 
		certs = $CERTSCACHEDIR 
		RANDFILE = $ROOTCADIR/root-random 
		#crl_dir = $CRLDIR 
		crl = $CRLDIR/rootca.crl
		copy_extensions = copy
		
	[policy_4ext]
		
	[ exts4exts ]	
		basicConstraints=CA:FALSE
		#extendedKeyUsage=serverAuth, clientAuth
		keyUsage = digitalSignature, keyEncipherment
		#copy_extensions = copy
	[policy_anything ] 
	EOF
		echo "[ exts ]"
		if [ -n "$2" ]; then 
			echo "subjectAltName = $2" 
			X509_subaltname=$2 
		fi 
		echo "subjectKeyIdentifier=hash" 
		if [ "$3" = sign ]; then
			echo "authorityKeyIdentifier=keyid:always" 
		fi 
		# todo webcollaboration server 
		case $1 in hg35|xpr|osb|ossbc|ucbackend|ucfrontend|ucfacade|openfire|mediatrix|osv)
			cat <<-EOF
			basicConstraints=CA:FALSE 
			extendedKeyUsage=serverAuth, clientAuth 
			keyUsage = digitalSignature, keyEncipherment 
			EOF
			;;
		osvserver|osms|dls) 
			cat <<-EOF
			basicConstraints=CA:FALSE
			extendedKeyUsage=serverAuth, clientAuth 
			keyUsage = digitalSignature, keyEncipherment
			EOF
			;; 
		sbcgui) 
			cat <<-EOF
			basicConstraints=CA:FALSE
			extendedKeyUsage=serverAuth 
			keyUsage = digitalSignature, keyEncipherment
			EOF
			;; 
		osbgui)
			cat <<-EOF
			basicConstraints=CA:FALSE
			extendedKeyUsage=serverAuth 
			keyUsage = digitalSignature, keyEncipherment
			EOF
			;; 
		cmp) 
			cat <<-EOF
			basicConstraints=CA:FALSE
			extendedKeyUsage=serverAuth 
			keyUsage = digitalSignature, keyEncipherment
			EOF
			;; 
		osvclient) 
			cat <<-EOF
			basicConstraints=CA:FALSE
			extendedKeyUsage=clientAuth 
			keyUsage = digitalSignature 
			EOF
			;;
		openstage)
			cat <<-EOF
			basicConstraints=CA:FALSE
			extendedKeyUsage=serverAuth, clientAuth 
			keyUsage = digitalSignature 
			EOF
			;; 
		ASCEvoIPactive)
			cat <<-EOF
			basicConstraints = CA:FALSE
			extendedKeyUsage=serverAuth, clientAuth 
			keyUsage = digitalSignature, nonRepudiation, keyEncipherment 
			EOF
			;; 
		acmesbc)
			cat <<-EOF
			basicConstraints = CA:FALSE
			keyUsage = digitalSignature, nonRepudiation, keyEncipherment 
			EOF
			;;
		apache) 
			cat <<-EOF
			basicConstraints = CA:FALSE
			extendedKeyUsage=serverAuth, clientAuth 
			keyUsage = digitalSignature, nonRepudiation, keyEncipherment 
			EOF
			;; 
		email) 
			cat <<-EOF
			basicConstraints = CA:FALSE
			extendedKeyUsage=emailProtection 
			keyUsage = critical, digitalSignature, keyEncipherment
			nsCertType=email
			EOF
			;; 	
		subca) 
			cat <<-EOF
			basicConstraints = CA:true 
			keyUsage = cRLSign, keyCertSign 
			EOF
			;; 
		*) 
			#echo -en "WARNING: Type $1 is will be created with default X509 attributes !\n" 
				cat <<-EOF
			basicConstraints=CA:FALSE 
			extendedKeyUsage=serverAuth
			 
			keyUsage = digitalSignature, keyEncipherment 
			EOF
			;; 
		esac 
		
		if [ -n "$ROOTCRLURI" ]; then 
			echo "crlDistributionPoints = URI:$ROOTCRLURI" 
		
		fi
		# new check ...
		
		
		if [ -n "$ROOTOCSPURI" -a -n "$ROOTCERTURI" ]; then
			echo "authorityInfoAccess = OCSP;URI:$ROOTOCSPURI, caIssuers;URI:$ROOTCERTURI" 
			elif [ -n "$ROOTOCSPURI" ]; then 
			echo "authorityInfoAccess = OCSP;URI:$ROOTOCSPURI" 
			elif [ -n "$ROOTCERTURI" ]; then 
			echo "authorityInfoAccess = caIssuers;URI:$ROOTCERTURI" 
		fi 
}
