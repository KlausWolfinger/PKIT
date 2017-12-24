function SignRequest(){ 
	#echo -en "---------- debug----------\n";read
	export CN
	CN_count=0;
	echo -en "--- Signing external certificates ...\n" 
	cat $HOSTDIR/candreq \
		| grep -v "^[ ]*#" \
		| grep -v "^[ ]*$" \
		| grep -v "^[ ]*!" \
		| while  read cert; do  
			extension=$(echo "$cert" | cut -f1 -d:) 
				   CN=$(echo "$cert" | cut -f2- -d: | cut -f1 -d,)
				   SALTN=$(echo "$cert" | cut -f2- -d: | grep , | cut -f2- -d,)
		
		# echo  -en "--> HOST: $cert CN: $CN extension: $extension "
		
	
	if [ -e $CANDCERTSDIR/$extension/"$CN".req -a ! -e $CANDCERTSDIR/$extension/"$CN".crt ]; then 
		mv $CANDCERTSDIR/$extension/"$CN".req $CANDCERTSDIR/$extension/tmp-"$CN".req 
		rm -f $CANDCERTSDIR/$extension/"$CN".* 
		mv $CANDCERTSDIR/$extension/tmp-"$CN".req $CANDCERTSDIR/$extension/"$CN".req 
		ca $extension "$SALTN" sign | openssl ca \
				-batch \
				-days $CERTDAYS \
				-out $CANDCERTSDIR/$extension/"$CN".crt \
				-in $CANDCERTSDIR/$extension/"$CN".req \
				-extensions exts -preserveDN\
				-config /dev/stdin
 
				#	echo "stop";read
					
				openssl x509 \
				-in $CANDCERTSDIR/$extension/"$CN".crt \
				-out $CANDCERTSDIR/$extension/"$CN".pem; 
		echo -en "\t---> $CN crt and pem created\n" 
		echo -en "=============================================\n"
		cat $CANDCERTSDIR/$extension/"$CN".pem 
		echo -en "=============================================\n" 
	else 
		echo -en "\t---> $CN Error, no cert written\n"
			echo -en "\t---> REQ does not exist or CRT already exists\n"
	fi
	done 
	echo -en "\n\t--> Press ENTER to continue" 
	read 
} 

