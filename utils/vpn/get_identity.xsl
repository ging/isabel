<?xml version="1.0" encoding="UTF-8"?>
<!--
 - ISABEL: A group collaboration tool for the Internet
 - Copyright (C) 2009 Agora System S.A.
 -
 - This file is part of Isabel.
 -
 - Isabel is free software: you can redistribute it and/or modify
 - it under the terms of the Affero GNU General Public License as published by
 - the Free Software Foundation, either version 3 of the License, or
 - (at your option) any later version.
 -
 - Isabel is distributed in the hope that it will be useful,
 - but WITHOUT ANY WARRANTY; without even the implied warranty of
 - MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 - Affero GNU General Public License for more details.
 -
 - You should have received a copy of the Affero GNU General Public License
 - along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 -
-->
<!-- This XSL obtain the DN of this node -->
<!-- Revision History:
         Versión 0.1: 
              - Initial Script: Only IPv6 support. 
-->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">
   <!--Entry param-->
	<xsl:param name="myIdentity"/>   
   <!-- Output method-->
	<xsl:output method="text"/>

<xsl:template match="/">

<xsl:apply-templates select="/EDL/site-description/SITE[site-identification/CERT_IDENTITY/DN = $myIdentity or site-identification/CERT_IDENTITY/FQDN = $myIdentity or site-identification/SITE_ADDRESS = $myIdentity or site-identification/ID = $myIdentity]"/>	
</xsl:template>

<xsl:template match="SITE">
    <xsl:choose>	
         <xsl:when test="site-identification/CERT_IDENTITY/DN">
	        <xsl:text>"</xsl:text><xsl:value-of select="site-identification/CERT_IDENTITY/DN"/><xsl:text>"</xsl:text>
         </xsl:when>
         <xsl:when test="site-identification/CERT_IDENTITY/FQDN">
	         <xsl:text>+</xsl:text><xsl:value-of select="site-identification/CERT_IDENTITY/FQDN"/><xsl:text>+</xsl:text>
         </xsl:when>         
     </xsl:choose>         
</xsl:template>

</xsl:stylesheet>
