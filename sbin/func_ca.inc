# production 1
function base() { 
	cat <<-EOF
	[ req ] 
	distinguished_name = req_distinguished_name 
	[ req_distinguished_name ] 
	EOF
} 
function rootca() { 
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
	#authorityKeyIdentifier=keyid:always,issuer:always
	authorityKeyIdentifier=keyid:always 
	basicConstraints=CA:TRUE,pathlen:0
	keyUsage = critical, cRLSign, keyCertSign

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
	# --- new 
	
	[ req_subca ]
	oid_section		= new_oids
	default_md = $ROOTSHA
	basicConstraints=CA:TRUE,pathlen:0
	keyUsage = nonRepudiation, digitalSignature, keyEncipherment
	distinguished_name = req_distinguished_name
	attributes = req_attributes	
	
	[ new_oids ]

	# Policies used by the TSA examples.
	tsa_policy1 = 1.2.3.4.1
	tsa_policy2 = 1.2.3.4.5.6
	tsa_policy3 = 1.2.3.4.5.7
	
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
	
	
	[ v3_req ]
	#policy = policy_PKIT
	#randfile=$ROOTCADIR/root-random
	subjectKeyIdentifier=hash
	#authorityKeyIdentifier=keyid:always,issuer:always
	authorityKeyIdentifier=keyid:always,issuer:always 
	basicConstraints=CA:TRUE,pathlen:0
	keyUsage = nonRepudiation, digitalSignature, keyEncipherment

	
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
	default_md = $SHA
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
function create_rootCA_DB() {
reInitVars
mkdir -m 700 -p $ROOTCADIR 
	export CN 
	if [ ! -e $ROOTCADIR/root-serial -o ! -e $ROOTCADIR/root-index.txt -o ! -e $ROOTCADIR/root.key ]; then 
		rm -f $ROOTCADIR/*.* 
		rm -f $CERTSDIR/*.*
		echo "01" >$ROOTCADIR/root-serial 
		echo -n >$ROOTCADIR/root-index.txt 
		#ln -s $ROOTCADIR/root-index.txt $ROOTCADIR/root-index.txt.attr
	fi
	if [ ! -e $ROOTCADIR/root.key ]; then 
		openssl genrsa -out $ROOTCADIR/root.key $ROOTBITS >/dev/null 2>&1 
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
if [ ! -e $ROOTCADIR/root_$HOSTNAME.req ]; then 
		
		### old version
		#echo "gen special root csr for ca .." 
		# read 
		# rootca request 
		rm -f root_$HOSTNAME.csr
		#rootca $ca_extension "" | openssl req \
		#	-batch \
		#	-new \
		#	-key $ROOTCADIR/root.key \
		#	-out $ROOTCADIR/root_$HOSTNAME.csr \
		#	-extensions v3_ca \
		#	-config /dev/stdin \
		#	-subj '/'"$BASEDN" >/dev/null 2>&1 
		# so gehts manuell: rootca openssl req -new -key osv500.key -out MYReq.pem 
		
		### new version
		echo "... post creating request from rootCA"
		openssl x509 \
			-in $ROOTCADIR/root.crt \
			-set_serial 0x$(openssl rand -hex 19) \
			-signkey $ROOTCADIR/root.key \
			-x509toreq -out $ROOTCADIR/root_$HOSTNAME.req
			
		else 
		echo -en "\t---> root req exists, nothing todo\n" 
		#openssl req -new -key privkey.pem -out cert.csr 
	fi

}

function create_SUB_rootCA_Req(){
	# BLOCK [ req_subca ] NUTZEN
	reInitVars
	. $CONFIGDIR/projectname.inf
	local SUBRootReqFIle=sub_$PKITprojectName.req
	subrootca $ca_extension "" | openssl req \
			-batch \
			-x509 \
			-set_serial 0x$(openssl rand -hex 19) \
			-days $ROOTDAYS \
			-new \
			-key $ROOTCADIR/root.key \
			-out $ROOTCADIR/root.crt \
			-extensions req_subca \
			-config /dev/stdin \
			-subj '/'"$BASEDN"'/CN='"$ROOTCN"'/emailAddress='"BASEMAIL" >/dev/null 2>&1 
		
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
			-subj '/'"$BASEDN"'/CN='"$ROOTCN"'/emailAddress='"BASEMAIL" >/dev/null 2>&1 
		#ln -s root.crt $ROOTCADIR/$(openssl x509 -noout -hash -in #$ROOTCADIR/root.crt).0 
		###
		###
	fi
	
	
	
	if [ -e $ROOTCADIR/root.crt -a ! -e $ROOTCADIR/root.cer ]; then 
		openssl x509 -in $ROOTCADIR/root.crt -outform DER -out $ROOTCADIR/root.cer
		cp -p  $ROOTCADIR/root.crt $ROOTCADIR/rootCA.pem
		mv  $ROOTCADIR/root.cer $ROOTCADIR/rootCA.der
		cp 	$ROOTCADIR/root.crt $WWWDIR/$RootFileName
		echo "## lastlog ##" >$WWWDIR/lastlog.log
		echo "## RootFileName=$RootFileName ##" >>$WWWDIR/lastlog.log
		chmod 644 $WWWDIR/$RootFileName
		 
	fi
	
	cat > $ROOTCADIR/WebBrowserREADME.txt <<-EOL
	If you want to avoid certificate warnings in your webbrowser, import the root.crt file in your browser's certificate store. 
	EOL
	cat > $ROOTCADIR/PKCSREADME.txt <<-EOL
	*.cer is DER format for publishing 
	EOL

}

function replace_rootcert() {
	reInitVars

	if [ -e $ROOTCADIR/root.crt ]; then
		echo "###########################################"
		echo "#   vi will now open the existing root.crt"
		echo "###########################################"
		echo
		echo "replace the existing pem block with the new one, signed from enterprise PKI"
		echo "[ENTER] for opening the cert file ...";read
		cp -p $ROOTCADIR/root.crt $ROOTCADIR/root.crt.save
		vi $ROOTCADIR/root.crt
		echo "###########################################"
		echo "#   root certificate changed ... finish the action [Yy|Nn]"
			if (yes_no); then 
				check_rootCA
				echo "rootCACheck = $rootCACheck"
				if [ $rootCACheck -eq 0 ]; then
						rm -f $ROOTCADIR/rootCA.pem
						rm -f $ROOTCADIR/rootCA.der
						create_rootca
						echo "###########################################"
						echo "#   Sucessfully Changed CA to SUBCA"
						echo "###########################################"
						echo "#   This RootCA is noW a SUB-CA"
						echo "###########################################";
						read -p "[ENTER] to cont ...."
					else
						echo "###########################################"
						echo "#   This SUB RootCA has errors !!"
						echo "###########################################"
						cp -p $ROOTCADIR/root.crt.save $ROOTCADIR/root.crt
						echo "# restored to the last version "
						echo "###########################################"
						read -p "[ENTER] to cont ...."
					
				fi	
					
				
				else
				echo "###########################################"
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
	echo "!!!!! There is no Root CA found. Create first, please !!!"
	fi
}

function check_rootCA() {
reInitVars
#
rootCACheck=0
echo -en "\n--> This will perform a rootCA check ...\n"
if [ -e $ROOTCADIR/root.key ]; then
echo -en "--> $ROOTCADIR/root.key check OK...\n"; rootCACheck=0; else 
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
echo -en "\nCHECK WAS SUCCESSFUL --- GOOD"
else
echo -en "\nCHECK FAILED !!!! Repair the RootCA first !!!"
fi
echo -en "\nEnter to continue ..."
read


}

function ca() { 
reInitVars
	base
	cat <<-EOF
	[ req ] 
		default_md = $SHA 
	[ ca ]
		default_ca = CA_default
	[ subca ]	
		private_key = $ROOTCADIR/root.key
		certificate = $ROOTCADIR/root.crt 
		new_certs_dir = $PKIHOME/subca
		database = $DATABASE 
		default_md = $SHA 
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
		default_md = $SHA 
		policy = policy_anything 
		serial = $ROOTCADIR/root-serial 
		certs = $CERTSCACHEDIR 
		RANDFILE = $ROOTCADIR/root-random 
		#crl_dir = $CRLDIR 
		crl = $CRLDIR/rootca.crl 
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
			extendedKeyUsage=serverAuth 
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
		if [ -n "$ROOTOCSPURI" -a -n "$ROOTCERTURI" ]; then
			echo "authorityInfoAccess = OCSP;URI:$ROOTOCSPURI, caIssuers;URI:$ROOTCERTURI" 
			elif [ -n "$ROOTOCSPURI" ]; then 
			echo "authorityInfoAccess = OCSP;URI:$ROOTOCSPURI" 
			elif [ -n "$ROOTCERTURI" ]; then 
			echo "authorityInfoAccess = caIssuers;URI:$ROOTCERTURI" 
		fi 
}
