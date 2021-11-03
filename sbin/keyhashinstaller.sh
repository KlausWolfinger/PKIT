File=/opt/siemens/share/tomcat/conf/server.xml
Line=$(egrep -n "tomcat-keystore.p12" $File | cut -d":" -f1)
LineNr=$(( $Line + 1 ))


PassHash=$(/opt/siemens/servicetools/cmp/encodePassphrase.sh 12345678)
NewLine='keystorePass="'$PassHash'"'

sed -i -e $(echo $LineNr)c"$NewLine" $File


File=/opt/siemens/common/conf/application-external/trust-store.properties
LineNr=$(egrep -n "keystorePass" $File | cut -d":" -f1)
sed -i -e $(echo $LineNr)c"$NewLine" $File

echo "---done--- good luck :-)"
