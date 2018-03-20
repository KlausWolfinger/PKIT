function subCA() { 
	base 
	cat <<-EOF
	[ v3_ca ] 
	subjectKeyIdentifier=hash
	#authorityKeyIdentifier=keyid:always,issuer:always
	authorityKeyIdentifier=keyid:always 
	basicConstraints = CA:true 
	keyUsage = cRLSign, keyCertSign 
	EOF
}
function ask_subCAName(){
echo -en "Enter a unique name for the subca:";read SUBCANAME
}

function reqsubca(){ 

	SUBCADIRNAME=$SUBCADIR/$SUBCANAME
	if [ ! -d $SUBCADIRNAME ];then
		mkdir -p $SUBCADIRNAME
	fi
	if [ -e $SubCATemplate -a ! -e $SUBCADIRNAME/subca.inf ];then
		# echo "now copying $SubCATemplate template to $SUBCADIRNAME/subca.inf";read
		# dir $SubCATemplate
		
		cmd="cp  -p $SubCATemplate $SUBCADIRNAME/subca.inf"
		$($cmd)
		# dir $SUBCADIRNAME;read
		#return;
			
	fi
	if [ -e $SUBCADIRNAME/subca.inf ]; then 
		echo -en "INFO: READ X509 Attributes from $SUBCADIRNAME/subca.inf \n" 
		. $SUBCADIRNAME/subca.inf 
		#read 
		else
		SUBCABASEDN="C=DE/L=Germany/O=SUBCA Authority/OU=PKI Management"
		SUBCADN="C=DE/L=Germany/O=Production/OU=Server Management"
		SUBBITS=2048
		SUBSHA="sha256"
		SUBDAYS=356
		SUBCN="PKITOOL SUB CA"
		SUBCERTBITS=2048
		SUBCERTDAYS=1500
		ca_extension=""
		SUBSHA="sha1"
	fi
	CN=$SUBCANAME
	
	
	
	export CN 
	if [ ! -e $SUBCADIRNAME/subca-serial -o ! -e $SUBCADIRNAME/subca-index.txt -o ! -e $SUBCADIRNAME/subca.key ]; then 
				 
		echo "00:00:00:01" >$SUBCADIRNAME/subca-serial 
		echo -n >$SUBCADIRNAME/subca-index.txt 
	fi
	if [ ! -e $SUBCADIRNAME/subca.key ]; then 
		openssl genrsa -out $SUBCADIRNAME/subca.key $ROOTBITS >/dev/null 2>&1 
	fi 
	
	if [ ! -e $SUBCADIRNAME/subca.req ]; then 
		# echo "gen root request for sub ca .." 
		# read 
		# subCA request 
		subCA $ca_extension "" | openssl req \
			-batch \
			-new \
			-key $SUBCADIRNAME/subca.key \
			-out $SUBCADIRNAME/subca.req \
			-extensions v3_ca \
			-config /dev/stdin -subj '/'"$SUBCABASEDN" >/dev/null 2>&1 
		
		else 
		echo -en "\t---> root req exists, nothing todo\n" 
	 
	fi
	
	cat > $SUBCADIRNAME/WebBrowserREADME.txt <<-EOL
	If you want to avoid certificate warnings in your webbrowser, import the subca.crt file in your browser's certificate store. 
	EOL
	cat > $SUBCADIRNAME/PKCSREADME.txt <<-EOL
	*.cer is DER format for publishing 
	EOL

}

function signsubca(){
	SALTN=""
	SUBCADIRNAME=$SUBCADIR/$SUBCANAME
	

	# test for key and request
	if [ ! -e $SUBCADIRNAME/subca.key -a ! -e $SUBCADIRNAME/subca.req ];then
		echo "Missing subca request and key ..."
		exit 1;
	fi
	if [ -e $SUBCADIRNAME/subca.crt ];then
		echo -en "SubCA certificate already exists\n"
		dir $SUBCADIRNAME;
		exit 1;
	fi
	# subca is keyword in ca routine
	ca subca "$SALTN" sign \
		| openssl ca \
		-batch \
		-days $CERTDAYS \
		-out $SUBCADIRNAME/subca.crt \
		-in $SUBCADIRNAME/subca.req \
		-extensions exts \
		-preserveDN \
		-config /dev/stdin 
	 
	if [ -e $SUBCADIRNAME/subca.crt -a ! -e $SUBCADIRNAME/subca.cer ]; then 
		openssl x509 -in $SUBCADIRNAME/subca.crt -outform DER -out $SUBCADIRNAME/subca.cer 
		cp -p $SUBCADIRNAME/subca.crt $SUBCADIRNAME/subCA.pem 
		# cp -p $SUBCADIRNAME/subca_$SUBCANAME.cer $SUBCADIRNAME/subCA_$SUBCANAME.der 
	fi

}


function delsubca(){
	echo ".. deleting subCA, are you sure [y/n]"
	if (yes_no); then
	SUBCADIRNAME=$SUBCADIR/$SUBCANAME
		if [ ! -e $SUBCADIRNAME/subca.crt ];then
		echo -en "Sorry certificatefiles do not exist ...";
		else
		
		SubCARevoke;
		#read;
		rm -R -f $SUBCADIRNAME 
		fi
	else 
	echo "--- nothing deleted ---"
	fi
}
