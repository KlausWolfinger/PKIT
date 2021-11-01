function subCA() {
	## diese funktion ist nicht in benutzung ##
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
	if [ -e $SubCATemplate -a ! -e $SUBCADIRNAME/$SUBCANAME.inf ];then
		# echo "now copying $SubCATemplate template to $SUBCADIRNAME/subca.inf";read
		# dir $SubCATemplate
		
		cmd="cp  -p $SubCATemplate $SUBCADIRNAME/$SUBCANAME.inf"
		$($cmd)
		# dir $SUBCADIRNAME;read
		#return;
			
	fi
	if [ -e $SUBCADIRNAME/$SUBCANAME.inf ]; then 
		echo -en "INFO: READ X509 Attributes from $SUBCADIRNAME/$SUBCANAME.inf \n" 
		. $SUBCADIRNAME/$SUBCANAME.inf 
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
	if [ ! -e $SUBCADIRNAME/$SUBCANAME-serial -o ! -e $SUBCADIRNAME/$SUBCANAME-index.txt -o ! -e $SUBCADIRNAME/$SUBCANAME.key ]; then 
				 
		echo "00:00:00:01" >$SUBCADIRNAME/$SUBCANAME-serial 
		echo -n >$SUBCADIRNAME/$SUBCANAME-index.txt 
	fi
	if [ ! -e $SUBCADIRNAME/$SUBCANAME.key ]; then 
		openssl genrsa -out $SUBCADIRNAME/$SUBCANAME.key $ROOTBITS >/dev/null 2>&1 
	fi 
	
	if [ ! -e $SUBCADIRNAME/$SUBCANAME.req ]; then 
		# echo "gen root request for sub ca .." 
		# read 
		# subCA request 
		subCA $ca_extension "" | openssl req \
			-batch \
			-new \
			-key $SUBCADIRNAME/$SUBCANAME.key \
			-out $SUBCADIRNAME/$SUBCANAME.req \
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
	
	## Import cert first
	echo "You have to paste the request (pem) to the request file"
	echo "Will open vi $SUBCADIRNAME/$SUBCANAME.req .. [ENTER]"
	read 
	if [ ! -e $SUBCADIRNAME  ]; then
	mkdir $SUBCADIRNAME
	fi
	
	vi $SUBCADIRNAME/$SUBCANAME.req
	if [ ! -e $SUBCADIRNAME/$SUBCANAME.req ];then
		echo "Missing subca request ..."
		exit 1;
	fi
	
	if [ -e $SUBCADIRNAME/$SUBCANAME.crt ];then
		echo -en "SubCA certificate already exists\n"
		dir $SUBCADIRNAME;
		exit 1;
	fi
	# subca is keyword in ca routine
	
	ca subca "$SALTN" sign | openssl ca \
		-batch \
		-days $ROOTDAYS \
		-out $SUBCADIRNAME/$SUBCANAME.pem \
		-in $SUBCADIRNAME/$SUBCANAME.req \
		-extensions exts -preserveDN \
		-config /dev/stdin 
	 
	if [ -e $SUBCADIRNAME/$SUBCANAME.pem -a ! -e $SUBCADIRNAME/$SUBCANAME.cer ]; then 
		openssl x509 -in $SUBCADIRNAME/$SUBCANAME.pem -outform DER -out $SUBCADIRNAME/$SUBCANAME.cer 
		
		
	fi
	echo "printing pem to screen .. ENTER .."
	echo
	read
	less $SUBCADIRNAME/$SUBCANAME.pem

}


function delsubca(){
	echo ".. deleting subCA, are you sure [y/n]"
	if (yes_no); then
	SUBCADIRNAME=$SUBCADIR/$SUBCANAME
		if [ ! -e $SUBCADIRNAME/$SUBCANAME.crt ];then
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
