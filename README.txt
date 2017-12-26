PKI-TOOL README
(c) Klaus Dieter Wolfinger
Only for Classroom Training 
README
======
HowTo: Installation
Linux Distribution:
# unzip the tool 
# chmod u+x *.sh
# edit ./host/hosts
# edit ./pkiconf/x509.inf
# ./pki_unify.sh

OpenScape Voice Server:
# Copy the zip to sysad account (SFTP)
# login as sysad
# chmod zipfile to 660
# switch user to srx (su - srx)
# make dir pki
# move zip to ~/pki
# cd pki; unzip zipfile
# edit ./host/hosts
# edit ./pkiconf/x509.inf
# ./pki_unify.sh

HowTo: Create Cert-Requests with PKI-TOOL


HowTo: Sign a Cert-Request with PKI-TOOL

### Generate a new RSA key
1. openssl  genrsa -out keyfile.key 2048
### Generate a new Sign Request, use the keyfile
### A OpenSSL config file is required to specify the X509 Extensions 
2. openssl req -new -nodes -days 365 -newkey rsa:2048 -keyout keyfile.key -out requestfile.req -config /usr/local/ssl/server.cnf -reqexts v3_req
### Extract the public key from the request file
3. openssl req -in requestfile.req -pubkey -noout >kv500.pub
### Prepare the PKI-TOOL : Replace FQDN with your unique system name
4. copy all keys to pki-tool/keys/FQDN.key und tool/keys/FQDN.pub
5. copy req file to pki-tool/certs/FQDN.req
6. in tool/host/hosts edit: replace FQDN, DNS and IP with your parameters
   tlsboth:FQDN,DNS:dnssrvalias,DNS:sipsm1,IP:4.4.4.4,IP:3.3.3.3
   ---> all Issuer's FQDNs and IP addresses should be inserted
   e.g: tlsboth:test-osb01-yxz,IP:1.2.3.4,DNS:test-osb01-yxz.munich.de
7. start tool: a) if rootca doesn not exist, create new b) use exising rootca
8. Generate certificates with pki-tool
9. dir tool/certs/*.pem
### Test: check pemfiles
openssl req -text -noout -in FQDN.req
openssl x509 -text -noout -in FQDN.pem  
