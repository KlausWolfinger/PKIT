# batcher.inc
function batcher(){
# echo "Parameter : $Param1"
echo


case "$Param1" in
	-x)
	#-------------------------
	echo "Execute $Param1 $Param2  ..."
	case "$Param2" in
		# ================
		-request)
		echo "Requesting ..."
		#
		;;
		# ================
		-read)
		echo "Reading ..."
		#
		;;
		# ================
		-clean)
		echo "Cleaning $Param3 ..."
		CleanupProject;
		;;
		# ================
		-signall)
		echo "Signing all Candidates ..."
		create_keys_certs;
		;;
		# ================
		-addcand)
		echo "Adding Candidate to candidates file ..."
		newcand=$Param3
		addcand;
		;;
		# ================
		-revoke)
		echo "Revoking Candidate with Name $Param3 ..."
		extension=$Param3
		CN=$Param4
		FastRevoke;
		
		#
		;;
		# ================
		-dbout)
		echo "Printing Database ..."
		ListCertIndex;
		#
		;;
		# ================
		-printhosts)
		echo "Printing Candidates ..."
		PrintActiveHosts
		;;	
		# ================
		-reqsubca)
		echo "Generating SUB-CA Request and DB...";
		SUBCANAME=$Param3
		reqsubca;
		;;	
		# ================
		-signsubca)
		echo "Signing SUB-CA by Root CA ...";
		SUBCANAME=$Param3
		signsubca;
		;;	
		# ================
		-delsubca)
		echo "Removing SUB-CA ...";
		SUBCANAME=$Param3
		delsubca;
		;;	
		# ================
		-revokesubca)
		echo "Removing SUB-CA ...";
		SUBCANAME=$Param3
		SubCARevoke;
		;;			
		*)
		echo "bad command ($Param2) ..."
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
clear
echo "--- pkit Cli ---"
read
# CA management 
	clear 
	case $keypress in 
		
		[:]* ) 
			# Cli
			echo "command: $keypress"
			read -p "ENTER to cont"			
			;;
		*) # main menu 
			return 
			;; 
		esac 
}