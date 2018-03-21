function base() { 
	cat <<-EOF
	[ req ] 
	distinguished_name = req_distinguished_name 
	[ req_distinguished_name ] 
	EOF
} 
function rootca() { 
	base 
	cat <<-EOF
	# --- new 
	attributes = req_attributes
	[ req ] 
	default_md = $ROOTSHA
	[ req_attributes ]
	challengePassword = $ChallPassWD	
	# ---- new end 
	[ v3_ca ]
	subjectKeyIdentifier=hash
	#authorityKeyIdentifier=keyid:always,issuer:always
	authorityKeyIdentifier=keyid:always 
	basicConstraints = CA:true 
	keyUsage = cRLSign, keyCertSign 
	EOF
} 
# --------new # not implemented full

# --------new 
function crl() {  
	base 
	cat <<-EOF
	[ ca ] 
	default_ca = CA_default 
	[ CA_default ] 
	private_key = $ROOTCADIR/root.key 
	certificate = $ROOTCADIR/root.crt 
	new_certs_dir = $PKIHOME/certs 
	database = $ROOTCADIR/root-index.txt 
	default_md = $SHA
	policy = policy_anything 
	serial = $ROOTCADIR/root-serial 
	certs = $PKIHOME/certs 
	RANDFILE = $ROOTCADIR/root-random 
	#crl_dir = $CRLDIR 
	crl = $CRLDIR/rootca.crl 
	default_crl_days = $CRLDAYS 
	[ policy_anything ] 
	EOF

} 
function create_dh(){
	# Diffie-Hellman-Parameter erstellen 
	for length in $ROOTBITS $CERTBITS; do 
		if [ ! -e $DHDIR/dh$length.pem ]; then
			openssl dhparam -out $DHDIR/dh$length.pem $length 
		fi
	done 
} 

function create_rootca(){ 
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
	if [ -e $ROOTCADIR/root.key -a ! -e $ROOTCADIR/root.crt ]; then 
		# echo -en "\n\t ca extension = $ca_extension\n" 
		# read 
		rootca $ca_extension "" | openssl req \
			-batch \
			-x509 \
			-set_serial 0 \
			-days $ROOTDAYS \
			-new \
			-key $ROOTCADIR/root.key \
			-out $ROOTCADIR/root.crt \
			-extensions v3_ca \
			-attributes req_attributes 
			-config /dev/stdin \
			-subj '/'"$BASEDN"'/CN='"$ROOTCN" >/dev/null 2>&1 
		ln -s root.crt $ROOTCADIR/$(openssl x509 -noout -hash -in $ROOTCADIR/root.crt).0 
	fi
	if [ ! -e $ROOTCADIR/root_$HOSTNAME.req ]; then 
		echo "gen root request for external ca .." 
		# read 
		# rootca request 
		rootca $ca_extension "" | openssl req \
			-batch \
			-new \
			-key $ROOTCADIR/root.key \
			-out $ROOTCADIR/root_$HOSTNAME.req \
			-extensions v3_ca \
			-config /dev/stdin -subj '/'"$BASEDN" >/dev/null 2>&1 
		# so gehts manuell: rootca openssl req -new -key osv500.key -out MYReq.pem 
		else 
		echo -en "\t---> root req exists, nothing todo\n" 
		#openssl req -new -key privkey.pem -out cert.csr 
	fi
	if [ -e $ROOTCADIR/root.crt -a ! -e $ROOTCADIR/root.cer ]; then 
		openssl x509 -in $ROOTCADIR/root.crt -outform DER -out $ROOTCADIR/root.cer
		cp -p  $ROOTCADIR/root.crt $ROOTCADIR/rootCA.pem		
		mv  $ROOTCADIR/root.cer $ROOTCADIR/rootCA.der 
	fi
	
	cat > $ROOTCADIR/WebBrowserREADME.txt <<-EOL
	If you want to avoid certificate warnings in your webbrowser, import the root.crt file in your browser's certificate store. 
	EOL
	cat > $ROOTCADIR/PKCSREADME.txt <<-EOL
	*.cer is DER format for publishing 
	EOL

}

function create_root_p12() {
# openssl pkcs12 -export -out rootCA.p12 -inkey root.key -in rootCA.pem
	if [ -e $ROOTCADIR/root.crt ]; then

	openssl pkcs12 -export -out $ROOTCADIR/rootCA.p12 -inkey $ROOTCADIR/root.key -in 	$ROOTCADIR/rootCA.pem
	echo "###### RootCA is now exported to file $ROOTCADIR/rootCA.p12"
	else
	echo "!!!!! There is no Root CA found. Create first, please !!!"
	fi
}

function check_rootCA() {
#
echo -en ".. This will perform a rootCA check ...\n"
if [ -e $ROOTCADIR/root.key ]; then
echo -en ".. $ROOTCADIR/root.key check OK...\n"; rootCACheck=0; else 
echo -en ".. $ROOTCADIR/root.key check NOK...\n"; rootCACheck=1;
fi
if [ -e $ROOTCADIR/root.crt ]; then
echo -en ".. $ROOTCADIR/root.crt check OK...\n"; rootCACheck=0; else 
echo -en ".. $ROOTCADIR/root.crt check NOK...\n"; rootCACheck=1; 
fi
if [ -e $ROOTCADIR/rootCA.pem ]; then
echo -en ".. $ROOTCADIR/rootCA.pem check OK...\n"; rootCACheck=0; else 
echo -en ".. $ROOTCADIR/rootCA.pem check NOK...\n"; rootCACheck=1;
fi
if [ -e $ROOTCADIR/rootCA.der ]; then
echo -en ".. $ROOTCADIR/rootCA.der check OK...\n"; rootCACheck=0; else 
echo -en ".. $ROOTCADIR/rootCA.der check NOK...\n"; rootCACheck=1;
fi
echo "Enter to continue ..."
read


}

function ca() { 
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
		new_certs_dir = $PKIHOME/certs
		database = $DATABASE 
		default_md = $SHA 
		policy = policy_anything 
		serial = $ROOTCADIR/root-serial 
		certs = $PKIHOME/certs 
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
