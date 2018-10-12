export MenuName
function menu0 () { 
	# main menu 
	let LOOPCNT++ 
	MenuLevel=0
	MenuName="MAIN"
	PROMPT_2="@$HOSTNAME[$MenuName] : "
	clear
	menu_header 
	#\t5 - Show current canditates file 
	#\t6 - Show example candidates file 
	echo 
	echo -en "\t90 - CA Management\n" 
	echo -en "\t94 - Ext Candidate Management\n" 
	echo -en "\t95 - TLS/SSL Tools\n" 
	echo -en "\t96 - PKI Tool Management\n" 
	echo -en "\t97 - PKI Tool Config \n" 
	if ([ $(whoami) == 'srx' ] && [ $osvServerType != 'Linux' ]) || ([ $DEBUGMODE -eq 1 ]); then 
		echo -en "" 
		echo -en "\t98 - # Maintain PKI on OSV \n" 
	fi
	
	echo -en "\th  - Help 
	\tq - Quit or <CTRL>-c\n " 
	ESC=$'\e' 
	read -p "$PROMPT_1" userchoice
	keypress=$(echo $userchoice | tr -d "\n") 
	Lastaction=$keypress 
		case $keypress in 
			[6] ) #show hosts 
				PrintDefaultHosts; 
			;; 
			[Hh] ) # manual 
				LessHelp; 
				#manual the old help 
			;; 
			98 ) # srx Menu 
				if [ $Toolmode=="OSV" ]; then 
					menu1 
				else # do nothing 
				Toolmode="Linux" 
				fi 
			;; 
			97 ) # tool config Menu 
				menu2 
				;; 
			96 ) # PKI Management Menu 
				menu3 
				;; 
			95 ) # TLS/OpenSSL Tools 
				menu4 
				;; 
			94 ) # candidate Management 
				menu5 
				;; 
			90 ) # CA Management 
				menu6 
				;; 
			OSV ) # PKIManagement Menu 
				ToggleDebugMode 
				;; 
			[Qq] ) 
				exit 0 
				;; 
			:* ) 
				# Cli
				pkitcli	
				;;
			esac 
} 
function menu2 () { 
	# config menu 
	MenuLevel=2
	MenuName="CONFIG"
	PROMPT_2="@$HOSTNAME[$MenuName] : "
	let LOOPCNT++ 
	clear 
	menu_header 
	echo -en "\n--> \tSelect action:\n 
	\tv - Configure PKI-Tool 
	\t Remote FTP Archive and Passphrase for pkcs12 
	\t99 - Return\n\n" 
	read -p "--> $MenuLevel$PROMPT_2" userchoice
	keypress=$(echo $userchoice | tr -d "\n") 
	Lastaction=$keypress 
		case $keypress in 
			[Vv] ) # edit and reload PKI Tool Config 
				editConfig;
				checkConfig; 
				;; 
			:* ) 
				# Cli
				pkitcli	
				;;	
			99 ) # main menu 
				menu0 
				;;
		esac 
} 

