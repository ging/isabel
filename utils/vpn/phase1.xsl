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
<!-- PHASE 1: Find this node and create the connection with the mcu node and with the master-->
<!-- Revision History:
         Versión 0.1: 
              - Initial Script: Only IPv6 support. 
              - MASTER is defined by ID.
-->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">
   <!--Entry param-->
	<xsl:param name="myIdentity"/>   
	<xsl:param name="certdir"/>	
   <!-- Output method-->
	<xsl:output method="text"/>

<!-- Header and default parameter. -->
<xsl:template match="/">
<xsl:text># ipsec.conf
# KAME IPv6 - IPSEC- IKE Config.
# This file configure Security Policy Database (SPD) with the setkey utility
# European IPv6 Internet Exchanges Backbone Project

# Flush the SAD and SPD entries
flush;
spdflush;</xsl:text>	
<!-- Search this node and create connection with mcu -->
<xsl:apply-templates  select="EDL/site-description"/></xsl:template>
<xsl:template match="SITE">
     <!--Is it my SITE definition?-->
     <!-- Yes -->
     <xsl:if test="site-identification/CERT_IDENTITY/DN = $myIdentity or site-identification/CERT_IDENTITY/FQDN = $myIdentity or site-identification/SITE_ADDRESS = $myIdentity or site-identification/ID = $myIdentity">
       <!-- MASTER is a special case. -->       
	<xsl:choose>
		<xsl:when test="CONNECTION-MODE/mcu/MCU_ADDRESS">
                	<!-- Support IPSEC? -->
              		<xsl:choose>
              	        	<!-- Yes -->
				<xsl:when test="site-identification/CERT_IDENTITY">							     
				       	<!-- Create the connection with mcu or master. -->
				       	<xsl:call-template name="find-mcu-master">
						<xsl:with-param name="MCU_ADDRESS">
						       	<xsl:value-of select="CONNECTION-MODE/mcu/MCU_ADDRESS"/>
						</xsl:with-param>				     
				 		<xsl:with-param name="left">
			 		     		<xsl:value-of select="site-identification/SITE_ADDRESS"/>
			 			</xsl:with-param>
					</xsl:call-template>				 
				</xsl:when>
			</xsl:choose>                    
		</xsl:when>
		<xsl:when test="CONNECTION-MODE/unicast/PEER_ADDRESS">
                        <!-- Support IPSEC? -->
                        <xsl:choose>
                                <!-- Yes -->
                                <xsl:when test="site-identification/CERT_IDENTITY">
                                        <!-- Create the connection with mcu or master. -->
                                        <xsl:call-template name="find-mcu-master">
                                                <xsl:with-param name="MCU_ADDRESS">
                                                        <xsl:value-of select="CONNECTION-MODE/unicast/PEER_ADDRESS"/>
                                                </xsl:with-param>
                                                <xsl:with-param name="left">
                                                        <xsl:value-of select="site-identification/SITE_ADDRESS"/>
                                                </xsl:with-param>
                                        </xsl:call-template>
                                </xsl:when>
                        </xsl:choose>
                </xsl:when>
	</xsl:choose>               
     </xsl:if> 
</xsl:template>

<!-- Search the mcu with MCU_ADDRESS into site's set or master node-->
<xsl:template name="find-mcu-master"> 
	   <xsl:param name="MCU_ADDRESS"/>
	   <xsl:param name="left"/>
	   <xsl:for-each select="/EDL/site-description/SITE">
	     <!-- mcu NODE or MASTER NODE-->
		<xsl:if test="site-identification/SITE_ADDRESS = $MCU_ADDRESS">
		<!-- Support IPSEC and CERT ? -->
		   <xsl:if test="IPSEC_CONF and site-identification/CERT_IDENTITY">
		   <!-- Ok. Connection thisNode-mcu OR thisNode-master-->
			<!-- SA =  This node-MCU-->
			      <xsl:text>
spdadd </xsl:text>
				<!-- Left subnet -->
		      		<xsl:value-of select="$left"/>	<xsl:text>/128 </xsl:text>
		
				<!-- Right Subnet -->
				<xsl:value-of select="site-identification/SITE_ADDRESS"/><xsl:text>/128</xsl:text>
				<!-- Protocol -->
				<xsl:text> any -P out ipsec</xsl:text>
				<xsl:text>
       esp/tunnel/</xsl:text>
				<!-- Left -->
				<xsl:value-of select="$left"/><xsl:text>-</xsl:text>
				<!-- Right -->
				<xsl:value-of select="site-identification/SITE_ADDRESS"/>
				<!-- Protocol -->
				<xsl:text>/require;
</xsl:text>
			<!-- SA =  MCU-This-node-->
				<xsl:text>
spdadd </xsl:text>
				<!-- Right Subnet -->
				<xsl:value-of select="site-identification/SITE_ADDRESS"/><xsl:text>/128 </xsl:text>	
				<!-- Left subnet -->
		      		<xsl:value-of select="$left"/>	<xsl:text>/128</xsl:text>		
		
				<!-- Protocol -->
				<xsl:text> any -P in ipsec</xsl:text>
				<xsl:text>
       esp/tunnel/</xsl:text>
				<!-- Right -->
				<xsl:value-of select="site-identification/SITE_ADDRESS"/><xsl:text>-</xsl:text>
				<!-- Left -->
				<xsl:value-of select="$left"/>
				<!-- Protocol -->
				<xsl:text>/require;
</xsl:text>				
		  </xsl:if>	          
	       </xsl:if>
	   </xsl:for-each>
</xsl:template>
</xsl:stylesheet>
