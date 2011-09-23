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
<!-- PHASE 3: Create the connections of the MASTER -->
<!-- Revision History:
         Versión 0.1: 
              - Initial Script: Only IPv6 support. 
-->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">
   <!--Entry param-->
	<xsl:param name="myIdentity"/>   
	<xsl:param name="certdir"/>
   <!-- Output method-->
	<xsl:output method="text"/>

<xsl:template match="/">
<!-- Search this node configuration -->
<xsl:for-each select="/EDL/site-description/SITE">
	<xsl:if test="site-identification/CERT_IDENTITY/DN = $myIdentity or site-identification/CERT_IDENTITY/FQDN = $myIdentity or site-identification/SITE_ADDRESS = $myIdentity or site-identification/ID = $myIdentity">			

	<!-- Is the MASTER -->
             <xsl:variable name="isMaster">
                 <xsl:choose>
                        <xsl:when test="CONNECTION-MODE/mcu/MCU_ADDRESS">
                                <xsl:text>no</xsl:text>
                        </xsl:when>
                        <xsl:when test="CONNECTION-MODE/unicast/PEER_ADDRESS">
                                <xsl:text>no</xsl:text>
                        </xsl:when>
                        <xsl:when test="CONNECTION-MODE/multicast/multicast-parameters/MCAST_ROOT">
                                <xsl:text>no</xsl:text>
                        </xsl:when>
                        <xsl:otherwise>
                                <xsl:text>yes</xsl:text>
                        </xsl:otherwise>
                 </xsl:choose>
             </xsl:variable>

		<!-- MASTER ID -->
             <xsl:variable name="masterID">
		<xsl:choose>
			<xsl:when test="$isMaster = 'yes'">
				<xsl:copy-of select="$myIdentity"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:text> </xsl:text>
			</xsl:otherwise>
		</xsl:choose>
	     </xsl:variable>

  <!-- Is it this node the master and IPSEC/CERT is supported? -->
  <xsl:if test="$isMaster = 'yes' and IPSEC_CONF and site-identification/CERT_IDENTITY">
	<!-- Search all nodes, except the master -->
	<xsl:apply-templates select="/EDL/site-description/SITE[$masterID != site-identification/ID]">
		   <xsl:with-param name="right"><xsl:value-of select="site-identification/SITE_ADDRESS"/></xsl:with-param>
	</xsl:apply-templates>
    </xsl:if>	
	       
	 </xsl:if>	   
</xsl:for-each>
<xsl:text>
</xsl:text>
</xsl:template>


<xsl:template match="SITE">
    <xsl:param name="isMaster"/>
    <xsl:param name="right"/>
	 	<!-- Required: IPSEC and CERT -->		
	         <xsl:if test="site-identification/CERT_IDENTITY">
			<!-- SA =  MCU-This-node-->
				<xsl:text>
spdadd </xsl:text>
				<!-- Right Subnet -->
				<xsl:value-of select="$right"/><xsl:text>/128 </xsl:text>	
				<!-- Left subnet -->
		      		<xsl:value-of select="site-identification/SITE_ADDRESS"/><xsl:text>/128</xsl:text>		
		
				<!-- Protocol -->
				<xsl:text> any -P out ipsec</xsl:text>
				<xsl:text>
       esp/tunnel/</xsl:text>
				<!-- Right -->
				<xsl:value-of select="$right"/><xsl:text>-</xsl:text>
				<!-- Left -->
                           <xsl:value-of select="site-identification/SITE_ADDRESS"/>				
				<!-- Protocol -->
				<xsl:text>/require;
</xsl:text>				
			<!-- SA =  This node-MCU-->
			      <xsl:text>
spdadd </xsl:text>
				<!-- Left subnet -->
		      		<xsl:value-of select="site-identification/SITE_ADDRESS"/><xsl:text>/128 </xsl:text>		
				<!-- Right Subnet -->
				<xsl:value-of select="$right"/><xsl:text>/128</xsl:text>
				<!-- Protocol -->
				<xsl:text> any -P in ipsec</xsl:text>
				<xsl:text>
       esp/tunnel/</xsl:text>
				<!-- Left -->
				<xsl:value-of select="site-identification/SITE_ADDRESS"/><xsl:text>-</xsl:text>
				<!-- Right -->
				<xsl:value-of select="$right"/>
				<!-- Protocol -->
				<xsl:text>/require;
</xsl:text>

 			</xsl:if>


</xsl:template>

</xsl:stylesheet>