function importcreq() {
	clear 
	dos2unix $CandFile >/dev/null 2>&1	
	extCandsArray=($(cat $CandFile))
	tLen=${#extCandsArray[@]}
	#let tLen++
	echo -en "Found $tLen entries in external candidates\n"
	echo -en "------------------------------------------\n"
	echo -en " Number\tSystem:Alias----------------------\n"
	for (( i=0; $i<${tLen}; i++ )); do 
		echo -en " $i --> \t${extCandsArray[$i]}\n" 
	done
	echo -en "\nChoose number from list...\n"
	echo -en "------------------------------------------\n"
	echo -en "Next, the tool will open the vi editor,   \n"
	echo -en ".. paste the X509 request pem block into ...\n"
	echo -en "... now the number, please: "
	read CandNum
	# debugging 
	#echo -en "Candidate: ${extCandsArray[$CandNum]}\n";read
	cert=$(echo ${extCandsArray[$CandNum]} \
		| grep -v "^[ ]*#" \
		| grep -v "^[ ]*$" \
		| grep -v "^[ ]*!")
	# debugging  
	#echo -en "Certificate: $cert\n";read
	
	CN=$(echo "$cert" | cut -f2- -d: | cut -f1 -d,) 
	# debugging  
	#echo -en "CN: $CN\n";read
	SALTN=$(echo "$cert" | cut -f2- -d: | grep , | cut -f2- -d,) 
	extension=$(echo "$cert" | cut -f1 -d:)
	# debugging  
	#echo -en "Extension: $extension\n";read	
	Alias=$(echo "$CN")
	# debugging 
	#echo -en "Alias: $Alias\n";read
	Vendor=$(echo "$extension") 	
	# debugging   
	#echo -en "Vendor: $Vendor\n";read
	
	#echo -en " --> Vendor $Vendor with certname $Alias\n"
	
	#	echo -en "Choose Vendor from list: \n";read CandNum
	#	echo -en "-- debugging variable i after while read = $i\n" 
	#	echo -en "-- CANDCERTSDIR = $CANDCERTSDIR\n"
	#	echo -en "-- Candidate Number:  = $CandNum\n"
	#	echo -en "-- Variable i :  = $i\n"
	#	echo -en "-- Vendor = $Vendor\n"
	#	echo -en "Requestfile = $CANDCERTSDIR/$Vendor/$Alias.req\n"
		# Requestfile = ./candidates/CandCerts//.req
		# -- Reuestfilename = .req
	#	echo -en "-- Requestfilename = $Alias.req\n ENTER ...."
	#	read
		if [ ! -d $CANDCERTSDIR/$Vendor ]; then 
			mkdir -p $CANDCERTSDIR/$Vendor 
		fi
		if [ ! -d $CANDKEYSDIR/$Vendor ]; then 
			mkdir -p $CANDKEYSDIR/$Vendor 
		fi
		vi $CANDCERTSDIR/$Vendor/$Alias.req 
		echo -en "\n\t ---- candidate request file ----- \n" 
		cat $CANDCERTSDIR/$Vendor/$Alias.req 
		echo -en "\n\t ENTER to continue \n"; read 
} 

function create_requests() { 
	export CN
	CN_count=0; 
	# echo -en "--- creating certificate requests from hosts file (func_requests.inc)...\n" 
	cat $HOSTDIR/hosts \
	| grep -v "^[ ]*#" \
	| grep -v "^[ ]*$" \
	| grep -v "^[ ]*!" \
	| while read cert; do 
		echo -en "\n--> HOST: $cert " 
		CN=$(echo "$cert" | cut -f2- -d: | cut -f1 -d,) 
		SALTN=$(echo "$cert" | cut -f2- -d: | grep , | cut -f2- -d,) 
		# SALTN is DNS and IP (subjectAltAttributes) 
		# extension is "keyword like tlsserver, tlsboth etc 
		extension=$(echo "$cert" | cut -f1 -d:) 
		#echo "---> extension= $extension" 
		if [ ! -d $CERTSDIR/$extension ]; then 
			mkdir $CERTSDIR/$extension 
		fi 
		if [ ! -d $KEYSDIR/$extension ]; then 
			mkdir $KEYSDIR/$extension 
		fi 
		if [ ! -e $KEYSDIR/$extension/"$CN".key ]; then 
			# echo "try to generate key" 
			rm -f $CERTSDIR/$extension/"$CN".* 
			openssl genrsa -out $KEYSDIR/$extension/"$CN".key $CERTBITS 
		fi 
		if [ -e $KEYSDIR/$extension/"$CN".key -a ! -e $KEYSDIR/$extension/"$CN".pub ]; then 
			openssl rsa -in $KEYSDIR/$extension/"$CN".key -pubout -out $KEYSDIR/$extension/"$CN".pub 
		fi 
		if [ -e $KEYSDIR/$extension/"$CN".key -a ! -e $CERTSDIR/$extension/"$CN".req ]; then 
		rm -f $CERTSDIR/$extension/"$CN".* 
		ca $extension "$SALTN" req | openssl req \
			-new -days $CERTDAYS \
			-nodes -key $KEYSDIR/$extension/"$CN".key \
			-out $CERTSDIR/$extension/"$CN".req \
			-config /dev/stdin \
			-reqexts exts \
			-subj /"$SUBDN"/CN="$CN""$CERTMAIL" 
		else 
		echo -en "\n---> $CN req exists, nothing todo" 
		fi 
		done 
		echo -en "\n\t--> Created all requests from candidates" 
}


function create_keys_certs() { 
	create_requests
	#export CN 
	CN_count=0;
	#echo -en "--- creating keys and certs from hosts file (func_createtls.inc)...\n"
	#mkdir -m 700 -p $CandKEYSDIR 
	# mkdir -m 700 -p $CandCERTSDIR 
	#mkdir -m 700 -p $CANDKEYSDIR 
	#mkdir -m 700 -p $CANDCERTSDIR 
	cat $HOSTDIR/hosts \
	| grep -v "^[ ]*#" \
	| grep -v "^[ ]*$" \
	| grep -v "^[ ]*!" \
	| while read cert; do
		echo -en "\n--> HOST: $cert\n\n" 
		CN=$(echo "$cert" | cut -f2- -d: | cut -f1 -d,) 
		SALTN=$(echo "$cert" | cut -f2- -d: | grep , | cut -f2- -d,) 
		# SALTN is DNS and IP (subjectAltAttributes) 
		# extension is "keyword like tlsserver, tlsboth etc 
		extension=$(echo "$cert" | cut -f1 -d:) 
		# --------------------------------------------------------- 
		if [ -e $CERTSDIR/$extension/"$CN".req -a ! -e $CERTSDIR/$extension/"$CN".crt ]; then 
			mv $CERTSDIR/$extension/"$CN".req $CERTSDIR/$extension/tmp-"$CN".req 
			rm -f $CERTSDIR/$extension/"$CN".* 
			mv $CERTSDIR/$extension/tmp-"$CN".req $CERTSDIR/$extension/"$CN".req 
			ca $extension "$SALTN" sign | openssl ca \
				-batch \
				-days $CERTDAYS \
				-out $CERTSDIR/$extension/"$CN".crt \
				-in $CERTSDIR/$extension/"$CN".req \
				-extensions exts -preserveDN \
				-config /dev/stdin 
		else 
			echo -en "\t--->$CN crt exists, nothing todo\n" 
		fi 
	# --------------------------------------------------------- 
		if [ -e $CERTSDIR/$extension/"$CN".crt -a ! -e $CERTSDIR/$extension/"$CN".cer ]; then 
			openssl x509 \
				-in $CERTSDIR/$extension/"$CN".crt \
				-outform DER \
				-out $CERTSDIR/$extension/"$CN".cer 
			else echo -en "\t---> $CN cer exists, nothing todo\n" 
		fi 
		#--------------------------------------------------------- 
		if [ -e $CERTSDIR/$extension/"$CN".crt -a ! -e $CERTSDIR/$extension/"$CN".p7b ]; then 
			openssl crl2pkcs7 \
				-nocrl \
				-certfile $CERTSDIR/$extension/"$CN".crt \
				-out $CERTSDIR/$extension/"$CN".p7b 
			else 
				echo -en "\t---> $CN p7b exists, nothing todo\n" 
		fi 
			#--------------------------------------------------------- 
		if [ -e $CERTSDIR/$extension/"$CN".crt -a -e $KEYSDIR/$extension/"$CN".key -a ! -e $CERTSDIR/$extension/"$CN".pem ]; then 
			openssl x509 \
				-in $CERTSDIR/$extension/"$CN".crt \
				-out $CERTSDIR/$extension/"$CN".pem; 
			cat $KEYSDIR/$extension/"$CN".key >>$CERTSDIR/$extension/"$CN".pem 
			else 
			echo -en "\t---> $CN pem exists, nothing todo\n" 
			fi 
			# ----------TLS Chain for Duplex----------------------------------------------- 
			if [ -e $CERTSDIR/$extension/"$CN".crt -a -e $KEYSDIR/$extension/"$CN".key -a ! -e $CERTSDIR/$extension/"$CN".chain.pem ]; then 
			openssl x509 \
				-in $CERTSDIR/$extension/"$CN".crt \
				-out $CERTSDIR/$extension/"$CN".chain.pem.tmp
			cat $CERTSDIR/$extension/"$CN".chain.pem.tmp $KEYSDIR/$extension/"$CN".key $ROOTCADIR/root.crt >$CERTSDIR/$extension/"$CN".chain.pem 
			rm $CERTSDIR/$extension/"$CN".chain.pem.tmp 
			else 
			echo -en "\t---> $CN chain.pem exists, nothing todo\n" 
			fi 
			if [ -e $CERTSDIR/$extension/"$CN".crt -a -e $KEYSDIR/$extension/"$CN".key -a ! -e $CERTSDIR/$extension/"$CN".p12 ]; then 
			openssl pkcs12 \
				-export \
				-inkey $KEYSDIR/$extension/"$CN".key \
				-in $CERTSDIR/$extension/"$CN".crt \
				-name "$CN" -out $CERTSDIR/$extension/"$CN".p12 \
				-nodes \
				-passout pass: 
			else
			echo -en "\t---> $CN p12 exists, nothing todo\n" 
			fi 
			#--------------------------------------------------------- 
			if [ -e $CERTSDIR/$extension/"$CN".crt -a -e $KEYSDIR/$extension/"$CN".key -a ! -e $CERTSDIR/$extension/"$CN".chain.p12 ]; then 
			openssl pkcs12 \
				-export -inkey $KEYSDIR/$extension/"$CN".key \
				-in $CERTSDIR/$extension/"$CN".crt \
				-name "$CN" \
				-out $CERTSDIR/$extension/"$CN".p12 \
				-certfile $ROOTCADIR/root.crt \
				-nodes \
				-passout pass: 
			else 
			echo -en "\t---> $CN p12.chain exists, nothing todo\n" 
			fi 
			#--------------------------------------------------------- 
			if [ -e $CERTSDIR/$extension/"$CN".crt -a -e $KEYSDIR/$extension/"$CN".key -a ! -e $CERTSDIR/$extension/"$CN"."$PassPhrase".p12 ]; then 
				openssl pkcs12 \
				-export \
				-inkey $KEYSDIR/$extension/"$CN".key \
				-in $CERTSDIR/$extension/"$CN".crt \
				-name "$CN" -out $CERTSDIR/$extension/"$CN"."$PassPhrase".p12 \
				-nodes -passout pass:"$PassPhrase"
			else
			echo -en "\t---> $CN XXX.p12 with passphrase XXX exists, nothing todo\n" 
			fi 
			# --------------------------------------------------------- 
			if [ -e $CERTSDIR/$extension/"$CN".crt -a -e $KEYSDIR/$extension/"$CN".key -a ! -e $CERTSDIR/$extension/"$CN".chain."$PassPhrase".p12 ]; then 
				openssl pkcs12 \
				-export \
				-inkey $KEYSDIR/$extension/"$CN".key \
				-in $CERTSDIR/$extension/"$CN".crt \
				-name "$CN" \
				-out $CERTSDIR/$extension/"$CN"."$PassPhrase".p12 \
				-certfile $ROOTCADIR/root.crt \
				-nodes \
				-passout pass:"$PassPhrase" 
			else 
			echo -en "\t---> $CN chain.xxx.p12 with passphrase XXX exists, nothing todo\n" 
			fi
			# --------- special cases ------------------ 
			#------------------------------------------ 
			# ----------- if OSMO---------------------- 
			if [ $extension = 'osmo' ]; then
				echo "OSMO special ...." 
				if [ -e $CERTSDIR/$extension/"$CN".chain.pem -a -e $KEYSDIR/$extension/"$CN".key -a ! -e $CERTSDIR/$extension/"$CN".osmc ]; then 
					openssl pkcs12 \
					-export \
					-out $CERTSDIR/$extension/"$CN".p12 \
					-inkey $KEYSDIR/$extension/"$CN".key \
					-in $CERTSDIR/$extension/"$CN".chain.pem \
					-password pass:$PassPhrase 
					cp $CERTSDIR/$extension/"$CN".p12 $CERTSDIR/$extension/"$CN".osmc 
				# Install .osmc und in Smartphone 
				else 
				echo -en "\t---> $CN OSMO.pem exists, nothing todo\n" 
			fi 
		fi 
	#--------------------------------------------------------- 
	# -------- UC Server 
		if [ $extension = 'ucfrontend' ]; then 
			if [ -e $CERTSDIR/$extension/"$CN".crt -a -e $KEYSDIR/$extension/"$CN".key -a ! -e $CERTSDIR/$extension/"$CN".UC.pem ]; then
				openssl x509 \
				-in $CERTSDIR/$extension/"$CN".crt \
				-out $CERTSDIR/$extension/"$CN".UC.pem.tmp 
				cat $ROOTCADIR/root.crt $CERTSDIR/$extension/"$CN".UC.pem.tmp >$CERTSDIR/$extension/"$CN".UCchain.pem 
				rm $CERTSDIR/$extension/"$CN".UC.pem.tmp 
				else 
				echo -en "\t---> $CN UC.pem exists, nothing todo\n" 
			fi 
		fi 
	# -------- Assistant 
		if [ $extension = 'assistant' ]; then 
			echo "writing README an temp certfile ..." 
			# merge files 
			cat $ROOTCADIR/root.crt $KEYSDIR/$extension/"$CN".key >$CERTSDIR/$extension/"$CN".temp_certfile.pem 
			cat >$CERTSDIR/$extension/README.txt <<-EOL 
			Final steps to do on Assistant/Bcom: 
			1) Copy the file "...temp_certfile.pem" to the backend machine 
			2) From the directory where you copied the file "temp_certfile.pem", 
			execute: /opt/siemens/assistant/scripts/generateKeyStore.sh temp_certfile.pem 
			3) Because the file "temp_certfile.pem" was deleted after the last step, copy it to the backend machine again 4) From the directory where you copied the file "temp_certfile.pem", execute: /opt/siemens/servicetools/bcom/bcomImportCert.sh temp_certfile.pem 
			5) Restart Symphonia 
			6) You are done. 
			EOL
			fi 
	#--------------------------------------------------------- 
	# -------- Facade 
		if [ $extension = 'ucfacade' ]; then 
			echo "no special handling for $extension" 
			# merge files 
			cat >$CERTSDIR/$extension/README.txt <<-EOL
			Final steps to do on FACADE: 
			1) Copy the rootca and p12 to FACADE
			2) Import the certfile.p12 into the keystore 
			3) Import the root.cer into the truststore: 
			/opt/siemens/share/java/jre/bin/keytool -import -keystore
			/usr/local/facade/apache-tomcat-7.0.52/conf/Tomcat-6270_trust.jks
			-trustcacerts -file /root/pki/rootca/root.cer -alias Tomcat-6270_trustcacerts 
			EOL
			fi 
	#--------------------------------------------------------- 
	# ----------- 	if OSB ------------------------------------------ 
		if [ $extension = 'osb' ]; then 
		#echo "Branch special ...." 
			if [ -e $KEYSDIR/$extension/"$CN".key -a ! -e $KEYSDIR/$extension/"$CN"_privatekey.pem ]; then
			cp $KEYSDIR/$extension/"$CN".key $KEYSDIR/$extension/"$CN"_privatekey.pem 
			# For Installation in OSB and SBC 
			else 
			echo -en "\t---> $CN OSB Private Keyfile exists, nothing todo\n" 
			fi 
			cat >$KEYSDIR/$extension/README.txt <<-EOL
			Final steps to do on OpenScape Branch GUI: 
			1) Upload the privat key 
			2) Upload the rootCA.pem 
			3) Upload the osb certificate 
			4) Create a new certificate profile 
			5) activate the profile 
			6) apply the new configuration 
			EOL
			fi 
	# ----------- if OSB ------------------------------------------ 
		if [ $extension = 'osbgui' ]; then 
		#echo "Branch GUI special ...." 
			if [ -e $KEYSDIR/$extension/"$CN".key -a ! -e $KEYSDIR/$extension/"$CN"_privatekey.pem ]; then
			cp $KEYSDIR/$extension/"$CN".key $KEYSDIR/$extension/"$CN"_privatekey.pem 
			# For Installation in OSB and SBC 
			else 
			echo -en "\t---> $CN OSB GUI Private Keyfile exists, nothing todo\n" 
			fi 
			cat >$KEYSDIR/$extension/README_OSBGUI.txt <<-EOL
			Final steps to do on OpenScape Branch GUI: 
			1) Upload the privat key 
			2) Upload the rootCA.pem
			3) Upload the osb certificate 
			4) Create a new certificate profile 
			5) activate the profile 
			6) apply the new configuration 
			EOL
			fi 
	# ----------- if OS SBC------------------------------------------ 
		if [ $extension = 'ossbc' ]; then 
		#
		echo "SBC special ...." 
		if [ -e $KEYSDIR/$extension/"$CN".key -a ! -e $KEYSDIR/$extension/"$CN"_privatekey.pem ]; then
		cp $KEYSDIR/$extension/"$CN".key $KEYSDIR/$extension/"$CN"_privatekey.pem 
		# For Installation in SBC 
		else 
		echo -en "\t---> $CN SBC Private Keyfile exists, nothing todo\n" 
		fi 
		cat >$KEYSDIR/$extension/README.txt <<-EOL
		Final steps to do on OpenScape Branch GUI: 
		1) Upload the privat key 
		2) Upload the rootCA.pem 
		3) Upload the SBC certificate 
		4) Create a new certificate profile 
		5) activate the profile 
		6) apply the new configuration 
		EOL
		fi 
		done 
		echo -en "\n\t--> Requested an signed all candidates" 
		
		}