function menu3 () { 
	# maintenance menu 
	MenuLevel=3
	MenuName="MAINT"
	PROMPT_2="@$HOSTNAME[$MenuName] : "
	let LOOPCNT++ 
	clear
	menu_header
	echo -en "\n#\tPKI MAINTENANCE ------------>" 
	echo -en "\n-->\tSelect action:\n 
	\tI - Re-Initialize Tool (Plugins $PLUGINV) 
	\tip - Show System IPs 
	\tC - Clone PKI to new project
	\tS - Switch to PKI project 
	\tD - Destroy entire project (this level) 
	\tK - Delete DiffiHellman Files
	\t7 - Remove&Revoke all external candidates certificates	
	\t8 - Delete all Backups (ssl and project) 
	\t9 - Cleanup project - will delete all candidates 
	\tA - Get/Update PKI project from FTP Server 
	\tB - Put/Upload PKI project to FTP Server 
	\tE - Export CAND keys and Certs to ZIP
	\tZ - ZIP entire PKI with all subdirs 
	\t99 - Return\n\n" 
	read -p "--> $MenuLevel$PROMPT_2" userchoice 
	keypress=$(echo $userchoice | tr -d "\n") 
	Lastaction=$keypress 
	case $keypress in 
		[I] ) #read X509 attributes and host file 
			read_fcts; 
			read_x509config; 
			# checkConfig;
			;; 
		ip ) # read IPs as root 
			readIP;
			;; 
		A) #Get/Update PKI from FTP Server 
			echo "not implemented jet, press key ...." 
			read #ftpget_pki; 
			;; 
		B) #PutPKI to FTP Server 
			echo "is beta ... , press key ...." 
			read ftpput_pki; 
			;; 
		[C] )# clone pki 
			clonepki;
			;; 
		[D] ) # destroy simple pki 
			destroypki 
			;; 
		[K] ) # delete diffihelman 
			deleteDHFiles 
			;; 
		[7] ) # delete ext candidates certs, without requests
			Revoke_extcandCRL;
			remove_extcerts;
			UpdateCRL;
			
			;;
		[8] ) #certs_remove
			remove_archives; 
			;; 
		[9] ) #certs_remove 
			remove_allcerts; 
			;;
				
		[Z] ) # zip entire PKI 
			ziptool; 
			;; 
		[E] ) # export CAND keys and Certs to ZIP	
			exportCAND;
			;;
		[S] ) # switch to runlevel PKI 
			changePKILevel; 
			;;
		:* ) 
			# Cli
			pkitcli	
			;;	
		99 ) # main menu 
			menu0 
			;; 
		esac 
} 
function menu4 () { 
	# tls/openssl menu
	MenuLevel=4
	MenuName="TLS"
	PROMPT_2="@$HOSTNAME[$MenuName] : "
	let LOOPCNT++ 
	clear 
	menu_header 
	echo -en "\n#\tTLS MANAGEMENT ------------>\n" 
	echo -en "\t$(openssl version)" 
	echo -en "\n--> \tSelect action:\n 
	\tL - List established tls sessions 
	\t99 - Return\n\n" 
	#Planned Options (not implemented yet..) 
	#- Edit TLS Neigbour IP:PORT or FQDN:PORT 
	#- Test TLS Server 
	#- Read and verify certificates from TLS hosts 
	read -p "--> $MenuLevel$PROMPT_2" userchoice
	keypress=$(echo $userchoice | tr -d "\n") 
	Lastaction=$keypress 
		case $keypress in 
			[Ll] ) #List TLS Sessions 
				echo "not implemented ...."; 
				read
			;; 
			:* )
				# Cli
				pkitcli	
				;;
			99 ) # main menu 
				menu0 
				;; 
			esac 
} 
function menu5 () { 
	# candidate management 
	MenuLevel=5 
	MenuName="CAND"
	PROMPT_2="@$HOSTNAME[$MenuName] : "
	let LOOPCNT++ 
	clear 
	menu_header 
	echo -en "\n#\tExtended Candidate Management --->" 
	echo -en "\n--> \tSelect action:\n 
	\tM - Maintain current candidates file 
	\tQ - Request certificates from candidates 
	\tS - Sign certificates from candidates 
	\tR - Reissue / resign certificate request from DB (beta) 
	\tC - CRL Revoke (selective) 
	\tU - CRL UNrevoke (selective) 
	\tN - new crl (update) 
	\t---------------------------------------- 
	\tL - List signed Candidates DB
	\tE - Edit/Add external Candidate import 
	\tI - Import external Candidate req (X509) 
	\tX - Sign / Resign external Candidate requests 
	\tV - View signed imported Certificates 
	\t99 - Return\n\n" #### new options 
	read -p "--> $MenuLevel$PROMPT_2" userchoice 
	keypress=$(echo $userchoice | tr -d "\n") 
	Lastaction=$keypress 
	case $keypress in 
		[Mm] ) # edit hosts file
			edithosts; 
			;; 
		[Qq] ) #Generate certificates
			read_x509config;
			create_requests; 
			;; 
		[Ss] ) #sign candidates
			read_x509config;
			create_keys_certs;
			#UpdateCRL;			
			;; 
		[Rr] ) # Reissue / resign certificates from local DB 
			reissue_signed;	
			UpdateCRL
			;; 
		[Nn] ) # update CRL 
			echo "Recalculating CRL ....";sleep 2 
			UpdateCRL;sleep 2
			;; 
		[Cc] )# update CRL 
			RevokeCRL 
			UpdateCRL 
			;; 
		[Uu] ) # UNrevoke CRL 
			#UnrevokeCRL
			echo "-- sorry, not perfect implemented ..." 
			echo -en "to UNREVOKE certificates, we open the file $DATABASE\n" 
			echo -en "with vi and change R-lines to V-lines\n" 
			echo "Do you want to open that file ? Enter [y] or [n]" 
				if (yes_no); then 
					vi $DATABASE 
				fi 
				cat $DATABASE 
				UpdateCRL 
				;; 
		[Ll] ) #list signed candidates 
			ListCertIndex 
			;; 
		[Aa] ) #Add Candidate request
				#CandAddReq 
				;; 
		[Ee] ) # Edit CandReq File 
			editcandidates 
			;; 
		[Ii] ) # import candreq file 
			importcreq 
			;; 
		[Vv] ) # view certificate 
			viewcandcert
			;; 
		[Xx] ) # sign requests 
			read_x509config;
			SignRequest 
			;; 
		:* ) 
				# Cli
				pkitcli	
				;;
		99 ) # main menu 
			menu0 
			;; 
		esac 
}   
function menu6 () { 
	# CA management 
	MenuLevel=6
	MenuName="CA"
	PROMPT_2="@$HOSTNAME[$MenuName] : "
	let LOOPCNT++ 
	clear 
	menu_header 
	echo -en "\n#\tCA Management --->" 
	echo -en "\n--> \tSelect action:\n 
	\t1 - Edit X509 ROOT-CA
	\tW - Write rootCA empty database 
	\tI - Create new RootCA-Request interactively (more options) 
	\t2 - Create Root-CA 
	\t3 - Show X509 ROOT-CA Params
	\tCDH - ReCreate DH-File with $ROOTBITS and $CERTBITS
	\tQ - Create new RootCA-Request from this RootCA Cert
	\tA - Create p12 root-Certificate with key (passphrase)
	\tB - Replace root.crt with certificate from Enterprise PKI
	\tC - Perform a ROOTCA CHECK
	\tR - Create SUBCA Request
	\tS - Sign and create SUBCA
	\tD - Delete SUBCA
	
	
	echo -en " 
	\t4 - Change this Level to SUB-CA "
	echo -en "
	\t5 - Update CRL 
	\t---------------------------------------- 
	\t99 - Return\n\n" 
	read -p "--> $MenuLevel$PROMPT_2" userchoice 
	keypress=$(echo $userchoice | tr -d "\n") 
	Lastaction=$keypress 
	case $keypress in 
		[1] ) # Edit Root CA X509
			editx509
			read_x509config;
			;;
		[A] ) # Create a p12 keybag with passphrase
			create_root_p12;
			;; 
		[B] ) # replace root.crt with signed cert from enterprise PKI-Tool
			replace_rootcert;
			;;
		[C] ) # check rootCA
			check_rootCA;
			;;
		[R] ) # create SUBCA Request
			ask_subCAName;
			reqsubca;
			;;
		[S] ) # sign SUBCA request
			ask_subCAName;
			signsubca;
			;;
		[D]	) # delete SUBCA
			ask_subCAName;
			delsubca;
			;;
		[W] ) # create initial empty RootCA DATABASE
			create_rootCA_DB;
			;;
		[2] ) # Create CA 
			read_x509config;
			create_dh; 
			create_rootca;
			;; 
		[3] ) # show X509 Parameter 
			read_x509config;
			print_x509config 
			;; 
		[4] ) # create SUB CA
			changeto_subca;			
			;; 
		[5] ) # update CRL 
			UpdateCRL; 
			;; 
		[Q] ) # create RootCA Request from RootCA Certificate
			create_rootCA_Req;
			;;
		
		[I] ) # create RootCA Request interactive
			create_rootCA_Req_IA;
			;;
		CDH ) # recreate DH Keys with lenght from config
			recreate_dh;
			;;
		:* )
			# Cli
			pkitcli	
			;;
		99 ) # main menu 
			menu0 
			;; 
		esac 
} 

