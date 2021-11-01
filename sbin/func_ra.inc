# PKIT (c) 2019 IT Consulting Wolfinger
# dont't change the file
# master functions for signing processes
# 
# todo# cleanup cert directories from unneccessary files 


function SignRequest(){ 
	##echo -en "---------- debug----------\n";read
	export CN
	CN_count=0;
	
	echo -en "--- Reading external certificates ...\n" 
	cat $HOSTDIR/candreq \
		| grep -v "^[ ]*#" \
		| grep -v "^[ ]*$" \
		| grep -v "^[ ]*!" \
		| while  read cert; do  
			extension=$(echo "$cert" | cut -f1 -d:) 
			  CN=$(echo "$cert" | cut -f2- -d: | cut -f1 -d,)
			  SALTN=$(echo "$cert" | cut -f2- -d: | grep , | cut -f2- -d,)
	
	if [ -e $CANDCERTSDIR/$extension/"$CN".req -a ! -e $CANDCERTSDIR/$extension/"$CN".crt ]; then 
		mv $CANDCERTSDIR/$extension/"$CN".req $CANDCERTSDIR/$extension/tmp-"$CN".req 
		rm -f $CANDCERTSDIR/$extension/"$CN".* 
		mv $CANDCERTSDIR/$extension/tmp-"$CN".req $CANDCERTSDIR/$extension/"$CN".req 
		echo "Debugging info: "
		echo "This dir = $(pwd)"
		echo "Variable CANDCERTSDIR = $CANDCERTSDIR"
		echo "DIR of $CANDCERTSDIR "
		echo "Database: $DATABASE"
		echo "Common Name CN = $CN"
		dir $CANDCERTSDIR
		
		# -create_serial is especially important. Many HOW-TOs will have you echo "01" into the serial file thus starting the serial number at 1, and using 8-bit serial numbers instead of 128-bit serial numbers. This will generate a random 128-bit serial number to start with. The randomness helps to ensure that if you make a mistake and start over, you won't overwrite existing serial numbers out there.
		### external ca for signing or external requests
		
		now=$(date +"%Y-%m-%d-%S")
		
		CNOUT=$CN$now
		echo "### Randomized CN: $CNOUT"
		read 
		
		mv $CANDCERTSDIR/$extension/"$CN".req $CANDCERTSDIR/$extension/$CNOUT.req
		read 
		
		set CN=$CNOUT
		export CNOUT
		
		# extrem variante serial number randomize
		# -------------------------
		
		if [ "$BSIMODE" -eq "1" ]; then
		declare -i key 
			  key=$(cat $ROOTCADIR/ra-count)
			  echo $(($key + 1)) >$ROOTCADIR/ra-count
			  openssl rand -hex $SERIALSTART >$ROOTCADIR/newRandom
			  echo $(cat $ROOTCADIR/ra-count)$(cat $ROOTCADIR/newRandom) >$ROOTCADIR/root-serial
			  if [ "$SERIALHASH" = 'md5' ]; then
				  md5sum $ROOTCADIR/root-serial >$ROOTCADIR/root-serial.md5
				  awk '{print $1}' $ROOTCADIR/root-serial.md5 >$ROOTCADIR/root-serial
				  rm -f $ROOTCADIR/root-serial.md5
			  fi
				  if [ "$SERIALHASH" = 'sha1' ]; then
				  sha1sum $ROOTCADIR/root-serial >$ROOTCADIR/root-serial.sha1
				  awk '{print $1}' $ROOTCADIR/root-serial.sha1 >$ROOTCADIR/root-serial
				  rm -f $ROOTCADIR/root-serial.sha1
			  fi
			  
			 
			  
		fi
		# ---------------------------------------------
		# -extensions exts -preserveDN \
		#
		ca $extension "$SALTN" sign | openssl ca \
				-batch \
				-days $CERTDAYS \
				-out $CANDCERTSDIR/$extension/$CNOUT.crt \
				-in $CANDCERTSDIR/$extension/$CNOUT.req \
				-name ca4ext \
				-extensions exts4exts \
				-preserveDN \
				-config /dev/stdin \
				>>$LASTLOG
 
				#	echo "stop";read
				echo "--- creating pem from crt ----"
					
				openssl x509 \
				-in $CANDCERTSDIR/$extension/$CNOUT.crt \
				-out $CANDCERTSDIR/$extension/$CNOUT.pem; 
		echo -en "\t---> $CN crt and pem created\n" 
		echo -en "==============copy from here ===================\n"
		cat $CANDCERTSDIR/$extension/$CNOUT.pem
		echo 		
		echo -en "=========copy to here============\n"
		echo "ENTER to continue"; read
		echo " you find the new certificate in directory:"
		echo " -->$CANDCERTSDIR/$extension	"
		echo "ENTER to continue"; read
	else 
		echo -en "\t---> $CN Error, no cert written\n"
			echo -en "\t---> REQ does not exist or CRT already exists\n"
	fi
	done 
	echo -en "\n\t--> Press ENTER to continue" 
	read 
} 

