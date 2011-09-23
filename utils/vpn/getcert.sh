#! /bin/sh
########################################################################
#
#  ISABEL: A group collaboration tool for the Internet
#  Copyright (C) 2009 Agora System S.A.
#  
#  This file is part of Isabel.
#  
#  Isabel is free software: you can redistribute it and/or modify
#  it under the terms of the Affero GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#  
#  Isabel is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  Affero GNU General Public License for more details.
# 
#  You should have received a copy of the Affero GNU General Public License
#  along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
#
########################################################################

## (Conexiones con MCU) Hijos left - MCU right
## (Conexiones con Master) Hijos left - Master right

# version para dig 9.2.3

## Parámetro de entrada: 
##  DN del nodo 

CERTDIR=/etc/ipsec.d/certs
CADIR=/etc/ipsec.d/certs
CRLDIR=/etc/ipsec.d/certs
LDAPSERVER=pki.umu.euro6ix.org
CERTNAME=mycert.pem
CRLNAME=crls.pem
CRLNAMESERVER=crl-Euro6IX+Root+PKI+IV.sigz.umu.euro6ix.org

if [ -z "$1" ] ; then
        echo "Error: no parameters";
        exit 1;
fi

## 1. Descargo certificado CA

if echo $1 | grep -q "^+"; then 
	# echo "MYFQDN: "$1;

	FQDN=`echo $1 | sed 's/+//g'`;

# Se descarga el certificado si no existe ya
if [ ! -e "$CERTDIR/$CERTNAME" ] ; then    		
	   echo "-----BEGIN CERTIFICATE-----" > /tmp/$CERTNAME.$$
	   dig CERT $FQDN | sed -n "/^$FQDN.*CERT./p" | awk ' {print substr($0,index($0,"MII"))}' | xargs -n1 >> /tmp/$CERTNAME.$$ 
	   echo "-----END CERTIFICATE-----" >> /tmp/$CERTNAME.$$;

  	   openssl x509 -in /tmp/$CERTNAME.$$ -out $CERTDIR/$CERTNAME -inform pem -outform pem
	   if [ $? = 1 ]; then
		echo "Error: Certificate ($FQDN) not found";
		exit 1;
	   fi

	fi
        MYDN=`openssl x509 -in $CERTDIR/$CERTNAME -subject | sed '/^subject= *\//!d; s///;q' | sed 's/\//,/g'`;
else
	MYDN=$1;
fi

# echo "MYDN: "$MYDN;

echo $MYDN | sed 's/"//g' | awk -F, {'print $1; print $2; print $3; print $4'} | sed 's/^[[:space:]]*//g' | { while read field; do
		value=`echo $field | cut -f2 -d"="`
		case $field in
			C=*|c=*)
				C=$value;
			;;
			CN=*|cn=*)
				CN=$value;
			;;
			OU=*|ou=*)
				OU=$value;
			;;
			O=*|o=*)
				O=$value;
			;;
			*)
			;;
		esac
	done;

        if [ ! -e $CADIR/ca.cer ]; then         
	    CADER=`ldapsearch -h $LDAPSERVER -b "ou=$OU,o=$O,c=$C" "(cn=*)" caCertificate -x -t | sed -n -e '/file:/p' | sed -e 's/cACertificate;binary:< file://g'`
	    if [ -z "$CADER" ]; then
		echo "Error: CA certificate not found";
		exit 1;
	    fi
            openssl x509 -in $CADER -out $CADIR/ca.cer -inform der -outform pem
        fi

        # Enlace simbólico necesario para KAME
        CALINK=$CADIR/`openssl x509 -noout -hash -in $CADIR/ca.cer`.0
        if [ ! -e $CALINK ]; then         
           ln -s $CADIR/ca.cer $CALINK
        fi
}

if [ $? = 1 ]; then
	exit 1;
fi

## 2. Descargo las CRL's desde la PKI

 
# echo "MYFQDN: "$1;
########################################################################################
#echo "-----BEGIN X509 CRL-----" > /tmp/$CRLNAME.$$
#dig CERT $CRLNAMESERVER | sed -n "/^$CRLNAMESERVER.*CERT./p" | awk ' {print substr($0,index($0,"MII"))}' | xargs -n1 >>/tmp/$CRLNAME.$$ 
#echo "-----END X509 CRL-----" >> /tmp/$CRLNAME.$$;

#openssl crl -in /tmp/$CRLNAME.$$ -out $CRLDIR/$CRLNAME -inform pem -outform pem
#if [ $? = 1 ]; then
#   echo "Error: CRL's ($CRLNAMESERVER) not found.";
#   exit 1;
#fi

# Enlace simbólico necesario para KAME
#CRLLINK=$CRLDIR/`openssl crl -noout -hash -in $CRLDIR/$CRLNAME`.r0
#if [ ! -e $CRLLINK ]; then         
#   ln -s $CRLDIR/$CRLNAME $CRLLINK
#fi

#if [ $? = 1 ]; then
#	exit 1;
#fi

exit 0;