function menu7 () { 
	# CA management 
	MenuLevel=7
	MenuName="RESTORE"
	PROMPT_2="@$HOSTNAME[$MenuName] : "
	let LOOPCNT++ 
	clear 
	menu_header 
	echo -en "\n#\tRESTORE --->" 
	echo -en "\n--> \tSelect action:\n 
	\tC - Change Restore Config
	\tR - Start Restore
	\t---------------------------------------- 
	\t99 - Return\n\n" 
	read -p "--> $MenuLevel$PROMPT_2" userchoice 
	keypress=$(echo $userchoice | tr -d "\n") 
	Lastaction=$keypress 
	case $keypress in 
		[C] ) # Edit Root CA X509
			echo "Restore Config"
			pki_restore_conf
			;;
		[R] ) # Create a p12 keybag with passphrase
			pki_restore			
			;; 
		
		:* )
			# Cli
			pkitcli	
			;;
		99 ) # main menu 
			menu0 
			;; 
		esac 
}
 
function menu_header () { # common menu header 
	if [ $(whoami) != 'srx' ]; then 
		#echo -en "\t!!!! --- On OSV login as srx --- !!!!" 
		echo -en "\n\t --- You are user $(whoami) ---" 
		#echo -en "\n\t ------- Limited functionality ------" 
		#echo -en "\n\t --> Toogle OSV MODE with 'OSV'\n" 
	fi 
	if [ $DEBUGMODE == 1 ]; then 
		echo -en " !!!!! Running OSV MODE !!!!!\n" 
	fi 
	echo -en " \n$BANNER\n\n" 
	#echo -en "------ UNIFY PKI HELPER $Version FOR OPENSCAPE ------\n\n" 
	# echo -en "\t-----$THISHOST on $osvServerType------\n" 
	if [ $osvServerType = 'simplex' ]; then 
		echo -en "\t------ OSV SIMPLEX SYSTEM -------\n" 
		echo -en "\tAction Counter --> $LOOPCNT <-- last action: $Lastaction" 
	fi 
	if [ $osvServerType = 'duplex' ]; then 
		echo -en "\t------ OSV DUPLEX SYSTEM -------\n" 
		echo -en "\tAction Counter --> $LOOPCNT <-- last action: $Lastaction" 
	fi 
	if [ $osvServerType = 'Linux' ]; then 
		echo -en "\t------ Linux SYSTEM -------\n" 
		echo -en "\tAction Counter --> $LOOPCNT <-- last action: $Lastaction" 
		fi 
} 