function importcreq() {
	dos2unix $HOSTDIR/candreq
	certArray=($(cat $HOSTDIR/candreq \
		| grep -v "^[ ]*#" \
		| grep -v "^[ ]*$" \
		| grep -v "^[ ]*!" \
		)) 
	tLen=${#certArray[@]}
	echo -en "Found $tLen entries in external candidates\n"
	echo -en "------------------------------------------\n"
	echo -en " Number\tSystem:Alias\n------------------------------\n"
	for (( i=0; $i<$tLen; i++ )); do 
		echo -en " $i --> \t${certArray[$i]}\n" 
	done
	echo -en "\nChoose number from list...\n"
	echo -en "------------------------------------------\n"
	echo -en "Next, the tool will open the vi editor,   \n"
	echo -en ".. paste the X509 request pem block into ...\n"
	echo -en " !!!! The CN inside the request MUST NOT exist\n"
	echo -en " within the local CA Database !!!!!!!!!!!!!!!\n"
	echo -en "... now the number, please: "
	read CandNum
	# debugging 
	echo -en "Candidate: ${certArray[CandNum]}\n";
	cert=${certArray[CandNum]}
	
	# debugging  
	echo -en "Certificate: $cert\n";
	
	CN=$(echo "$cert" | cut -f2- -d: | cut -f1 -d,) 
	# debugging  
	echo -en "CN: $CN\n";
	SALTN=$(echo "$cert" | cut -f2- -d: | grep , | cut -f2- -d,) 
	extension=$(echo "$cert" | cut -f1 -d:)
	# debugging  
	echo -en "Extension: $extension\n";	
	Alias=$(echo "$CN")
	# debugging 
	echo -en "Alias: $Alias\n";
	Vendor=$(echo "$extension") 	
	# debugging   
	echo -en "Vendor: $Vendor\n";
	
	echo -en " --> Vendor $Vendor with certname $Alias\n"
	
	#	echo -en "Choose Vendor from list: \n";read CandNum
	#	echo -en "-- debugging variable i after while read = $i\n" 
	#	echo -en "-- CANDCERTSDIR = $CANDCERTSDIR\n"
	#	echo -en "-- Candidate Number:  = $CandNum\n"
	#	echo -en "-- Variable i :  = $i\n"
	#	echo -en "-- Vendor = $Vendor\n"
	#	echo -en "Requestfile = $CANDCERTSDIR/$Vendor/$Alias.req\n"
		# Requestfile = ./candidates/CandCerts//.req
		# -- Reuestfilename = .req
		echo -en "-- Requestfilename = $Alias.req\n ENTER ...."
		read
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
	#exec 2>> $LASTLOG 
	reInitVars
	export CN
	CN_count=0; 
	echo -en "--- creating certificate requests from hosts file\n" >>$LASTLOG
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
			-subj '/'"$SUBDN"'/CN='"$CN"
			
			echo "--- key and req for $CN created ..." | tee >>$LASTLOG 
			# $CN must be unique within this PKI
		else 
		echo -en "\n---> $CN req exists, nothing todo" 
		fi 
		done 
		echo -en "--- Created all requests from candidates\n" >>$LASTLOG
}


function create_keys_certs() {
	# general signing process
	# NEW OPTION -set_serial n
	# p12 with intermediate CA if SUBCA
	# $ROOTCADIR/isSuBCA.cnf
	# $TopLevelCAFile
	# if $ROOTCADIR/isSuBCA.cnf == 1 then create p12 chains with TopCA
	
	reInitVars
	echo "--- starting signing process ..." >>$LASTLOG
	. $PKIHOME/pkiconf/pkitool.conf # load current passphrase
	if [ "$HidePassPhrase" -eq "1" ]; then
	P12FileNamePass="_x_x"
	else
	P12FileNamePass=$PassPhrase
	fi
	create_requests
	check_rootCA
		if [ "$rootCACheck" -gt "0" ]; then
		#if [[ $rootCACheck=1 ]]; then 
		echo "There are rootCA Errors ... Inititialise RootCA first ...[ENTER]"
		read
		exit;
		fi
		export CN 
	CN_count=0;
	#read -p "break1"
	# ask for root key passphrase if set
	# test if root.key is encrypted
	_encrypted=$(grep ENCRYPTED $ROOTCADIR/root.key)
	if [ "$_encrypted" != '' ]; then
	echo "The rootCA key is encrypted, enter the passphrase, please:"
	echo
	read _passphrase
	
	fi
	cat $HOSTDIR/hosts \
	| grep -v "^[ ]*#" \
	| grep -v "^[ ]*$" \
	| grep -v "^[ ]*!" \
	| while read cert; do
	#read -p "break2"
		echo -en "\n--> HOST: $cert\n\n" 
		CN=$(echo "$cert" | cut -f2- -d: | cut -f1 -d,) 
		SALTN=$(echo "$cert" | cut -f2- -d: | grep , | cut -f2- -d,) 
		# SALTN is DNS and IP (subjectAltAttributes) 
		# extension is "keyword like tlsserver, tlsboth etc 
		extension=$(echo "$cert" | cut -f1 -d:) 
		FriendlyName=''
		# ---------------------------------------------------------
		# adaptions for special cases 
		#
		# ---------------------------------------------------------
		
		if [ -e $CERTSDIR/$extension/"$CN".req -a ! -e $CERTSDIR/$extension/"$CN".crt ]; then 
			mv $CERTSDIR/$extension/"$CN".req $CERTSDIR/$extension/tmp-"$CN".req 
			rm -f $CERTSDIR/$extension/"$CN".* 
			mv $CERTSDIR/$extension/tmp-"$CN".req $CERTSDIR/$extension/"$CN".req 
			# here randomize serial number
			# increment 2 byte counter
			# create random hex number with length
			# merge counter with random >$ROOTCADIR/root-serial
			# openssl rand -hex $SERIALSTART >$ROOTCADIR/newRandom
			# ------------------------
			if [ "$BSIMODE" -eq "1" ]; then
			# here we declare the new serial start value
			# ra-count means the database incremental unique index
			declare -i key 
			  key=$(cat $ROOTCADIR/ra-count)
			  echo $(($key + 1)) >$ROOTCADIR/ra-count
			  openssl rand -hex $SERIALSTART >$ROOTCADIR/newRandom
			  echo $(cat $ROOTCADIR/ra-count)$(cat $ROOTCADIR/newRandom) >$ROOTCADIR/root-serial
				if [ "$SERIALHASH" = 'md5' ]; then
				  md5sum $ROOTCADIR/root-serial >$ROOTCADIR/root-serial.md5
				  awk '{print $1}' $ROOTCADIR/root-serial.md5 >$ROOTCADIR/root-serial
				  rm -f $ROOTCADIR/root-serial.md5
				fi
				if [ "$SERIALHASH" = 'sha1' ]; then
				  sha1sum $ROOTCADIR/root-serial >$ROOTCADIR/root-serial.sha1
				  awk '{print $1}' $ROOTCADIR/root-serial.sha1 >$ROOTCADIR/root-serial
				  rm -f $ROOTCADIR/root-serial.sha1
				fi
			  echo "--- signing process for $CN started | serial ($SERIALHASH)# = $(cat $ROOTCADIR/root-serial) ..." >>$LASTLOG
			fi
	# ------------------------  create the initial cert from request (all cases) 
			
			# first optional check if request is ok
			# openssl req -text -noout -verify -in $CERTSDIR/$extension/"$CN".req 
			# check $? if 0 ?
			ca $extension "$SALTN" sign | openssl ca \
				-batch \
				-days $CERTDAYS \
				-out $CERTSDIR/$extension/"$CN".crt \
				-in $CERTSDIR/$extension/"$CN".req \
				-extensions exts -preserveDN  \
				-passin pass:$_passphrase \
				-config /dev/stdin >>$LASTLOG
			_crtFileSize=$(stat -c %s $CERTSDIR/$extension/"$CN".crt)
						
			# if [[ -s diff.txt ]]; then echo "file has something"; else echo "file is empty"; fi
			if [[ -s $CERTSDIR/$extension/"$CN".crt ]]; then  # -s test file greater than zero
				echo -e $green "--- crt files for $CN created (returncode = $?) (crt filesize=$_crtFileSize ..." $black >>$LASTLOG
				
			else
				echo -e $red "--- we have a problem with $CN or root-serial or ra-count !! (returncode=$?) (crt filesize=$_crtFileSize ..." $black >>$LASTLOG
				echo -e $red "--- nothing created !!! " $black >>$LASTLOG
				return
			fi
		else 
			echo -en "\t--->$CN crt exists, nothing todo\n" >>$LASTLOG 
		fi 
		
	# --------------------------------------------------------- 
		if [ -e $CERTSDIR/$extension/"$CN".crt -a ! -e $CERTSDIR/$extension/"$CN".cer ]; then 
			openssl x509 \
				-in $CERTSDIR/$extension/"$CN".crt \
				-outform DER \
				-out $CERTSDIR/$extension/"$CN".cer 
			echo "--- cer files for $CN created ..." >>$LASTLOG 
			else echo -en "\t---> $CN cer exists, nothing todo\n" >>$LASTLOG  
		fi 
		#--------------------------------------------------------- 
		if [ -e $CERTSDIR/$extension/"$CN".crt -a ! -e $CERTSDIR/$extension/"$CN".p7b ]; then 
			openssl crl2pkcs7 \
				-nocrl \
				-certfile $CERTSDIR/$extension/"$CN".crt \
				-out $CERTSDIR/$extension/"$CN".p7b 
			echo "--- p7b files for $CN created ..." >>$LASTLOG
			else 
				echo -en "\t---> $CN p7b exists, nothing todo\n" 
		fi 
			#--------------------------------------------------------- 
		if [ -e $CERTSDIR/$extension/"$CN".crt -a -e $KEYSDIR/$extension/"$CN".key -a ! -e $CERTSDIR/$extension/"$CN".pem ]; then 
			openssl x509 \
				-in $CERTSDIR/$extension/"$CN".crt \
				-out $CERTSDIR/$extension/"$CN".pem 
			echo "--- pem files for $CN created ..." >>$LASTLOG 
			
			if [ $extension = 'osvserver' ]; then
			cat $KEYSDIR/$extension/"$CN".key >>$CERTSDIR/$extension/"$CN".pem 
			fi
			if [ $extension = 'osvclient' ]; then
			cat $KEYSDIR/$extension/"$CN".key >>$CERTSDIR/$extension/"$CN".pem 
			fi
			
			
			else 
			echo -en "\t---> $CN pem exists, nothing todo\n" 
			fi 
			# ----------TLS Chain if subca----------------------------------------------- 
			
			if [ -e $CERTSDIR/$extension/"$CN".crt -a -e $KEYSDIR/$extension/"$CN".key -a ! -e $CERTSDIR/$extension/"$CN".chain.pem  -a -e $ROOTCADIR/trustchain.crt ]; then 
			openssl x509 \
				-in $CERTSDIR/$extension/"$CN".crt \
				-out $CERTSDIR/$extension/"$CN".chain.pem.tmp 
			echo "--- *.chain.pem files for $CN created ..." >>$LASTLOG
			cat $CERTSDIR/$extension/"$CN".chain.pem.tmp  $ROOTCADIR/trustchain.crt $KEYSDIR/$extension/"$CN".key >$CERTSDIR/$extension/"$CN".chain.pem 
			rm $CERTSDIR/$extension/"$CN".chain.pem.tmp 
			else 
			echo -en "\t---> $CN chain.pem exists, nothing todo\n" 
			fi 
			# -------- case UC Server - name = tomcat
		if [ $extension = 'ucfrontend' ] || [ $extension = 'ucbackend' ];
		then 
			if [ -e $CERTSDIR/$extension/"$CN".crt -a -e $KEYSDIR/$extension/"$CN".key -a ! -e $CERTSDIR/$extension/"$CN".UC.pem ]; 
			then
				openssl x509 \
				-in $CERTSDIR/$extension/"$CN".crt \
				-out $CERTSDIR/$extension/"$CN".UC.pem.tmp 
			echo "--- *.UCchain.pem files for $CN created ..." >>$LASTLOG
			
				cat $ROOTCADIR/root.crt $CERTSDIR/$extension/"$CN".UC.pem.tmp >$CERTSDIR/$extension/"$CN".UCchain.pem 
				rm $CERTSDIR/$extension/"$CN".UC.pem.tmp 
			else 
				echo -en "\t---> $CN UC.pem exists, nothing todo\n" 
			fi
						
			# ------------------ P12 CHAIN with pass-----------------------
				if [ -e $CERTSDIR/$extension/"$CN".crt -a -e $CERTSDIR/$extension/"$CN".chain.pem -a -e $KEYSDIR/$extension/"$CN".key -a -e $ROOTCADIR/trustchain.crt -a ! -e $CERTSDIR/$extension/"$CN".chain."$P12FileNamePass".p12 ]; then 
				#openssl pkcs12 \
				-export \
				-inkey $KEYSDIR/$extension/"$CN".key \
				-in $CERTSDIR/$extension/"$CN".crt \
				-name "tomcat" \
				-out $CERTSDIR/$extension/"$CN".chain."$P12FileNamePass".p12 \
				-certfile $ROOTCADIR/trustchain.crt \
				-nodes \
				-passout pass:"$PassPhrase"
				
				openssl pkcs12 \
				-export \
				-inkey $KEYSDIR/$extension/"$CN".key \
				-in $CERTSDIR/$extension/"$CN".chain.pem \
				-name "tomcat" \
				-out $CERTSDIR/$extension/"$CN".chain."$P12FileNamePass".p12 \
				-nodes \
				-passout pass:"$PassPhrase" 
				echo "--- *.chain.p12 file with passphrase for $CN created ..." >>$LASTLOG
				else 
			
				echo -en "\t---> $CN chain.xxx.p12 with passphrase XXX exists or no subca, nothing todo\n" >>$LASTLOG
				fi
				# ------------------- P12 CHAIN without pass
					if [ -e $CERTSDIR/$extension/"$CN".crt -a -e $CERTSDIR/$extension/"$CN".chain.pem -a -e $KEYSDIR/$extension/"$CN".key -a -e $ROOTCADIR/trustchain.crt -a ! -e $CERTSDIR/$extension/"$CN".chain.p12 ]; 
					then 
					openssl pkcs12 \
					-export \
					-inkey $KEYSDIR/$extension/"$CN".key \
					-in $CERTSDIR/$extension/"$CN".chain.pem \
					-name "tomcat" \
					-out $CERTSDIR/$extension/"$CN".chain.p12 \
					-nodes \
					-passout pass: 
					echo "--- *chain.p12 files without passphrase for $CN created ..." >>$LASTLOG
				else 
					echo -en "\t---> $CN chain.p12 with empty passphrase exists or no subca, nothing todo\n" >>$LASTLOG
				fi
				cat >$CERTSDIR/$extension/README_aboutp12.txt <<-EOL 
				About the p12 friendly name for UC: 
				For UC TOMCAT server the CN must be 'tomcat'. 
				You can check this by opening the keybag with:
				# openssl pkcs12 -in [filename].chain.p12 | less
				# 
				EOL
				fi 
			
		
			# --------------  p12 for all other cases ------------------
			# -------------- write standard p12 keybags without pass - name = $CN -----------
			## enhance check [ $extension != 'osvservcer' ] && .....
			if  [ -e $CERTSDIR/$extension/"$CN".crt -a -e $KEYSDIR/$extension/"$CN".key -a ! -e $CERTSDIR/$extension/"$CN".p12 ]; then 
			openssl pkcs12 \
				-export -inkey $KEYSDIR/$extension/"$CN".key \
				-in $CERTSDIR/$extension/"$CN".crt \
				-name "$CN" \
				-out $CERTSDIR/$extension/"$CN".p12 \
				-certfile $ROOTCADIR/root.crt \
				-nodes \
				-passout pass: 
			echo "--- *.p12 files without passphrase for $CN created ..." >>$LASTLOG
			else 
			echo -en "\t---> $CN .p12 with empty passphrase exists, nothing todo\n"  >>$LASTLOG
			fi 
			# -------------- write standard p12 keybags with pass - name = $CN -----------
			if [ -e $CERTSDIR/$extension/"$CN".crt -a -e $KEYSDIR/$extension/"$CN".key -a ! -e $CERTSDIR/$extension/"$CN"."$P12FileNamePass".p12 ]; then 
				openssl pkcs12 \
				-export \
				-inkey $KEYSDIR/$extension/"$CN".key \
				-in $CERTSDIR/$extension/"$CN".crt \
				-name "$CN" -out $CERTSDIR/$extension/"$CN"."$P12FileNamePass".p12 \
				-nodes -passout pass:"$PassPhrase" 
			echo "--- *.p12 files with passphrase for $CN created ..." >>$LASTLOG
			else
			echo -en "\t---> $CN XXX.p12 with passphrase XXX exists, nothing todo\n" >>$LASTLOG
			fi 
			# ------------------------p12 chain --------------------------------- 
			if [ -e $CERTSDIR/$extension/"$CN".crt -a -e $CERTSDIR/$extension/"$CN".chain.pem -a -e $KEYSDIR/$extension/"$CN".key -a -e $ROOTCADIR/trustchain.crt  -a ! -e $CERTSDIR/$extension/"$CN".chain."$P12FileNamePass".p12 ]; then 
				openssl pkcs12 \
				-export \
				-inkey $KEYSDIR/$extension/"$CN".key \
				-in $CERTSDIR/$extension/"$CN".chain.pem \
				-name "$CN" \
				-out $CERTSDIR/$extension/"$CN".chain."$P12FileNamePass".p12 \
				-nodes \
				-passout pass:"$PassPhrase" 
			echo "--- *.chain.p12 files with passphrase for $CN created ..." >>$LASTLOG
			else 
			echo -en "\t---> $CN chain.xxx.p12 with passphrase XXX exists or no subca, nothing todo\n"  >>$LASTLOG
			fi
			# --------- special cases ------------------ 
			#------------------------------------------ 
			# ----------- if osvserver ----------------
			if [ $extension = 'osvserver' ]; then
			
			echo "creating OSV script ...."
			osvscripter
			fi
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
			echo "--- OSMO *chain.p12 with passphrase $CN created ..." >>$LASTLOG
					cp $CERTSDIR/$extension/"$CN".p12 $CERTSDIR/$extension/"$CN".osmc 
				# Install .osmc und in Smartphone 
				else 
				echo -en "\t---> $CN OSMO.pem exists, nothing todo\n"  >>$LASTLOG
			fi 
		fi 
	#--------------------------------------------------------- 
	
	# -------- CMP -------
		if [ $extension = 'cmp' ]; then
				keystorechanger
				keystorechanger_local
		fi
	# -------- Assistant 
		if [ $extension = 'assistant' ]; then 
			echo "writing README and temp certfile ..." 
			# merge files 
			cat $ROOTCADIR/root.crt $KEYSDIR/$extension/"$CN".key >$CERTSDIR/$extension/"$CN".temp_certfile.pem 
			cat >$CERTSDIR/$extension/README.txt <<-EOL 
			Final steps to do on Assistant/Bcom: 
			1) Copy the file "...temp_certfile.pem" to the backend machine 
			2) From the directory where you copied the file "temp_certfile.pem", 
			execute: /opt/siemens/assistant/scripts/generateKeyStore.sh temp_certfile.pem 
			3) Because the file "temp_certfile.pem" was deleted after the last step, copy it to the backend machine again 
			4) From the directory where you copied the file "temp_certfile.pem", execute: /opt/siemens/servicetools/bcom/bcomImportCert.sh temp_certfile.pem 
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
		
		# ----------- if OS SBCGUI------------------ 
		if [ $extension = 'sbcgui' ]; then 
		#
		echo "SBC WEB Server special ...." 
		if [ -e $KEYSDIR/$extension/"$CN".key -a ! -e $KEYSDIR/$extension/"$CN"_privatekey.pem ]; then
		cp $KEYSDIR/$extension/"$CN".key $KEYSDIR/$extension/"$CN"_privatekey.pem 
		# For Installation in SBC 
		else 
		echo -en "\t---> $CN SBC Private Keyfile exists, nothing todo\n" 
		fi 
		cat >$KEYSDIR/$extension/README.txt <<-EOL
		Final steps to do on OpenScape SBC GUI: 
		1) Upload the privat key 
		2) Upload the rootCA.pem 
		3) Upload the SBC certificate 
		4) Create a new certificate profile 
		5) activate the profile 
		6) apply the new configuration 
		EOL
		fi
		done 
		echo -en "\n\t--> Requested and signed all candidates" 
		# last step
		# UpdateCRL;
		
		}
		
