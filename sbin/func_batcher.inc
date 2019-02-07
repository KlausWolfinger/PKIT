# batcher.inc
function batcher(){
# echo "Parameter : $Param1"
echo


case "$Param1" in
	-x)
	#-------------------------
	echo "# Execute $Param1 $Param2  ..."
	case "$Param2" in
		# ================
		-request)
		echo "# Requesting ..."
		#
		;;
		# ================
		-read)
		echo "# Reading ..."
		#
		;;
		# ================
		-clean)
		echo "# Cleaning $Param3 ..."
		CleanupProject;
		;;
		# ================
		-signall)
		echo "# Signing all Candidates ..."
		create_keys_certs;
		;;
		# ================
		-addcand)
		echo "# Adding Candidate to candidates file ..."
		newcand=$Param3
		addcand;
		;;
		# ================
		-revoke)
		echo "# Revoking Candidate with Name $Param3 ..."
		extension=$Param3
		CN=$Param4
		FastRevoke;
		
		#
		;;
		# ================
		-dbout)
		echo "# Printing Database ..."
		ListCertIndex;
		#
		;;
		# ================
		-printhosts)
		echo "# Printing Candidates ..."
		PrintActiveHosts
		;;	
		# ================
		-reqsubca)
		echo "# Generating SUB-CA Request and DB...";
		SUBCANAME=$Param3
		reqsubca;
		;;	
		# ================
		-signsubca)
		echo "# Signing SUB-CA by Root CA ...";
		SUBCANAME=$Param3
		signsubca;
		;;	
		# ================
		-delsubca)
		echo "# Removing SUB-CA ...";
		SUBCANAME=$Param3
		delsubca;
		;;	
		# ================
		-revokesubca)
		echo "# Removing SUB-CA ...";
		SUBCANAME=$Param3
		SubCARevoke;
		;;			
		*)
		echo "# bad command ($Param2) ..."
		esac
		
	
	
	#-------------------------
	;;
	
	*)
	echo -en " 
	Helper --->
	$0 -h	-	This help 
	$0 -x	-	execute action
	
	ACTIONS
	=======
	-request		- create certificate request
					Parameter: CN=\"OUI=mycompany, C=DE\"
	-clean 			- clean project
					Parameter: projectname [root|...]
	-addcand		- add a candidate to database
					Parameter: [SYSTEM:ALIAS;IP:1.2.3.4,DNS:www,host.com]
	-revoke			- revoke certificate
					Parameter: [Extension CN]
	-dbout			- print root CA Database
	-printhosts		- print candidates file
	-reqsubca		- creates a subca Request
					Parameter [name]
	-signsubca		- signed the subca 
					Parameter [name]
	-delsubca		- removes a subca
					Parameter [name]
	-revokesubca		- revoke subca
					Parameter [name]
	
	"
	exit 0;
	
	
esac;
}
	
function pkitcli() {
#
CLIcommand=$(echo "$keypress" | cut -d ":" -f 2)
clear
echo "PKIT -- commander"
echo "-----------------------------------------"
# CA management 
	case $keypress in 
	
		[:]lastlog )
			# print lastlog
			less $LASTLOG
			;;
		[:]clearlastlog )
			# clear lastlog.log file
			echo "file cleared" >$LASTLOG
			;;	
		[:]ip* ) 
			# Cli
			echo "command: $CLIcommand"
			eval $CLIcommand 2>/dev/null
			read -p "ENTER to cont"			
			;;
		[:]ls* ) 
			# Cli
			echo "command: $CLIcommand" 
			eval $CLIcommand | less 2>/dev/null
						
			;;
		[:]tree* ) 
			# Cli
			echo "command: $CLIcommand" 
			eval $CLIcommand | less 2>/dev/null
			;;
		[:]restore ) 
			# Cli
			menu7			
			;;
		[:]listvars )
			# list all variables
			ListVars
			;;
		[:]removeRootkeyPass )
		    #  remove_rootcaKey_passphrase
			remove_rootcaKey_passphrase
			;;
		[:]setRootkeyPass )
			# sets a passphrase for root key
			set_rootcaKey_passphrase
			;;
		[:]set* )
			# change bash option set +x for debugging
			echo "command: $CLIcommand" 
			eval $CLIcommand | less 2>/dev/null
			read -p "Debugging toggled -> ENTER to cont"	
			;;
		[:]show* )
			# show x509 of this rootca
			echo "command: $CLIcommand" 
			if [ $CLIcommand = 'showrootca' ]; then
			ShowRootCA;
			else 
			echo "## $CLIcommand not supported command"
			echo "## ENTER to cont ..."
			read
			fi
			;;
		[:][?h] )
			# change bash option set +x for debugging
			echo "command list" 
			echo "------------"
			echo -e ":listvars - List PKIT variables"
			echo -e ":ls [directory]  - List [directory]"
			echo -e ":showrootca - Shows Root Certificate"
			echo -e ":restore - Open Restore Dialog"
			echo -e ":lastlog - Show logfile"
			echo -e ":removeRootkeyPass - DANGEROUS passphrase remover"
			echo -e ":setRootkeyPass - DANGEROUS sets new passphrase"
			
			echo
			echo "PKIT workshop recommended"
			echo "Information: www.itconsulting-wolfinger.de!"
			
			read -p "ENTER to cont"	
			;;	
		*) # main menu 
			echo -en "\n#####################################################\n"
			echo "# you entered an invalid command: \"$keypress\""
			echo "#####################################################"
			read -p "ENTER to cont ..."	
			return 
			;; 
		esac 
}